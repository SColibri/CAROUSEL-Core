#pragma once

#include <catch2/catch_test_macros.hpp>
#include "../Carousel/include/IpcTools/IpcPipeHandler.h"
#include "../Carousel/include/Exceptions/IpcCommunicationException.h"


TEST_CASE("Ipc", "[classic]")
{
	SECTION("Ipc communication")
	{
		carousel::ipcTools::IpcPipeHandler newIpc("./ConsoleMock.exe MC:", "MC: ", "MC:", 30);
		//carousel::ipcTools::IpcPipeHandler newIpc("C:\\Program Files\\MatCalc 6\\mcc.exe", "MC: ", "", 10);

		std::string pipeData;
		std::string flag = " MC:\r\n";

		// ConsoleMock is waiting for input, reading should not block and return and empty string
		pipeData = newIpc.read();
		REQUIRE(true);

		// Ipc status should be ready
		REQUIRE(newIpc.isReady());

		// Short command
		newIpc.send("Invalid content\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		pipeData = newIpc.read();
		REQUIRE(pipeData == "Invalid content" + flag);

		// Long command
		std::string lorem = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt.Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem.Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur ? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur ?";
		newIpc.send(lorem + "\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		pipeData = newIpc.read();
		REQUIRE(pipeData == lorem + flag);
	}

	SECTION("Ipc process that can't be initialized")
	{
		try
		{
			// Using an invalid path
			carousel::ipcTools::IpcPipeHandler newIpc("./Invalid.exe MC:", "MC: ", "MC:", 1);
			std::string pipeData = newIpc.read();
			REQUIRE(false);
		}
		catch (const carousel::exceptions::IpcCommunicationException& e)
		{
			REQUIRE(true);
		}

	}
}
