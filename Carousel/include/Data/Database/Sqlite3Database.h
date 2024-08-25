#pragma once

#include <vector>
#include <sqlite3.h>
#include "IDatabase.h"
#include "DatabaseTable.h"
#include "DatabaseConfiguration.h"
#include "DatabaseConstants.h"
#include "../../Callbacks/ErrorCallback.h"
#include "../../Logging/CarouselLogger.h"
#include "../../Exceptions/DatabaseNotConnectedException.h"
#include "../../Exceptions/DatabaseQueryFailed.h"
#include "../../Exceptions/NotImplementedException.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Implementation of IDatabase using SQLite3
		/// </summary>
		class Sqlite3Database : public IDatabase
		{
		private:
			/// <summary>
			/// SQLite3 object
			/// </summary>
			sqlite3* db{ nullptr };


			char* _errorMessage{};
			bool _connectionOpen{ false };
			std::vector<std::string> _tableNames;
			std::string _databaseFile{};

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			Sqlite3Database(DatabaseConfiguration* configuration) :IDatabase(configuration) { }

			/// <summary>
			/// Destructor
			/// </summary>
			~Sqlite3Database();

		public: // IDatabase implementation
			virtual void connect() override;
			virtual void disconnect() override;
			virtual std::vector<std::string> getTableNames() override;
			virtual void createTable(DatabaseTable* tableData) override;
			virtual void dropTable(DatabaseTable* tableData) override;
			virtual void save(IDatabaseObject* object) override;
			virtual void save(std::vector<IDatabaseObject*> object) override;

		private:
			/// <summary>
			/// Returns query for inserting a new entry
			/// </summary>
			std::string getInsertQuery(DatabaseTable* const tableData, IDatabaseObject* object);

			/// <summary>
			/// Returns query for updating an entry
			/// </summary>
			std::string getUpdateQuery(DatabaseTable* const tableData, IDatabaseObject* object);

			/// <summary>
			/// Returns the last primary key value
			/// </summary>
			std::string getLastPrimaryKey(DatabaseTable* const tableData);

			/// <summary>
			/// Returns true if current entry is new and not contained in the database
			/// </summary>
			/// <param name="table"></param>
			/// <param name="object"></param>
			/// <returns></returns>
			bool isNewEntry(DatabaseTable* tableData, IDatabaseObject* object);

			/// <summary>
			/// Returns the count for a specific unique constraint
			/// </summary>
			int getCountForUniqueConstraint(DatabaseTable* const tableData, IDatabaseObject* object);

			/// <summary>
			/// Puts string into database format depending on the given type
			/// </summary>
			static std::string string_to_database_format(const std::string& typeidName, const std::string& value);

			/// <summary>
			/// Maps typeid names to SQLite types
			/// </summary>
			static std::string MapToSqlType(const std::string& typeidName);
		};
	}
}