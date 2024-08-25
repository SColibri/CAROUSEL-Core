#pragma once
#include <string>

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Struct for database configurations
		/// </summary>
		struct DatabaseConfiguration
		{
			/// <summary>
			/// Database type enum
			/// </summary>
			enum class TYPE
			{
				SQLITE3
			};

			/// <summary>
			/// Selected database type
			/// </summary>
			TYPE selectedDatabaseType{ TYPE::SQLITE3 };

			/// <summary>
			/// Database schema name
			/// </summary>
			std::string schemaName{ "Carousel" };

			/// <summary>
			/// Directory to database
			/// </summary>
			std::string databaseDirectory{""};

			/// <summary>
			/// name of the database file
			/// </summary>
			std::string databaseFileName{"carousel.db"};
		};
	}
}