#pragma once
#include <exception>

namespace carousel
{
	namespace exceptions
	{
		/// <summary>
		/// Exception thrown when an operation is tried on a closed database connection
		/// </summary>
		class DatabaseNotConnectedException : public std::exception
		{
		public:
			virtual const char* what() const noexcept {
				return "Database connection is not open.";
			}
		};
	}
}