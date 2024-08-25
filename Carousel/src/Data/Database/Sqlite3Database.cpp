
#include "../../../include/Data/Database/Sqlite3Database.h"
#include <sstream>

namespace carousel
{
	namespace data
	{
#pragma region Destructor
		Sqlite3Database::~Sqlite3Database() {
			if (_connectionOpen == true) {
				sqlite3_close(db);
			}
		}
#pragma endregion

#pragma region SQLite3 Implementation
		void Sqlite3Database::connect()
		{
			if (_connectionOpen)
			{
				carousel::logging::CarouselLogger::instance().warning("Connecting to database failed because there is currently an open connection.");
				return;
			}

			std::string databaseFilename = _configuration->databaseDirectory + "\\" + _configuration->databaseFileName;
			if (sqlite3_open(databaseFilename.c_str(), &db) == SQLITE_OK)
			{
				// Allow waiting before throwing "is busy", required for threading
				int timeoutSetup = sqlite3_busy_timeout(db, 1000);
				_connectionOpen = true;
			}
			else
			{
				sqlite3_close(db);
				carousel::logging::CarouselLogger::instance().warning("Connecting to database was not possible: " + std::string(sqlite3_errmsg(db)));
				throw carousel::exceptions::DatabaseNotConnectedException();
			}
		}

		void Sqlite3Database::disconnect()
		{
			if (!_connectionOpen)
			{
				carousel::logging::CarouselLogger::instance().warning("Closing connection failed because connection was already closed.");
				return;
			}

			if (sqlite3_close(db) == SQLITE_OK)
			{
				_connectionOpen = false;
			}
			else
			{
				carousel::logging::CarouselLogger::instance().warning("Closing connection failed because connection was already closed.");
			}
		}

		std::vector<std::string> Sqlite3Database::getTableNames()
		{
			if (!_connectionOpen)
			{
				carousel::logging::CarouselLogger::instance().warning("Connecting to database failed because there is currently no open connection.");
				throw carousel::exceptions::DatabaseNotConnectedException();
			}

			std::vector<std::string> response;
			std::string query = "SELECT name FROM sqlite_schema WHERE type='table'";
			sqlite3_stmt* stmt;

			int returnCode = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0);
			if (returnCode == SQLITE_OK) {

				carousel::logging::CarouselLogger::instance().Info("getTableNames was executed successfully.");
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					const unsigned char* text = sqlite3_column_text(stmt, 0);
					response.push_back(reinterpret_cast<const char*>(text));
				}
			}
			else
			{
				std::string errMessage = sqlite3_errmsg(db);
				carousel::logging::CarouselLogger::instance().warning(errMessage);
				throw carousel::exceptions::DatabaseQueryFailed(errMessage);
			}

			sqlite3_finalize(stmt);
			return response;
		}

		void Sqlite3Database::createTable(DatabaseTable* newTable)
		{
			int columnCount = newTable->size();

			// Check columns
			if (columnCount == 0)
			{
				std::string errMessage = "Malformed database table for table '" + newTable->getTableName() + "', contained columns is zero.";
				carousel::logging::CarouselLogger::instance().warning(errMessage);
				throw carousel::exceptions::DatabaseQueryFailed(errMessage);
			}

			// Query definition
			std::string query = "CREATE TABLE IF NOT EXISTS '" + newTable->getTableName() + "' ( ";
			for (size_t i = 0; i < columnCount; i++)
			{
				// Column data
				DatabaseColumn* column = newTable->operator[](i);
				DatabaseConstraintType constraintType = column->getConstraint();

				// Query text
				if (i > 0) query += ", ";
				switch (constraintType)
				{
				case carousel::data::COLUMN:
					query += column->getName() + " " + MapToSqlType(column->getType());
					break;
				case carousel::data::PRIMARY_KEY:
					query += column->getName() + " " + MapToSqlType(typeid(int).name()) + " PRIMARY KEY AUTOINCREMENT ";
					break;
				case carousel::data::UNIQUE:
					query += column->getName() + " " + MapToSqlType(column->getType()) + " NOT NULL UNIQUE ";
					break;
				default:
					throw carousel::exceptions::NotImplementedException("createTable: Constraint type is not defined.");
					break;
				}
			}

			// End query
			query += " )";
			carousel::logging::CarouselLogger::instance().Info(query);

			// Execute query
			int Response = sqlite3_exec(db, query.c_str(), NULL, NULL, &_errorMessage);
			if (Response != SQLITE_OK)
			{
				std::string errMessage = sqlite3_errmsg(db);
				carousel::logging::CarouselLogger::instance().warning(errMessage);
				throw carousel::exceptions::DatabaseQueryFailed(errMessage);
			}
			
			carousel::logging::CarouselLogger::instance().Info("Table " + newTable->getTableName() + " was created successfully.");
		}

		void Sqlite3Database::dropTable(DatabaseTable* tableName)
		{
			// Query definition
			std::string query = "DROP TABLE \'" + tableName->getTableName() + "\'";
			carousel::logging::CarouselLogger::instance().Info(query);

			// Execute query
			int response = sqlite3_exec(db, query.c_str(), NULL, NULL, &_errorMessage);
			if (response != SQLITE_OK)
			{
				carousel::logging::CarouselLogger::instance().warning(_errorMessage);
				throw carousel::exceptions::DatabaseQueryFailed(std::string(_errorMessage));
			}

			carousel::logging::CarouselLogger::instance().Info("Table " + tableName->getTableName() + " was dropped successfully.");
		}

		bool Sqlite3Database::isNewEntry(DatabaseTable* tableData, IDatabaseObject* object)
		{
			// Check primary key
			bool isPkNew = true;
			if (tableData->hasPrimaryKey())
			{
				// Get primary key and compare with the default ID value
				std::string& pkValue = tableData->getPrimaryKey()->getValue(object);
				isPkNew = std::to_string(carousel::data::DEFAULT_ID) == pkValue;
			}

			// Check if any has been set in the database
			std::vector<DatabaseColumn*> uniqueColumns = tableData->getColumnsByConstraint(DatabaseConstraintType::UNIQUE);
			bool areUniqueNew = getCountForUniqueConstraint(tableData, object) == 0;

			return isPkNew && areUniqueNew;
		}

		void Sqlite3Database::save(IDatabaseObject* object)
		{
			DatabaseTable table = object->get_table_structure();
			carousel::logging::CarouselLogger::instance().Info("Saving IDatabaseObject: " + table.getTableName());

			// check if this is a new constraint
			bool isNew = isNewEntry(&table, object);

			// Build query
			std::ostringstream queryStream;
			if (isNew)
			{
				queryStream << getInsertQuery(&table, object);
			}
			else
			{
				queryStream << getUpdateQuery(&table, object);
			}

			// Run query
			int result = sqlite3_exec(db, queryStream.str().c_str(), NULL, NULL, &_errorMessage);
			if (result != SQLITE_OK)
			{
				std::string errMessage = std::string(_errorMessage) + ", using the query: " + queryStream.str();
				carousel::logging::CarouselLogger::instance().Info(errMessage);
				throw carousel::exceptions::DatabaseQueryFailed(errMessage);
			}

			// Update primary key value
			if (isNew && table.hasPrimaryKey())
			{
				table.getPrimaryKey()->setValue(object, getLastPrimaryKey(&table));
			}
		}

		void Sqlite3Database::save(std::vector<IDatabaseObject*> objects)
		{
			std::vector<IDatabaseObject*> constraintPrimaryKey;
			std::vector<IDatabaseObject*> constraintUnique;

			for (auto& object : objects)
			{

			}
			// Get SQL queries
			// Sepparate from update to new
			// sepparate from constraint type
		}


#pragma endregion

#pragma region Helpers
		std::string Sqlite3Database::getInsertQuery(DatabaseTable* const tableData, IDatabaseObject* object)
		{
			// Get columns that are not auto-generated
			std::vector<DatabaseColumn*> columns = tableData->getColumnsByConstraint(DatabaseConstraintType::COLUMN | DatabaseConstraintType::UNIQUE);

			// Query 
			std::ostringstream queryStream;

			// Add column names
			queryStream << "INSERT INTO \'" << tableData->getTableName() << "\' ( " << columns[0];
			for (size_t i = 1; i < columns.size(); i++)
			{
				queryStream << ", " << columns[i]->getName();
			}
			queryStream << " ) VALUES ( ";

			// Add Column values
			queryStream << "\'" << columns[0]->getValue(object) << "\'";
			for (size_t i = 1; i < columns.size(); i++)
			{
				queryStream << ", \'" << columns[i]->getValue(object) << "\'";
			}
			queryStream << " );";

			return queryStream.str();
		}

		std::string Sqlite3Database::getUpdateQuery(DatabaseTable* const tableData, IDatabaseObject* object)
		{
			// Get columns that are not auto-generated
			std::vector<DatabaseColumn*> columns = tableData->getColumnsByConstraint(DatabaseConstraintType::COLUMN);

			// Query
			std::ostringstream queryStream;

			// Values to update
			queryStream << "UPDATE \'" << tableData->getTableName() << "\' SET ";
			bool firstColumn{ true };
			for (size_t i = 0; i < columns.size(); i++)
			{
				if (!firstColumn) queryStream << ", ";
				queryStream << columns[i]->getName() + " = \'" + columns[i]->getValue(object) + "\'";
				firstColumn = false;
			}

			// Constraints
			std::vector<DatabaseColumn*> constraintColumns = tableData->getColumnsByConstraint(DatabaseConstraintType::PRIMARY_KEY | DatabaseConstraintType::UNIQUE);

			queryStream << " WHERE ";
			if (constraintColumns.size() > 0)
			{
				bool firstConstraint{ true };
				for (const auto& column : constraintColumns)
				{
					if (!firstConstraint) queryStream << " AND ";
					queryStream << column->getName() << " = " << string_to_database_format(column->getType(), column->getValue(object));
					firstConstraint = false;
				}
			}
			else
			{
				throw carousel::exceptions::NotImplementedException("getUpdateQuery did not find any valid constraints in the table definition, missing implementation?.");
			}

			return queryStream.str();
		}

		std::string Sqlite3Database::getLastPrimaryKey(DatabaseTable* const tableData)
		{
			std::string Query = "SELECT " + tableData->getPrimaryKey()->getName() + " FROM \'" + tableData->getTableName() + "\' ORDER BY ID DESC LIMIT 1 ";
			std::string Response{ "-1" };
			sqlite3_stmt* stmt;

			int exec_result = sqlite3_prepare_v2(db, Query.c_str(), -1, &stmt, 0);
			if (exec_result == SQLITE_OK) {

				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					std::stringstream ss;
					std::string TName;
					ss << sqlite3_column_text(stmt, 0);
					ss >> TName;
					Response = TName;
				}
			}

			sqlite3_finalize(stmt);
			return Response;
		}

		int Sqlite3Database::getCountForUniqueConstraint(DatabaseTable* const tableData, IDatabaseObject* object)
		{
			// Unique columns
			std::vector<DatabaseColumn*> uniqueColumns = tableData->getColumnsByConstraint(DatabaseConstraintType::UNIQUE);
			if (uniqueColumns.size() == 0)
			{
				// No unique columns were found
				return 0;
			}

			// Build query
			std::ostringstream queryStream;
			queryStream << "SELECT COUNT(*) FROM \'" + tableData->getTableName() + "\' WHERE ";

			bool firstConstraint{ false };
			for (const auto& column : uniqueColumns)
			{
				if (!firstConstraint) queryStream << " OR ";
				queryStream << column->getName() << " = " << string_to_database_format(column->getType(), column->getValue(object));
				firstConstraint = true;
			}
			queryStream << ";";

			// Send query to database and return value 
			int Response{ -1 };
			sqlite3_stmt* stmt;
			int exec_result = sqlite3_prepare_v2(db, queryStream.str().c_str(), -1, &stmt, 0);
			if (exec_result == SQLITE_OK) {

				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					std::stringstream ss;
					std::string TName;
					ss << sqlite3_column_text(stmt, 0);
					ss >> TName;
					Response = std::stoi(TName);
				}
			}

			sqlite3_finalize(stmt);
			return Response;
		}

		std::string Sqlite3Database::string_to_database_format(const std::string& typeidName, const std::string& value)
		{
			if (typeidName == typeid(std::string).name())
				return "'" + value + "'";
			else if (typeidName == typeid(int).name()
				|| typeidName == typeid(double).name())
				return value;
			else
				throw carousel::exceptions::NotImplementedException("Managing data type '" + typeidName + "' is not implemented.");
		}

		std::string Sqlite3Database::MapToSqlType(const std::string& typeidName)
		{
			if (typeidName == typeid(std::string).name())
				return "VARCHAR";
			else if (typeidName == typeid(int).name())
				return "INTEGER";
			else if (typeidName == typeid(bool).name())
				return "BOOLEAN";
			else if (typeidName == typeid(float).name())
				return "FLOAT";
			else if (typeidName == typeid(double).name())
				return "DOUBLE";
			else
				throw carousel::exceptions::NotImplementedException("Mapping type '" + typeidName + "' to SQL is not yet implemented.");
		}
#pragma endregion
	}
}

