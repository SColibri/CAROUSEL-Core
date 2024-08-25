#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <map>
#include <stdexcept>
#include "DatabaseColumn.h"
#include "../../Exceptions/DatabaseConstraintException.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Structure for database table
		/// </summary>
		class DatabaseTable
		{
		private:
			/// <summary>
			/// Table name
			/// </summary>
			std::string _tableName;

			/// <summary>
			/// Columns
			/// </summary>
			std::vector<std::shared_ptr<DatabaseColumn>> _columns;

			/// <summary>
			/// Column map (name, DatabaseColumn)
			/// </summary>
			std::map<std::string, std::shared_ptr<DatabaseColumn>> _columnMap;

			/// <summary>
			/// Column map by type (type, DatabaseColumn).
			/// Allows finding column corresponding to primary key, unique constraints and other
			/// </summary>
			std::map<DatabaseConstraintType, std::vector<std::shared_ptr<DatabaseColumn>>> _columnTypeMap;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			DatabaseTable(const std::string& tableName)
			{
				this->_tableName = tableName;
			}

			/// <summary>
			/// Database column by index
			/// </summary>
			DatabaseColumn* operator[](int index)
			{
				return _columns[index].get();
			}

			/// <summary>
			/// Find Database column by name
			/// </summary>
			DatabaseColumn* operator[](const std::string& name)
			{
				return _columnMap.at(name).get();
			}

			/// <summary>
			/// Returns table name
			/// </summary>
			const std::string& getTableName()
			{
				return _tableName;
			}

			/// <summary>
			/// Returns the column count
			/// </summary>
			int size()
			{
				return _columns.size();
			}

			/// <summary>
			/// Adds a new column to this managed data table.
			/// </summary>
			/// <param name="T">Model object</param>
			/// <param name="R">Output/Input type</param>
			/// <param name="name">Column name</param>
			/// <param name="type">Column type</param>
			/// <param name="getter">Pointer to member for getter method</param>
			/// <param name="setter">Pointer to member for setter method</param>
			/// <param name="constraint">Column constraint type</param>
			template<typename T, typename R>
			void addColumn(const std::string& name, const std::string& type, const R& (T::* getter)(), void (T::* setter)(const R&), DatabaseConstraintType constraint = DatabaseConstraintType::COLUMN)
			{
				// Create and add column
				std::shared_ptr<DatabaseColumn> column = std::make_shared<DatabaseColumn>(name, type, constraint);
				column->setGetter(getter);
				column->setSetter(setter);

				// Add to vector
				_columns.push_back(column);

				// Add to map
				_columnMap.emplace(name, column);

				// Add to type map
				_columnTypeMap[column->getConstraint()].push_back(column);
			}

			/// <summary>
			/// Returns primary key column if defined
			/// </summary>
			DatabaseColumn* getPrimaryKey()
			{
				auto columnEntry = _columnTypeMap.find(DatabaseConstraintType::PRIMARY_KEY);
				if (columnEntry != _columnTypeMap.end())
				{
					return columnEntry->second.front().get();
				}
				else
				{
					throw carousel::exceptions::DatabaseConstraintException(_tableName + " has no primary key defined.");
				}
			}

			/// <summary>
			/// Returns true if the table contains a primary key
			/// </summary>
			bool hasPrimaryKey() 
			{
				auto columnEntry = _columnTypeMap.find(DatabaseConstraintType::PRIMARY_KEY);
				return columnEntry != _columnTypeMap.end();
			}

			/// <summary>
			/// Returns list of database columns that have the DatabaseConstraintType as given constraint 
			/// </summary>
			std::vector<DatabaseColumn*> getColumnsByConstraint(DatabaseConstraintType constraint)
			{
				// output
				std::vector<DatabaseColumn*> result;

				// Loop over available enum values and append to result
				for (int constraintEnum = DatabaseConstraintType::FIRST;
					constraintEnum <= DatabaseConstraintType::LAST;
					constraintEnum++)
				{
					// check if flag is set
					DatabaseConstraintType constraintCheck = static_cast<DatabaseConstraintType>(1 << constraintEnum);
					if ((constraint & constraintCheck) == constraintCheck)
					{
						// append if constraint has been set
						auto columnEntry = _columnTypeMap.find(constraintCheck);
						if (columnEntry != _columnTypeMap.end())
						{
							for (const auto& column : columnEntry->second)
							{
								result.push_back(column.get());
							}
						}
					}
				}

				return result;
			}
		};
	}
}

