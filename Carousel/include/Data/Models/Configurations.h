#pragma once

#include <filesystem>
#include <sstream>
#include <iostream>
#include <string>
#include "../../Core/Interfaces/ISerializableObject.h"
#include "../SharedTypes/CarouselConfigurations.h"
#include "../../Exceptions/NotImplementedException.h"
#include "../../Helpers/String.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Singleton class
		/// Contains all modifiable configurations for carousel
		/// </summary>
		class Configurations : public carousel::core::ISerializableObject
		{
		private:
			/// <summary>
			/// Base model data
			/// </summary>
			carousel::data::CarouselConfiguration _model;

			/// <summary>
			/// Filename for carousel configuration data
			/// </summary>
			const std::string _filename = "carousel.config";

			/// <summary>
			/// Constructor
			/// </summary>
			Configurations()
				: _model(carousel::data::DatabaseConfiguration(),
					carousel::data::IpcConfiguration(),
					carousel::data::LoggingConfiguration())
			{
				// Try load file
				bool fileLoaded = load(_filename); //loadConfiguration(_filename);

				if (!fileLoaded)
				{
					save();
				}
			}

		public:

			/// <summary>
			/// Copy constructor (removed)
			/// </summary>
			Configurations(const Configurations&) = delete;

			/// <summary>
			/// Assignment operator (removed)
			/// </summary>
			void operator=(const Configurations&) = delete;

			/// <summary>
			/// Gets the configurations object
			/// </summary>
			/// <tag>#pythonSingleton</tag>
			static Configurations& instance()
			{
				static Configurations _config;
				return _config;
			}

		public:

			/// <summary>
			/// Gets the database directive
			/// </summary>
			/// <returns></returns>
			const carousel::data::DatabaseType& getSelectedDatabase()
			{
				return _model.DatabaseConfiguration().selectedDatabase();
			}

			/// <summary>
			/// Set the database directive
			/// </summary>
			void setSelectedDatabase(const carousel::data::DatabaseType::value& databaseSelection)
			{
				_model.DatabaseConfiguration().selectedDatabase(databaseSelection);
			}

			/// <summary>
			/// Gets the database directive
			/// </summary>
			/// <returns></returns>
			std::string getSelectedDatabaseString()
			{
				return DatabaseTypeToString(_model.DatabaseConfiguration().selectedDatabase());
			}

			/// <summary>
			/// Set the database directive
			/// </summary>
			/// <tag>#pythonProperty</tag>
			void setSelectedDatabaseString(const std::string& databaseSelection)
			{
				_model.DatabaseConfiguration().selectedDatabase(stringToDatabaseType(databaseSelection));
			}

			/// <summary>
			/// Saves current configuration
			/// </summary>
			/// <returns></returns>
			void saveToXml(const std::string& filename)
			{
				std::ofstream outFile(filename);
				outFile << serialize();
				outFile.close();
			}

			void save() 
			{
				saveToXml(_filename);
			}

			void loadFromXml(const std::string& filename)
			{
				load(filename);
			}

			std::string serialize() 
			{
				std::stringstream ss;
				carousel::data::CarouselConfiguration_(ss, _model);
				return ss.str();
			}

		private:

			/// <summary>
			/// Loads configuration from xml file
			/// </summary>
			/// <param name="filename"></param>
			/// <returns></returns>
			bool load(const std::string& filename)
			{
				bool result{ false };

				try
				{
					if (std::filesystem::exists(filename))
					{
						// Load existing configuration
						auto newModel = carousel::data::CarouselConfiguration_(filename, xml_schema::flags::dont_validate);
						_model = *newModel;
						result = true;
					}
				}
				catch (const std::exception& ex)
				{
					carousel::logging::CarouselLogger::instance().error("Loading configuration was not possible: " + std::string(ex.what()));
				}

				return result;
			}

			std::string DatabaseTypeToString(const carousel::data::DatabaseType::value& databaseSelection)
			{
				std::string returnValue;

				switch (databaseSelection)
				{
				case carousel::data::DatabaseType::value::SQLite3:
					returnValue = "SQLite3";
					break;
				default:
					throw carousel::exceptions::NotImplementedException("Database Type " + std::to_string(databaseSelection) + " is not implemented ");
				}

				return returnValue;
			}

			carousel::data::DatabaseType::value stringToDatabaseType(const std::string& databaseType)
			{
				carousel::data::DatabaseType::value returnValue = carousel::data::DatabaseType::value::SQLite3;

				std::string realType = carousel::helpers::string::trim(databaseType);
				if (realType == "SQLite3")
				{
					returnValue = carousel::data::DatabaseType::value::SQLite3;
				}
				else
				{
					throw carousel::exceptions::NotImplementedException("Database Type " + databaseType + " is not implemented ");
				}

				return returnValue;
			}

		};
	}
}