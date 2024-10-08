#pragma once
#include "CallbackDefinitions.h"

namespace carousel
{
	namespace callbacks
	{
		/// <summary>
		/// Error callback, gets message from core implementation
		/// </summary>
		class ErrorCallback
		{
		public:
			/// <summary>
			/// Callback
			/// </summary>
			static inline ErrorCallbackF CallFunction{ nullptr };

			/// <summary>
			/// Trigger
			/// </summary>
			/// <param name="callbackData"></param>
			static inline void TriggerCallback(char* callbackData)
			{
				// Check if callback has been set
				if (CallFunction != nullptr)
				{
					(*CallFunction)(callbackData);
				}
			}

		};
	}
}