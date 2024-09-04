#include "../../../include/IpcTools/IpcPipeHandler.h"
#include "../../../include/Exceptions/IpcCommunicationException.h"
#include "../../../include/Logging/CarouselLogger.h"
#include <Windows.h>
#include <thread>
#include <future>

namespace carousel
{
	namespace ipcTools
	{
#pragma region Constructor&Destructor
		IpcPipeHandler::IpcPipeHandler(std::string pathToExe, std::string endFlag, std::string parameters, int timeout) :
			_pathToExe(std::move(pathToExe)),
			_parameters(std::move(parameters)),
			_endFlag(std::move(endFlag)),
			_timeout(timeout)
		{
			// Initialize default handles
			_pipeWriteIn = NULL;
			_pipeReadOut = NULL;
			_pipeWriteOut = NULL;
			_pipeReadIn = NULL;
			_processHandle = NULL;

			// Shoot initialization thread
			std::thread initThread(&IpcPipeHandler::startProcess, this);
			initThread.detach();
		}

		IpcPipeHandler::~IpcPipeHandler()
		{
			closeProcess();
		}
#pragma endregion

#pragma region Methods
		bool IpcPipeHandler::isReady() const
		{
			DWORD exitCode = STATUS_CONTROL_C_EXIT;

			if (!GetExitCodeProcess(_processHandle, &exitCode))
			{
				carousel::logging::CarouselLogger::instance().warning("IpcPipeHandler::isReady returned process exit code '" + std::to_string(exitCode) + "'");
			}

			return _processReady && (exitCode == STILL_ACTIVE);
		}

		void IpcPipeHandler::send(const std::string& command)
		{
			waitUntilReady();
			writeToPipe(command, _pipeWriteIn);
		}

		std::string IpcPipeHandler::read()
		{
			// Make sure proccess is ready
			waitUntilReady();
			std::string result, pipeOutput;

			if (isReady())
			{
				// Read data from pipe and make sure process has ended transmitting 
				do
				{
					pipeOutput = readFromPipe(_pipeReadOut);
					result.append(pipeOutput);

					// Wait for process
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				} while (pipeOutput.length() > 0 && result.rfind(_endFlag) == std::string::npos);
			}
			else
			{
				// Connection is broken
				throw carousel::exceptions::IpcCommunicationException("IpcPipeHandler::read - Communication to process is broken.");
			}

			return result;
		}
#pragma endregion

#pragma region Private_helpers
		void IpcPipeHandler::startProcess()
		{
			createPipes();

			STARTUPINFOA startup_info;
			PROCESS_INFORMATION process_info;

			// Subprocess startup setup
			memset(&startup_info, 0, sizeof(startup_info));
			memset(&process_info, 0, sizeof(process_info));
			startup_info.cb = sizeof(startup_info);
			startup_info.hStdInput = _pipeReadIn;
			startup_info.hStdOutput = _pipeWriteOut;
			startup_info.hStdError = _pipeWriteOut;
			startup_info.dwFlags |= STARTF_USESTDHANDLES;

			// prepare string
			char* zt = new char[_pathToExe.length() + 1];
			std::strcpy(zt, _pathToExe.c_str());

			// Try create new subprocess
			bool bSuccess = CreateProcessA(NULL,
				zt,					 // command line 
				NULL,				 // process security attributes 
				NULL,				 // primary thread security attributes 
				TRUE,				 // handles are inherited 
				CREATE_NO_WINDOW,    // creation flags 
				NULL,				 // use parent's environment 
				NULL,				 // use parent's current directory 
				&startup_info,		 // STARTUPINFO pointer 
				&process_info);		 // receives PROCESS_INFORMATION 

			// If initializing the process was successful then
			// then read the initialization output from the process.
			if (bSuccess)
			{
				_processHandle = process_info.hProcess;
				_processId = process_info.dwProcessId;

				// Wait for process to finish initialization
				std::string pipeData{ "" };
				while (pipeData.length() == 0)
				{
					// wait for process
					std::this_thread::sleep_for(std::chrono::milliseconds(_initProcTime));
					pipeData = readFromPipe(_pipeReadOut);;
				}

				_processReady = true;
			}
			else 
			{
				// Process did not initialize, clear handles
				_pipeWriteIn = NULL;
				_pipeReadOut = NULL;
				_pipeWriteOut = NULL;
				_pipeReadIn = NULL;
				_processHandle = NULL;
			}

			// cleanup
			delete[] zt;
		}

		void IpcPipeHandler::closeProcess()
		{
			// Close all other handles
			CloseHandle(_pipeWriteIn);
			CloseHandle(_pipeReadOut);
			CloseHandle(_pipeWriteOut);
			CloseHandle(_pipeReadIn);

			// Force closing process
			UINT uExitCode = -1;
			TerminateProcess(_processHandle, uExitCode);

			// set null
			_pipeWriteIn = NULL;
			_pipeReadOut = NULL;
			_pipeWriteOut = NULL;
			_pipeReadIn = NULL;
			_processHandle = NULL;
		}

		void IpcPipeHandler::createPipes()
		{
			// Pipe attributes
			SECURITY_ATTRIBUTES security_attrib;
			security_attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
			security_attrib.bInheritHandle = TRUE;
			security_attrib.lpSecurityDescriptor = NULL;

			CreatePipe(&_pipeReadIn, &_pipeWriteIn, &security_attrib, 0);
			SetHandleInformation(_pipeWriteIn, HANDLE_FLAG_INHERIT, 0);

			CreatePipe(&_pipeReadOut, &_pipeWriteOut, &security_attrib, 0);
			SetHandleInformation(_pipeReadOut, HANDLE_FLAG_INHERIT, 0);
		}
#pragma endregion

#pragma region Helpers
		std::string IpcPipeHandler::readFromPipe(SystemHandle pipeReadOut)
		{
			// Result
			std::stringstream pipeData;

			char buffer[BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));
			DWORD bytesRead, bytesAvailable;

			// While data is contained in the pipe, get data from the pipe.
			// Note: PeekNamedPipe checks if data is available before calling the
			//		 ReadFile method. This avoids a possible deadlock.
			while (PeekNamedPipe(pipeReadOut, NULL, 0, NULL, &bytesAvailable, NULL) > 0)
			{
				if (bytesAvailable == 0) break;
				bool readSuccess = ReadFile(pipeReadOut, buffer, sizeof(BUFFER_SIZE), &bytesRead, NULL);

				if (!readSuccess || bytesRead == 0) break;
				pipeData.write(buffer, bytesRead);
			}

			return pipeData.str();
		}

		void IpcPipeHandler::writeToPipe(const std::string& command, SystemHandle pipeWriteIn)
		{
			DWORD dwWritten;
			BOOL bSuccess = FALSE;
			size_t dataLen = command.length();
			size_t totalWritten = 0;

			while (totalWritten < dataLen)
			{
				// Calculate how much data to write in this iteration
				size_t toWrite = min(BUFFER_SIZE, dataLen - totalWritten);

				// Write the data to the pipe
				bSuccess = WriteFile(
					pipeWriteIn,					  // Handle to the pipe
					command.c_str() + totalWritten,   // Pointer to the current position in the string
					static_cast<DWORD>(toWrite),      // Number of bytes to write
					&dwWritten,                       // Number of bytes actually written
					NULL                              // Not overlapped
				);

				if (!bSuccess) break;

				totalWritten += dwWritten;
			}
		}
#pragma endregion
	}
}