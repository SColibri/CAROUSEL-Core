#pragma once

#include <catch2/catch_test_macros.hpp>
#include "../Carousel/include/IpcTools/Ipc.h"


TEST_CASE("Ipc", "[classic]")
{
	SECTION("Ipc connection")
	{
		//carousel::ipcTools::Ipc newIpc("./ConsoleMock.exe");
		carousel::ipcTools::Ipc newIpc("C:\\Program Files\\MatCalc 6\\mcc.exe");
		
		std::string tz = newIpc.readCommand();

		std::string tz1 = newIpc.sendCommand("SendThisCommand of chars whoooohooo");

		std::string tz2 = newIpc.readCommand();

		std::string tz3 = newIpc.sendCommand("exit");

		std::string tz4 = newIpc.readCommand();

	}
}
