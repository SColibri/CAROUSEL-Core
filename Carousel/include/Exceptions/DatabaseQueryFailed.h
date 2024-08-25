#pragma once
#include <exception>
#include <string>

namespace carousel
{
	namespace exceptions
	{
		/// <summary>
		/// Exception thrown when executing a query was not successful
		/// </summary>
		class DatabaseQueryFailed : public std::exception
		{
		private:
			std::string _message{ "" };

		public:
			DatabaseQueryFailed(const std::string& message) : _message(message) {}

			virtual const char* what() const noexcept {
				return _message.c_str();
			}
		};
	}
}