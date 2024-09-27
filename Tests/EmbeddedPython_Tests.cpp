#pragma once

#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include "../Carousel/include/Scripting/CarouselModule.h"
#include "../Carousel/include/Core/CarouselCore.h"

TEST_CASE("Embedded Python tests","[classic]")
{
	SECTION("Python hello world script run")
	{
		// Initialize xerces which is used for serialization
		//xercesc::XMLPlatformUtils::Initialize();

		// Initialize python using pybind11
		//pybind11::scoped_interpreter guard{};

		// Expose the carousel module
		carousel::scripting::expose_module();

        // Execute a Python script that uses the exposed class
		try {
			pybind11::exec(R"(
				import carouseldata
				calc = carouseldata.CarouselSandbox()
				print("Product from C++:", calc.hello_carousel())
			)");

			REQUIRE(true);
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			REQUIRE(false);
		}

		try
		{
			pybind11::eval_file("C:\\Users\\drogo\\Documents\\CarouselProject\\CAROUSEL Core\\Tests\\Scripts\\LoggingScriptTests.py");
			pybind11::eval_file("C:\\Users\\drogo\\Documents\\CarouselProject\\CAROUSEL Core\\Tests\\Scripts\\ConfigurationsScriptTests.py");
		}
		catch (const std::exception& ex)
		{
			std::string message = ex.what();
			REQUIRE(false);
		}
	}
}