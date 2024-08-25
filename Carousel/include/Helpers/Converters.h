#pragma once

#include <charconv>
#include <string>
#include <system_error>
#include <type_traits>

namespace carousel
{
	namespace helpers
	{
		namespace converters
		{
			/// <summary>
			/// Numeric precision to be used.
			/// </summary>
			static inline const int Precision = 32;

			/// <summary>
			/// Converts numeric type to string with decimal.
			/// </summary>
			/// <param name="T">Numeric type</param>
			/// <param name="value"></param>
			/// <returns>Returns the string representation of the numeric type</returns>
			template<typename T>
			std::string convertToString(T value)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					return value;
				}
				else if (constexpr (std::is_same_v<T, double>)
					|| constexpr (std::is_same_v<T, long double>))
				{
					// using 32 precision
					char arr[Precision] = {};

					// Convert value to string
					auto [ptr, ec] = std::to_chars(arr, arr + sizeof(arr), value);

					// Throw if conversion was not carried out
					if (ec != std::errc()) {
						throw ec;
					}

					// Return string
					return std::string(arr, ptr);
				}
				else
				{
					return std::to_string(value);
				}

			}

			/// <summary>
			/// Converts string value to corresponding data type value
			/// </summary>
			template<typename T>
			static T convertFromString(const std::string& str) {
				if constexpr (std::is_same_v<T, std::string>)
				{
					return str;
				}
				else if constexpr (std::is_integral_v<T>)
				{
					return std::stoi(str);
				}
				else if constexpr (std::is_floating_point_v<T>)
				{
					return std::stof(str);
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					return std::stod(str);;
				}
				else if constexpr (std::is_same_v<T, long double>)
				{
					return std::stold(str);;
				}
				else
				{
					static_assert("Unsupported type");
				}
			}
		}
	}
}
