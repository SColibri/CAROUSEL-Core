#pragma once

#include <charconv>
#include <string>
#include <system_error>
#include <type_traits>

namespace carousel
{
	namespace helpers
	{
		/// <summary>
		/// Converts numeric type to string with decimal.
		/// </summary>
		/// <param name="T">Numeric type</param>
		/// <param name="value"></param>
		/// <returns>Returns the string representation of the numeric type</returns>
		template<typename T>
		std::string convertToString(T value) {

			// Only numeric value types shall be converted
			static_assert(std::is_arithmetic_v<T>, "Template parameter must be a numeric type.");

			// using 32
			char arr[32] = {};

			// Convert value to string
			auto [ptr, ec] = std::to_chars(arr, arr + sizeof(arr), value);

			// Throw if conversion was not carried out
			if (ec != std::errc()) {
				throw ec;
			}

			// Return string
			return std::string(arr, ptr);
		}
	}
}
