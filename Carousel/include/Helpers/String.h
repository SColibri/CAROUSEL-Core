#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace carousel
{
	namespace helpers
	{
		namespace string
		{
			// Trim whitespace from the beginning
			std::string ltrim(const std::string& s) {
				std::string::const_iterator it = std::find_if_not(s.begin(), s.end(), ::isspace);
				return std::string(it, s.end());
			}

			// Trim whitespace from the end
			std::string rtrim(const std::string& s) {
				std::string::const_reverse_iterator it = std::find_if_not(s.rbegin(), s.rend(), ::isspace);
				return std::string(s.begin(), it.base());
			}

			// Trim whitespace from both ends
			std::string trim(const std::string& s) {
				return ltrim(rtrim(s));
			}
		}
	}
}