#pragma once

#include <catch2/catch_test_macros.hpp>
#include "../Carousel/include/IpcTools/IpcPipeHandler.h"


TEST_CASE("Ipc", "[classic]")
{
	SECTION("Ipc connection")
	{
		//carousel::ipcTools::Ipc newIpc("./ConsoleMock.exe");
		carousel::ipcTools::IpcPipeHandler newIpc("C:\\Program Files\\MatCalc 6\\mcc.exe", "MC: ", "", 10);
		auto tzr = newIpc.isReady();

		
		auto testy = newIpc.read();

		newIpc.send("Invalid content");

		auto testy4 = newIpc.read();

	}
}
