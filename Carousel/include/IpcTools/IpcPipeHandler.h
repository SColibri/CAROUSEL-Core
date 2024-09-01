#pragma once
#include <string>
#include <sstream>
#include <future>
#include <Poco/Pipe.h>
#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
#include "../Core/Interfaces/Ipc.h"
#include "../Logging/CarouselLogger.h"

#ifdef _WIN32
#include <Windows.h>
#elif __unix__
// TODO: Implement unix IPC
#endif

namespace carousel
{
	namespace ipcTools
	{
		constexpr int BUFFER_SIZE = 1024;
		constexpr int TIMEOUT = 15;

#ifdef _WIN32
		typedef HANDLE SystemHandle;
#elif __unix__
		typedef int SystemHandle;
#endif // OS

		/// <summary>
		/// IPC communication using pipes for applications that use stdout (e.g. console applications)
		/// </summary>
		class IpcPipeHandler : public Ipc
		{
		private:
			/// <summary>
			/// Path to executable to which communication is established
			/// </summary>
			const std::string _pathToExe;

			/// <summary>
			/// Flag that determines the end of the message
			/// </summary>
			const std::string _endFlag;

			/// <summary>
			/// Parameters used for starting a process
			/// </summary>
			const std::string _parameters;

			/// <summary>
			/// Timeout for receiving data, in seconds
			/// </summary>
			int _timeout{ TIMEOUT };

			/// <summary>
			/// Dummy command
			/// </summary>
			const std::string _dummyCommand{ " " };

			/// <summary>
			/// Send and receive retries
			/// </summary>
			int _maxRetries{ 2 };

			/// <summary>
			/// Pointer to process handle
			/// </summary>
			Poco::ProcessHandle* _processHandle{ nullptr };

			/// <summary>
			/// Out pipe stream
			/// </summary>
			Poco::PipeOutputStream* _outStream{ nullptr };

			/// <summary>
			/// In pipe stream
			/// </summary>
			Poco::PipeInputStream* _inStream{ nullptr };

			/// <summary>
			/// In pipe object
			/// </summary>
			Poco::Pipe _pipeIn;

			/// <summary>
			/// Out pipe
			/// </summary>
			Poco::Pipe _pipeOut;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			IpcPipeHandler(std::string pathToExe, std::string endFlag, std::string parameters = "", int timeout = TIMEOUT) :
				_pathToExe(std::move(pathToExe)),
				_parameters(std::move(parameters)),
				_endFlag(std::move(endFlag)),
				_timeout(timeout)
			{
				// Launch process and get handles
				Poco::ProcessHandle ph = Poco::Process::launch(
					_pathToExe,						   // Path to application
					{ _parameters },					   // Parameters to be passed at application start
					{},                                // Environment variables
					&_pipeIn, &_pipeOut,               // Connect pipes to process stdin and stdout
					nullptr                            // Working directory
				);
				_processHandle = new Poco::ProcessHandle(ph);

				// Create streams
				_outStream = new Poco::PipeOutputStream(_pipeIn);
				_inStream = new Poco::PipeInputStream(_pipeOut);
			}

			/// <summary>
			/// Destructor
			/// </summary>
			~IpcPipeHandler()
			{
				clearResources();

				// Remove all resources
				delete _inStream;
				delete _outStream;
				delete _processHandle;

				_inStream = nullptr;
				_outStream = nullptr;
				_processHandle = nullptr;
			}

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			bool isReady()
			{
				// Check if stream are initialized
				if (_outStream == nullptr || _inStream == nullptr)
				{
					return false;
				}

				// Check if handles are still valid
				if (!_outStream->good() || !_inStream->good())
				{
					return false;
				}

				return true;
			}

			/// <summary>
			/// Send command
			/// </summary>
			/// <param name="command"></param>
			/// <returns></returns>
			std::string sendCommand(const std::string& command)
			{
				if (!isReady()) throw std::exception("Communication channel is not ready");
				if (command.length() == 0) throw std::exception("Empty command");

				std::string response;
				for (int retry = 0; retry <= _maxRetries; retry++)
				{
					auto commandStatus = writeTask(command);

					if (std::get<0>(commandStatus) == std::future_status::ready)
					{
						// Success
						response = std::get<1>(commandStatus);
					}
					else
					{
						// Timeout error
						response = "IpcPipeHandler: Command could not be sent, timeout(" + std::to_string(_timeout) + ") has been raised.";
						carousel::logging::CarouselLogger::instance().error(response);

						// Try flush the read pipe
						auto testy = readTask();

						// Retry count
						if (retry == _maxRetries)
						{
							clearResources();
						}
					}
				}
				return response;
			}


			std::string readCommand()
			{
				if (!isReady()) throw std::exception("Communication channel is not ready");

				auto responseRead = readTask();
				std::string response;

				return std::get<1>(responseRead);
			}


		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="in"></param>
			/// <param name="promise"></param>
			static void read(Poco::PipeInputStream* in, std::promise<std::string>&& promise) {
				try {
					std::string result;
					int retryCount = 0;
					while (in->good())
					{
						char buffer[1024];
						std::memset(buffer, '\0', 1024);
						//in->getline(buffer, sizeof(buffer));
						
						in->read(buffer, 1024);
						result.append(buffer);

						auto positionFlag = result.rfind("MC: ");
						if (positionFlag != std::string::npos && positionFlag + 4 == result.length())
						{
							break;
						}
						else
						{
							//retryCount = tasty4 < 1 ? retryCount + 1 : 0;
							std::this_thread::sleep_for(std::chrono::milliseconds(100));
						}

					}
					promise.set_value(result);
				}
				catch (...) {
					promise.set_exception(std::current_exception());
				}
			}

			static void send(Poco::PipeOutputStream* outStream, std::promise<std::string>&& promise, std::string command)
			{
				(*outStream) << command.c_str() << std::endl;

				outStream->flush();

				promise.set_value("Command sent");
			}

			std::tuple<std::future_status, std::string> readTask()
			{
				// Setup the task
				std::promise<std::string> promise;
				std::future<std::string> future = promise.get_future();

				// Start send command
				std::thread readThread(read, std::ref(_inStream), std::move(promise));

				// Use timeout
				std::future_status status = future.wait_for(std::chrono::seconds(_timeout));
				std::string response;
				int retryCount{ 0 };
				bool runExecution{ true };
				do
				{
					if (status == std::future_status::ready) {
						try {
							// Success
							response = future.get();
							carousel::logging::CarouselLogger::instance().Info("IpcPipeHandler: Reading the content of the pipe was not possible.");
							readThread.join();
						}
						catch (const std::exception& e) {
							// Future error
							carousel::logging::CarouselLogger::instance().error("IpcPipeHandler: Attempting to read the pipe throwed an exception -" + std::string(e.what()));
						}
						runExecution = false;
					}
					else
					{
						// Update retry count
						retryCount++;
						runExecution = retryCount < _maxRetries;

						// Something went wrong, maybe it's blocked by write access so try to clear it
						if (runExecution)
						{
							sendDummyCommand();

						}
						else
						{
							// nothing worked
							clearResources();
						}
					}
				} while (runExecution);

				return std::tuple<std::future_status, std::string>(status, response);
			}

			void sendDummyCommand()
			{
				// Setup the task
				std::promise<std::string> promise;
				std::future<std::string> future = promise.get_future();

				// Start send command
				std::thread readThread(send, std::ref(_outStream), std::move(promise), _dummyCommand);

				// Set a timeout period (e.g., 5 seconds)
				std::future_status status = future.wait_for(std::chrono::seconds(_timeout));
				std::string response;
				if (status == std::future_status::ready) {
					try {
						// Success
						response = future.get();
						carousel::logging::CarouselLogger::instance().Info("IpcPipeHandler: Command -" + _dummyCommand + "- was sent via IPC.");
						readThread.join();
					}
					catch (const std::exception& e) {
						// Future error
						carousel::logging::CarouselLogger::instance().error("IpcPipeHandler: Command could not be sent -" + std::string(e.what()));
					}
				}
				else
				{
					carousel::logging::CarouselLogger::instance().error("IpcPipeHandler: Sending dummy command timed-out.");
				}
			}

			std::tuple<std::future_status, std::string> writeTask(const std::string& command)
			{
				// Setup the task
				std::promise<std::string> promise;
				std::future<std::string> future = promise.get_future();

				// Start send command
				std::thread readThread(send, std::ref(_outStream), std::move(promise), command);

				// Set a timeout period (e.g., 5 seconds)
				std::future_status status = future.wait_for(std::chrono::seconds(_timeout));
				std::string response;
				if (status == std::future_status::ready) {
					try {
						// Success
						response = future.get();
						carousel::logging::CarouselLogger::instance().Info("IpcPipeHandler: Command -" + command + "- was sent via IPC.");
						readThread.join();
					}
					catch (const std::exception& e) {
						// Future error
						carousel::logging::CarouselLogger::instance().error("IpcPipeHandler: Command could not be sent -" + std::string(e.what()));
					}
				}

				return std::tuple<std::future_status, std::string>(status, response);
			}

			/// <summary>
			/// Clears all handled resources
			/// </summary>
			void clearResources()
			{
				if (_processHandle != nullptr)
				{
					try
					{
						// Kill process if still running on the background
						Poco::Process::kill(*_processHandle);
					}
					catch (const std::exception&)
					{
						// Todo log about failure
					}
				}
			}


		};
	}
}