#pragma once

#include <string>
#include "../Database/IDatabaseObject.h"
#include "../Database/DatabaseColumn.h"
#include "../Database/DatabaseTable.h"
#include "../Database/DatabaseConstants.h"
#include "../SharedTypes/carouselModels.h"
#include "../../Core/BaseTypes/XmlSerializable.h"

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Project model is the root node for all data generated in any calculation set
		/// </summary>
		class Project : public carousel::data::IDatabaseObject, public carousel::core::XmlSerializable
		{
		private:
			/// <summary>
			/// Base model data
			/// </summary>
			carousel::data::ProjectModel _model{};

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			Project() : carousel::data::IDatabaseObject(), carousel::core::XmlSerializable("Project")
			{
				// Default values
				_model.Id().set(carousel::data::DEFAULT_ID);
				_model.Name().set("Project name");
				_model.ApiName().set("");
				_model.SoftwareName().set("");

				// Serialization setup
				registerProperty("Id", [&] {return std::to_string(getId()); }, [&](const std::string& newValue) {setId(std::stoi(newValue)); });
				registerProperty("ProjectName", [&] {return getName(); }, [&](const std::string& newValue) {setName(newValue); });
				registerProperty("ApiName", [&] {return getApiName(); }, [&](const std::string& newValue) {setApiName(newValue); });
				registerProperty("SoftwareName", [&] {return getSoftwareName(); }, [&](const std::string& newValue) {setSoftwareName(newValue); });
			}

		public: // Tags: #python

			/// <summary>
			/// Get Id
			/// </summary>
			/// <returns></returns>
			const int& getId() { return _model.Id().get(); }

			/// <summary>
			/// Set Id
			/// </summary>
			/// <param name="newValue"></param>
			/// <tag>#pythonProperty</tag>
			void setId(const int& newValue) { _model.Id().set(newValue); }

			/// <summary>
			/// Get name
			/// </summary>
			/// <returns></returns>
			const std::string& getName() { return _model.Name().get(); }

			/// <summary>
			/// Set name
			/// </summary>
			/// <param name="newName"></param>
			/// <tag>#pythonProperty</tag>
			void setName(const std::string& newName) { _model.Name().set(newName); }

			/// <summary>
			/// Get api name that is being used
			/// </summary>
			/// <returns></returns>
			const std::string& getApiName() { return _model.ApiName().get(); }

			/// <summary>
			/// Set api name that is being used
			/// </summary>
			/// <param name="newName"></param>
			/// <tag>#pythonProperty</tag>
			void setApiName(const std::string& newName) { _model.ApiName().set(newName); }

			/// <summary>
			/// Get name of software used for all calculations
			/// </summary>
			/// <returns></returns>
			const std::string& getSoftwareName() { return _model.SoftwareName().get(); }

			/// <summary>
			/// Set name of software used for all calculations
			/// </summary>
			/// <param name="newName"></param>
			/// <tag>#pythonProperty</tag>
			void setSoftwareName(const std::string& newName) { _model.SoftwareName().set(newName); }

		public: // IDatabaseObject implementation

			/// <summary>
			/// 
			/// </summary>
			/// <tag>#pythonMethod</tag>
			virtual int load(std::vector<std::string>& rawData) override
			{
				setId(-1);
				if (rawData.size() < get_table_structure().size()) return 1;
				setId(std::stoi(rawData[0]));
				setName(rawData[1]);
				setApiName(rawData[2]);
				setSoftwareName(rawData[3]);

				return 0;
			}

			/// <summary>
			/// 
			/// </summary>
			/// <tag>#pythonMethod</tag>
			virtual carousel::data::DatabaseTable& get_table_structure() override
			{
				static carousel::data::DatabaseTable table("Project");
				static bool initialized = false;

				if (!initialized)
				{
					table.addColumn("Id", typeid(int).name(), &Project::getId, &Project::setId, carousel::data::DatabaseConstraintType::PRIMARY_KEY);
					table.addColumn("ProjectName", typeid(std::string).name(), &Project::getName, &Project::setName, carousel::data::DatabaseConstraintType::COLUMN);
					table.addColumn("ApiName", typeid(std::string).name(), &Project::getApiName, &Project::setApiName, carousel::data::DatabaseConstraintType::COLUMN);
					table.addColumn("SoftwareName", typeid(std::string).name(), &Project::getSoftwareName, &Project::setSoftwareName, carousel::data::DatabaseConstraintType::COLUMN);
					initialized = true;
				}

				return table;
			}
		};
	}
}