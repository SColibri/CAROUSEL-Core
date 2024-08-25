#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <stdlib.h>
#include <charconv>
#include "../Carousel/include/Core/BaseTypes/XmlSerializable.h"
#include "../Carousel/include/Helpers/Converters.h"

/// <summary>
/// Class used for testing the implementation of a XmlSerializable object
/// </summary>
class SerializableTest : public carousel::core::XmlSerializable
{
public:
	/// <summary>
	/// String property
	/// </summary>
	std::string property1{ "FirstValue" };

	/// <summary>
	/// Another string property
	/// </summary>
	std::string property2{ "SecondValue" };

	/// <summary>
	/// int property
	/// </summary>
	int property3{ 0 };

	/// <summary>
	/// double property
	/// </summary>
	double property4{ 1E-10 };

	/// <summary>
	/// Constructor
	/// 
	/// 1: Base constructor needs the name of the class that is implementing
	/// 2: Register serializable properties 
	/// </summary>
	SerializableTest() : carousel::core::XmlSerializable("SearializableTest")
	{
		// Register all properties at initialization, use methods or lambda functions
		// to return the values related to each property and another function that
		// updates the value from a string value.
		registerProperty("property1",
			[&] {return property1; },
			[&](const std::string& newValue) {property1 = newValue; });

		registerProperty("property2",
			[&] {return property2; },
			[&](const std::string& newValue) {property2 = newValue; });

		registerProperty("property3",
			[&] {return std::to_string(property3); },
			[&](const std::string& newValue) {property3 = std::stoi(newValue); });

		registerProperty("property4",
			[&] {return carousel::helpers::converters::convertToString(property4); },
			[&](const std::string& newValue) {property4 = std::stod(newValue); });
	}
};

/// <summary>
/// Same as SerializableTest but with less properties
/// </summary>
class SerializableTestOther : public carousel::core::XmlSerializable
{
public:
	/// <summary>
	/// String property
	/// </summary>
	std::string otherProperty{ "OtherValue" };

	/// <summary>
	/// Constructor
	/// 
	/// 1: Base constructor needs the name of the class that is implementing
	/// 2: Register serializable properties 
	/// </summary>
	SerializableTestOther() : carousel::core::XmlSerializable("SerializableTestOther")
	{
		// Register all properties at initialization, use methods or lambda functions
		// to return the values related to each property and another function that
		// updates the value from a string value.
		registerProperty("otherProperty",
			[&] {return otherProperty; },
			[&](const std::string& newValue) {otherProperty = newValue; });
	}
};

TEST_CASE("Core_Base_types", "[classic]")
{
	SECTION("Serializable object - Save and load")
	{
		// Initialize xerces which is used for serialization
		xercesc::XMLPlatformUtils::Initialize();

		// Create Serializable object, modify and save into file
		SerializableTest example;
		example.property1 = "Changed at runtime";
		example.property2 = "This is also changed at runtime";
		example.property3 = 1010;
		example.property4 = 123E5;
		example.saveToXml("SerializableTest.xml");

		// Create Serializable object, and load previous file
		SerializableTest example_empty;
		example_empty.loadFromXml("SerializableTest.xml");

		// Both objects should have the same data.
		REQUIRE(example.property1 == example_empty.property1);
		REQUIRE(example.property2 == example_empty.property2);
		REQUIRE(example.property3 == example_empty.property3);
		REQUIRE(example.property4 == example_empty.property4);

		// Load incorrect structure
		SerializableTestOther exampleIncorrectStructure;
		
		// Exception should be thrown, since the file to be loaded
		// corresponds to another structure
		REQUIRE_THROWS_AS(exampleIncorrectStructure.loadFromXml("SerializableTest.xml"), std::runtime_error);

	}
}