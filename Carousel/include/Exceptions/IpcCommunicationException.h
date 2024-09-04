#pragma once

#include "../Logging/CarouselLogger.h"

namespace carousel
{
	namespace exceptions
	{
		/// <summary>
		/// Exception thrown if the database table constraint is wrong 
		/// </summary>
		class IpcCommunicationException : public std::exception
		{
		private:
			std::string _message{ "" };

		public:
			IpcCommunicationException(const std::string& message) : _message(message)
			{
				carousel::logging::CarouselLogger::instance().error(_message);
			}

			virtual const char* what() const noexcept {
				return _message.c_str();
			}
		};
	}
}