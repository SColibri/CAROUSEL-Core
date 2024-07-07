#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Structure for database table
		/// </summary>
		struct DatabaseTable
		{
			/// <summary>
			/// Table name
			/// </summary>
			std::string tableName;

			/// <summary>
			/// List of column names
			/// </summary>
			std::vector<std::string> columnNames;

			/// <summary>
			/// List of data types corresponding to each column
			/// </summary>
			std::vector<std::string> columnDataTypes;

			/// <summary>
			/// Adds new column only if not contained
			/// </summary>
			/// <param name="columnName"Column name></param>
			/// <param name="columnDataType">Data type for column</param>
			void add_new(const std::string& columnName, const std::string& columnDataType)
			{
				std::vector<std::string>::iterator it = std::find(columnNames.begin(), columnNames.end(), columnName);

				if (it == columnNames.end())
				{
					columnNames.push_back(columnName);
					columnDataTypes.push_back(columnDataType);
				}
			}
		};
	}
}

