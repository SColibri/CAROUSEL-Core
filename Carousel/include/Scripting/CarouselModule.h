#pragma once

#include <string>
#include <pybind11/pybind11.h>
#include "../Core/BaseTypes/CarouselSandbox.h"
#include "../IpcTools/IpcPipeHandler.h"
#include "../Data/Models/Project.h"
#include "carouselDataPythonBindings.h"

namespace carousel
{
	namespace scripting
	{
		static void expose_ipcTools(pybind11::module& carouselModule);

		static void expose_module() 
		{
			// Create a Python module dynamically
			pybind11::module carouselModule("carousel");
			pybind11::module carouselDataModule("carouseldata");

			// Insert the module into sys.modules to make it importable in Python
			pybind11::dict sys_modules = pybind11::module::import("sys").attr("modules").cast<pybind11::dict>();
			sys_modules["carousel"] = carouselModule;
			sys_modules["carouseldata"] = carouselDataModule;

			// carousel data
			expose_models(carouselDataModule);

			// 
			expose_ipcTools(carouselModule);
		}

		/// <summary>
		/// Exposes the IPC tools
		/// </summary>
		/// <param name="carouselModule"></param>
		static void expose_ipcTools(pybind11::module& carouselModule)
		{
			pybind11::class_<carousel::ipcTools::IpcPipeHandler>(carouselModule, "IpcPipeHandler")
				.def(pybind11::init<std::string, std::string, std::string, int>(),
					pybind11::arg("pathToExe"),
					pybind11::arg("endFlag"),
					pybind11::arg("parameters") = "",
					pybind11::arg("timeout") = carousel::ipcTools::TIMEOUT)
				.def("isReady", &carousel::ipcTools::IpcPipeHandler::isReady)
				.def("send", &carousel::ipcTools::IpcPipeHandler::send, pybind11::arg("command"))
				.def("read", &carousel::ipcTools::IpcPipeHandler::read);
		}

	}
}