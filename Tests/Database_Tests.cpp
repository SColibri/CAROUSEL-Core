#include <iostream>
#include <fstream>
#include <catch2/catch_test_macros.hpp>
#include "../Carousel/include/Data/SharedTypes/carouselModels.h"
#include "../Carousel/include/Data/Models/Project.h"
#include "../Carousel/include/Data/Database/Sqlite3Database.h"
#include "../Carousel/include/Data/Database/DatabaseTable.h"
#include "../Carousel/include/Data/Database/DatabaseAdapterManager.h"
#include "../Carousel/include/Logging/CarouselLogger.h"
#include "../Carousel/include/Helpers/Converters.h"


#pragma region Mock classes
/// <summary>
/// Mock object used for testing saving data into the database using Primary Key ID. 
/// </summary>
class ProjectMock : public carousel::data::IDatabaseObject
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
	ProjectMock() : carousel::data::IDatabaseObject()
	{
		// Default values
		_model.Id().set(-1);
		_model.Name().set("Project name");
		_model.ApiName().set("");
		_model.SoftwareName().set("");
	}

public:
	/// <summary>
	/// Get Id
	/// </summary>
	/// <returns></returns>
	const int& getId() { return _model.Id().get(); }

	/// <summary>
	/// Set Id
	/// </summary>
	/// <param name="newValue"></param>
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
	void setSoftwareName(const std::string& newName) { _model.SoftwareName().set(newName); }

public: // IDatabaseObject implementation

	virtual int load(std::vector<std::string>& rawData) override
	{
		setId(-1);
		if (rawData.size() < get_table_structure().size()) return 1;
		setId(carousel::helpers::converters::convertFromString<int>(rawData[0]));
		setName(rawData[1]);
		setApiName(rawData[2]);
		setSoftwareName(rawData[3]);

		return 0;
	}

	virtual carousel::data::DatabaseTable& get_table_structure() override
	{
		static carousel::data::DatabaseTable table("Project");
		static bool initialized = false;

		if (!initialized)
		{
			table.addColumn("Id", typeid(int).name(), &ProjectMock::getId, &ProjectMock::setId, carousel::data::DatabaseConstraintType::PRIMARY_KEY);
			table.addColumn("ProjectName", typeid(std::string).name(), &ProjectMock::getName, &ProjectMock::setName, carousel::data::DatabaseConstraintType::COLUMN);
			table.addColumn("ApiName", typeid(std::string).name(), &ProjectMock::getApiName, &ProjectMock::setApiName, carousel::data::DatabaseConstraintType::COLUMN);
			table.addColumn("SoftwareName", typeid(std::string).name(), &ProjectMock::getSoftwareName, &ProjectMock::setSoftwareName, carousel::data::DatabaseConstraintType::COLUMN);
			initialized = true;
		}

		return table;
	}
};

/// <summary>
/// 
/// </summary>
class ProjectMockUniqueConstraints : public carousel::data::IDatabaseObject
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
	ProjectMockUniqueConstraints() : carousel::data::IDatabaseObject()
	{
		// Default values
		_model.Id().set(-1);
		_model.Name().set("Project name");
		_model.ApiName().set("");
		_model.SoftwareName().set("");
	}

public:
	/// <summary>
	/// Get Id
	/// </summary>
	/// <returns></returns>
	const int& getId() { return _model.Id().get(); }

	/// <summary>
	/// Set Id
	/// </summary>
	/// <param name="newValue"></param>
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
	void setSoftwareName(const std::string& newName) { _model.SoftwareName().set(newName); }

public: // IDatabaseObject implementation

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

	virtual carousel::data::DatabaseTable& get_table_structure() override
	{
		static carousel::data::DatabaseTable table("ProjectsUnique");
		static bool initialized = false;

		if (!initialized)
		{
			table.addColumn("Id", typeid(std::string).name(), &ProjectMockUniqueConstraints::getId, &ProjectMockUniqueConstraints::setId, carousel::data::DatabaseConstraintType::UNIQUE);
			table.addColumn("ProjectName", typeid(std::string).name(), &ProjectMockUniqueConstraints::getName, &ProjectMockUniqueConstraints::setName, carousel::data::DatabaseConstraintType::COLUMN);
			table.addColumn("ApiName", typeid(std::string).name(), &ProjectMockUniqueConstraints::getApiName, &ProjectMockUniqueConstraints::setApiName, carousel::data::DatabaseConstraintType::COLUMN);
			table.addColumn("SoftwareName", typeid(std::string).name(), &ProjectMockUniqueConstraints::getSoftwareName, &ProjectMockUniqueConstraints::setSoftwareName, carousel::data::DatabaseConstraintType::UNIQUE);
			initialized = true;
		}

		return table;
	}
};

#pragma endregion


TEST_CASE("Carousel Database", "[classic]")
{
	// Initialize xerces which is used for serialization
	xercesc::XMLPlatformUtils::Initialize();

	ProjectMock project;

	carousel::data::DatabaseConfiguration databaseConfiguration;
	databaseConfiguration.databaseDirectory = "Database";
	databaseConfiguration.databaseFileName = "carouselExample.db";
	databaseConfiguration.selectedDatabaseType = carousel::data::DatabaseConfiguration::TYPE::SQLITE3;

	SECTION("Basic database operations")
	{
		carousel::data::Sqlite3Database db(&databaseConfiguration);
		db.connect();

		SECTION("Create and remove table")
		{
			// Add table
			db.createTable(&project.get_table_structure());
			auto tableName = db.getTableNames();
			auto it = std::find(tableName.begin(), tableName.end(), project.get_table_structure().getTableName());
			REQUIRE(it != tableName.end());

			// Remove table test
			db.dropTable(&project.get_table_structure());
			tableName = db.getTableNames();
			it = std::find(tableName.begin(), tableName.end(), project.get_table_structure().getTableName());
			REQUIRE(it == tableName.end());
		}

		// cleanup
		db.disconnect();
	}
}

TEST_CASE("DatabaseAdapterManager Tests")
{
	// Initialize xerces which is used for serialization
	xercesc::XMLPlatformUtils::Initialize();

	carousel::data::DatabaseConfiguration databaseConfiguration;
	databaseConfiguration.databaseDirectory = "Database";
	databaseConfiguration.databaseFileName = "DatabaseAdapterManagerExample.db";
	databaseConfiguration.selectedDatabaseType = carousel::data::DatabaseConfiguration::TYPE::SQLITE3;

	carousel::data::Sqlite3Database db(&databaseConfiguration);
	db.connect();

	carousel::data::DatabaseAdapterManager adapter(db);

	SECTION("Save and load DatabaseObject with primaryKey")
	{
		// Mock object
		ProjectMock projectWithPrimaryKey;
		projectWithPrimaryKey.setSoftwareName("Software name test");
		projectWithPrimaryKey.setApiName("API name test");
		projectWithPrimaryKey.setName("Name test");

		projectWithPrimaryKey.get_table_structure()[0]->setValue(&projectWithPrimaryKey, "2");
		auto& testyon = projectWithPrimaryKey.get_table_structure()[0]->getValue(&projectWithPrimaryKey);
		
		// Add table
		db.createTable(&projectWithPrimaryKey.get_table_structure());
		auto tableName = db.getTableNames();
		auto it = std::find(tableName.begin(), tableName.end(), projectWithPrimaryKey.get_table_structure().getTableName());
		REQUIRE(it != tableName.end());

		// Save new item into database
		db.save(&projectWithPrimaryKey);
		REQUIRE(projectWithPrimaryKey.getId() > -1);

		// Modify and save object - Update test
		projectWithPrimaryKey.setName("Updated project " + std::to_string(projectWithPrimaryKey.getId()));
		db.save(&projectWithPrimaryKey);

		// Save multiple objects
		std::vector<carousel::data::IDatabaseObject*> testy;
		for (size_t i = 0; i < 20; i++)
		{
			ProjectMock* projectWithPrimaryKey = new ProjectMock();
			projectWithPrimaryKey->setSoftwareName("Software name test");
			projectWithPrimaryKey->setApiName("API name test");
			projectWithPrimaryKey->setName("Name test");
		}
	}

	SECTION("Save and load DatabaseObject with Unique constraints")
	{
		// Mock object
		ProjectMockUniqueConstraints projectWithUniqueConstraint;
		projectWithUniqueConstraint.setSoftwareName("Software name test");
		projectWithUniqueConstraint.setApiName("API name test");
		projectWithUniqueConstraint.setName("Name test");
		projectWithUniqueConstraint.setId(1);
		
		// Add table
		db.createTable(&projectWithUniqueConstraint.get_table_structure());
		auto tableName = db.getTableNames();
		auto it = std::find(tableName.begin(), tableName.end(), projectWithUniqueConstraint.get_table_structure().getTableName());
		REQUIRE(it != tableName.end());

		// Save new item into database
		db.save(&projectWithUniqueConstraint);
	}

	// cleanup
	db.disconnect();
}