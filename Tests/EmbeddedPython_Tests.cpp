#pragma once

#include <catch2/catch_test_macros.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include "../Carousel/include/Scripting/CarouselModule.h"
#include <iostream>

TEST_CASE("Embedded Python tests","[classic]")
{
	SECTION("Python hello world script run")
	{
		// Initialize python using pybind11
		pybind11::scoped_interpreter guard{};

		// Expose the carousel module
		carousel::scripting::expose_module();

        // Execute a Python script that uses the exposed class
		try {
			pybind11::exec(R"(
				import carousel
				calc = carousel.CarouselSandbox()
				print("Product from C++:", calc.hello_carousel())
			)");

			REQUIRE(true);
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			REQUIRE(false);
		}
	}
}