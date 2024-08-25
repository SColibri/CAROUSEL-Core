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

namespace carousel
{
	namespace ipcTools
	{
		constexpr int BUFFER_SIZE = 1024;
		constexpr int TIMEOUT = 15;

		/// <summary>
		/// 
		/// </summary>
		class IpcPipeHandler
		{
		private:

			/// <summary>
			/// Path to executable to which communication is established
			/// </summary>
			const std::string _pathToExe;

			/// <summary>
			/// Buffer container that will the communication data
			/// </summary>
			char _buffer[BUFFER_SIZE];

			/// <summary>
			/// Flag that determines the end of the message
			/// </summary>
			const std::string _endFlag;

			/// <summary>
			/// Timeout for receiving data, in seconds
			/// </summary>
			int _timeout{ TIMEOUT };

			/// <summary>
			/// Returns if communication channel is ready 
			/// </summary>
			bool isReady{ false };

			/// <summary>
			/// Pointer to console application
			/// </summary>
			Poco::ProcessHandle* _processHandle{ nullptr };
			Poco::PipeOutputStream* _outStream{ nullptr };
			Poco::PipeInputStream* _inStream{ nullptr };

			Poco::Pipe _pipeIn;
			Poco::Pipe _pipeOut;


		public:
			/// <summary>
			/// Constructor
			/// </summary>
			IpcPipeHandler(std::string pathToExe, std::string endFlag, int timeout = TIMEOUT) :
				_pathToExe(std::move(pathToExe)),
				_endFlag(std::move(endFlag)),
				_timeout(timeout)
			{
				// Launch process and get handles
				Poco::ProcessHandle ph = Poco::Process::launch(
					pathToExe,						   // Replace with your external application path
					{},
					{},                                // No environment variables
					&_pipeIn, &_pipeOut,                 // Connect pipes to process stdin and stdout
					nullptr                            // No working directory
				);
				_processHandle = new Poco::ProcessHandle(ph);

				// Create streams
				_outStream = new Poco::PipeOutputStream(_pipeIn);
				_inStream = new Poco::PipeInputStream(_pipeOut);

				isReady = true;
			}

			/// <summary>
			/// Destructor
			/// </summary>
			~IpcPipeHandler()
			{
				clearResources();
			}

			/// <summary>
			/// Send command
			/// </summary>
			/// <param name="command"></param>
			/// <returns></returns>
			std::string sendCommand(const std::string& command)
			{
				if (!_outStream->good()) throw std::exception("Communication channel is not ready");
				if (command.length() == 0) throw std::exception("Empty command");

				// Setup the task
				std::promise<std::string> promise;
				std::future<std::string> future = promise.get_future();

				// Start send command
				std::thread readThread(send, std::ref(_outStream), std::ref(_inStream), std::move(promise), command);

				// Set a timeout period (e.g., 5 seconds)
				auto status = future.wait_for(std::chrono::seconds(_timeout));
				std::string response;

				if (status == std::future_status::ready) {
					try {
						response = future.get();
						std::cout << "Response: " << response << std::endl;
					}
					catch (const std::exception& e) {
						std::cerr << "Error during send: " << e.what() << std::endl;
					}
				}
				else {
					std::cerr << "Timeout occurred while reading from stream." << std::endl;
					Poco::Process::kill(*_processHandle);
				}

				readThread.join();  // Ensure the thread has finished
				return response;



			}

			std::string readCommand()
			{
				if (!isReady) throw std::exception("Communication channel is not ready");

				std::promise<std::string> promise;
				std::future<std::string> future = promise.get_future();

				// Start the thread with the bound function
				std::thread readThread(read, std::ref(_inStream), std::move(promise));

				// Set a timeout period (e.g., 5 seconds)
				auto status = future.wait_for(std::chrono::seconds(_timeout));
				std::string response;
				if (status == std::future_status::ready) {
					try {
						response = future.get();
						std::cout << "Response: " << response << std::endl;
					}
					catch (const std::exception& e) {
						std::cerr << "Error during read: " << e.what() << std::endl;
					}
				}
				else {
					std::cerr << "Timeout occurred while reading from stream." << std::endl;
					sendCommand("exit");
					//Poco::Process::kill(*_processHandle);
				}

				response = future.get();
				readThread.join();  // Ensure the thread has finished


				return response;
			}


		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="in"></param>
			/// <param name="promise"></param>
			static void read(Poco::PipeInputStream* in, std::promise<std::string>&& promise) {
				try {
					std::stringstream result;
					int retryCount = 0;
					while (in->good())
					{
						char buffer[512];
						in->getline(buffer, sizeof(buffer));
						//in->read(buffer, sizeof(buffer));
						result << buffer;
						
						auto positionFlag = result.str().rfind("MC: ");
						if (positionFlag != std::string::npos && positionFlag + 4 == result.str().length())
						{
							break;
						}
						else
						{
							//retryCount = tasty4 < 1 ? retryCount + 1 : 0;
							//std::this_thread::sleep_for(std::chrono::microseconds(1000));
						}

					}
					promise.set_value(result.str());
				}
				catch (...) {
					promise.set_exception(std::current_exception());
				}
			}


			static void send(Poco::PipeOutputStream* outStream, Poco::PipeInputStream* in, std::promise<std::string>&& promise, std::string command)
			{
				(*outStream) << command.c_str() << std::endl;

				outStream->flush();

				promise.set_value("Command sent");
				//read(in, std::move(promise));
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

				// Remove all resources
				delete _inStream;
				delete _outStream;
				delete _processHandle;

				_inStream = nullptr;
				_outStream = nullptr;
				_processHandle = nullptr;

				// Set state back to false
				isReady = false;
			}


		};
	}
}