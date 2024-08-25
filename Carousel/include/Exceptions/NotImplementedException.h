#pragma once

#include "../Logging/CarouselLogger.h"

namespace carousel
{
	namespace exceptions
	{
		/// <summary>
		/// Exception thrown if a method is not yet implemented
		/// </summary>
		class NotImplementedException : public std::exception
		{
		private:
			std::string _message{ "" };

		public:
			NotImplementedException(const std::string& message) : _message(message) 
			{
				carousel::logging::CarouselLogger::instance().error(_message);
			}

			virtual const char* what() const noexcept {
				return _message.c_str();
			}
		};
	}
}