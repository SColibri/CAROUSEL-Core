#pragma once
#include <pybind11/pybind11.h>
#include "..\Data\Models\Configurations.h"
#include "..\Data\Models\Project.h"
#include "..\Core\BaseTypes\CarouselSandbox.h"
#include "..\Logging\CarouselLogger.h"

namespace carousel 
 {
	namespace scripting 
	{
		static void expose_models(pybind11::module& carouselModule) {

			// Configurations - python embedding 
		    pybind11::class_<carousel::data::Configurations>(carouselModule, "Configurations")
		        .def_static("instance", &carousel::data::Configurations::instance, pybind11::return_value_policy::reference)
		        .def_property("SelectedDatabaseString", &carousel::data::Configurations::getSelectedDatabaseString, &carousel::data::Configurations::setSelectedDatabaseString)
		        ;

			// Project - python embedding 
		    pybind11::class_<carousel::data::Project>(carouselModule, "Project")
		        .def(pybind11::init<>())
		        .def_property("Id", &carousel::data::Project::getId, &carousel::data::Project::setId)
		        .def_property("Name", &carousel::data::Project::getName, &carousel::data::Project::setName)
		        .def_property("ApiName", &carousel::data::Project::getApiName, &carousel::data::Project::setApiName)
		        .def_property("SoftwareName", &carousel::data::Project::getSoftwareName, &carousel::data::Project::setSoftwareName)
		        .def("load", &carousel::data::Project::load, pybind11::arg("rawData"))
		        .def("get_table_structure", &carousel::data::Project::get_table_structure)
		        ;

			// CarouselSandbox - python embedding 
		    pybind11::class_<carousel::core::CarouselSandbox>(carouselModule, "CarouselSandbox")
		        .def(pybind11::init<>())
		        .def("hello_carousel", &carousel::core::CarouselSandbox::hello_carousel)
		        ;

			// CarouselLogger - python embedding 
		    pybind11::class_<carousel::logging::CarouselLogger>(carouselModule, "CarouselLogger")
		        .def_static("instance", &carousel::logging::CarouselLogger::instance, pybind11::return_value_policy::reference)
		        .def("Info", &carousel::logging::CarouselLogger::Info, pybind11::arg("message"))
		        .def("debug", &carousel::logging::CarouselLogger::debug, pybind11::arg("message"))
		        .def("error", &carousel::logging::CarouselLogger::error, pybind11::arg("message"))
		        .def("warning", &carousel::logging::CarouselLogger::warning, pybind11::arg("message"))
		        ;
		}
	}
}