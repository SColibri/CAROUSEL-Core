#pragma once

#include "IDatabase.h"
#include "DatabaseConstants.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// 
		/// </summary>
		class DatabaseAdapterManager
		{
		private:
			/// <summary>
			/// Reference to database
			/// </summary>
			IDatabase& _database;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			DatabaseAdapterManager(IDatabase& database) :_database(database) { }

		public:
			/// <summary>
			/// Saves database object into the database
			/// </summary>
			void save(IDatabaseObject& databaseObject) 
			{
				DatabaseTable testy = databaseObject.get_table_structure();
				
			}

			/// <summary>
			/// Saves multiple database objects into the database
			/// </summary>
			void save(const std::vector<IDatabaseObject*>& databaseObjects)
			{
			
			}

			/// <summary>
			/// Loads database object based on DatabaseTable properties.
			/// Note: Either primary key or unique properties have to be specified before load.
			/// </summary>
			// void load(IDatabaseObject* const databaseObject) = 0;

		private:
		};
	}
}