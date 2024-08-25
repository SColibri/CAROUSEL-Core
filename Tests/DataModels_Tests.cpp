#include <iostream>
#include <fstream>
#include <catch2/catch_test_macros.hpp>
#include "../Carousel/include/Data/SharedTypes/carouselModels.h"
#include "../Carousel/include/Data/Models/Project.h"
#include "../Carousel/include/Logging/CarouselLogger.h"


TEST_CASE("Carousel Data Models", "[classic]")
{
	SECTION("Saving and loading file")
	{
		carousel::data::CaseModel testy;
		testy.Id(1);
		
		xml_schema::namespace_infomap map;
		map[""].name = "http://www.carousel.com/carousel/data";
		std::ostringstream oss;
		std::ofstream ofs("CaseModel.xml");
		carousel::data::CaseModel_(oss, testy, map);
		std::string xmldata = oss.str();

		std::cout << oss.str() << std::endl;

		// Create an instance of CaseModel to hold the deserialized data
		carousel::data::CaseModel case_model_instance;

		// Deserialize from the XML string
		xml_schema::properties props;

		std::istringstream iss(xmldata);
		xml_schema::flags flags = xml_schema::flags::dont_validate;
		std::unique_ptr<carousel::data::CaseModel> testys = carousel::data::CaseModel_(iss, flags, props);
		testys->Name();

		carousel::logging::CarouselLogger::instance().Info("Testing log message");
		carousel::logging::CarouselLogger::instance().Info("Testing log message");

		REQUIRE(1 == 1);
	}

	SECTION("Projects")
	{
		// Initialize xerces which is used for serialization
		xercesc::XMLPlatformUtils::Initialize();

		// Initialize Project
		carousel::data::Project project{};
		int idValue = project.getId();
		std::string projectName = project.getName();

		REQUIRE(idValue == -1);
		REQUIRE(projectName.length() > 0);

		// Setters and getters
		project.setId(100);
		REQUIRE(project.getId() == 100);

		project.setName("100");
		REQUIRE(project.getName() == "100");

		// Serialized Project
		std::string serializedProject = project.serialize();
	}
}