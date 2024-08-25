#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "DatabaseConfiguration.h"
#include "DatabaseTable.h"
#include "IDatabaseObject.h"
#include "../../Exceptions/NotImplementedException.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Database communication interface
		/// </summary>
		class IDatabase
		{
		protected:
			/// <summary>
			/// Database configuration
			/// </summary>
			DatabaseConfiguration* _configuration{ nullptr };

		public:
			/// <summary>
			/// Delimiter for csv data
			/// </summary>
			static const std::string Delimiter;

		protected:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="configuration"></param>
			IDatabase(DatabaseConfiguration* configuration)
			{
				_configuration = configuration;
			}

			/// <summary>
			/// Destructor
			/// </summary>
			~IDatabase() {}

		public:
			/// <summary>
			/// Opens a connection to the database specified by filename.
			/// </summary>
			virtual void connect() = 0;

			/// <summary>
			/// Closes the connection to the database
			/// </summary>
			virtual void disconnect() = 0;

			/// <summary>
			/// Returns the list of tables that the database has
			/// </summary>
			virtual std::vector<std::string> getTableNames() = 0;

			/// <summary>
			/// Creates a new table
			/// </summary>
			virtual void createTable(DatabaseTable* tableData) = 0;

			/// <summary>
			/// Drops an existing table
			/// </summary>
			virtual void dropTable(DatabaseTable* tableData) = 0;

			/// <summary>
			/// Saves entry
			/// </summary>
			/// <param name="object"></param>
			virtual void save(IDatabaseObject* object) = 0;

			/// <summary>
			/// Saves entry
			/// </summary>
			/// <param name="object"></param>
			virtual void save(std::vector<IDatabaseObject*> object) = 0;
		};
	}
}