#pragma once
#include "../../../include/IpcTools/IpcPipeHandler.h"
#include <thread>
#include <future>

namespace carousel
{
	namespace ipcTools
	{
#pragma region private_helpers
		void IpcPipeHandler::waitUntilReady()
		{	
			if (_processReady) return;

			// Setup the task
			std::promise<bool> promise;
			std::future<bool> future = promise.get_future();
			bool keepWaiting{ true };

			std::thread waitThread([this, &promise, &keepWaiting]()
				{
					// Wait until process is ready or timeout
					while (!_processReady && keepWaiting)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
					promise.set_value(true);
				});

			// Wait for process to finish initialization
			std::future_status status = future.wait_for(std::chrono::seconds(_timeout));
			if (status != std::future_status::ready) 
			{
				throw std::runtime_error("Timeout: Process not ready within the expected time.");
			}

			// Cleanup
			keepWaiting = false;
			waitThread.join();
		}
#pragma endregion
	}
}