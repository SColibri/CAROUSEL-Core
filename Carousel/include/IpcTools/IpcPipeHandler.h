#pragma once
#include <string>
#include <sstream>
#include <future>
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
		constexpr int TIMEOUT = 30;

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
			int _timeout;

			/// <summary>
			/// Allocated time for the process to initialize before starting communication.
			/// </summary>
			int _initProcTime{ 500 };

			/// <summary>
			/// If true, means the process is ready for communication
			/// </summary>
			bool _processReady{ false };

			/// <summary>
			/// Process Id
			/// </summary>
			int _processId{ 0 };

			/// <summary>
			/// Pointer to process handle
			/// </summary>
			SystemHandle _processHandle;

			/// <summary>
			/// In pipe read handle
			/// </summary>
			SystemHandle _pipeReadIn;

			/// <summary>
			/// In pipe write handle
			/// </summary>
			SystemHandle _pipeWriteIn;

			/// <summary>
			/// Out pipe read handle
			/// </summary>
			SystemHandle _pipeReadOut;

			/// <summary>
			/// Out pipe write handle
			/// </summary>
			SystemHandle _pipeWriteOut;

		public: // IpcPipeHandler

			/// <summary>
			/// Constructor
			/// </summary>
			IpcPipeHandler(std::string pathToExe, std::string endFlag, std::string parameters = "", int timeout = TIMEOUT);

			/// <summary>
			/// Destructor
			/// </summary>
			~IpcPipeHandler();

		public: // IPC Interface

			/// <summary>
			/// Returns true if communication channel is ready
			/// </summary>
			bool isReady();

			/// <summary>
			/// Send command to process
			/// </summary>
			void send(const std::string& command);

			/// <summary>
			/// Reads incoming data
			/// </summary>
			std::string read();

		private: // Process initialization

			/// <summary>
			/// Initializes the process
			/// </summary>
			void startProcess();

			/// <summary>
			/// Terminates process
			/// </summary>
			void closeProcess();

			/// <summary>
			/// Creates anonymous pipes to be attached to process stdout/stdin
			/// </summary>
			void createPipes();

			/// <summary>
			/// Waits until the process has successfully initialized or the timeout is reached
			/// </summary>
			void waitUntilReady();

		private: // Helpers

			/// <summary>
			/// Reads from pipe
			/// </summary>
			std::string readFromPipe(SystemHandle pipe);

			/// <summary>
			/// Writes to pipe
			/// </summary>
			void writeToPipe(const std::string& command, SystemHandle pipeInWrite);
		};
	}
}