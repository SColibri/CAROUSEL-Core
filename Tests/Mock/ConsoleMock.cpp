#pragma once

#include <iostream>
#include <string>

/// <summary>
/// ConsoleMock application
/// </summary>
int main(int argc, char* argv[])
{
	// Start application
	std::cout << "Console application started new version" << std::endl;
	std::string cmd = "MC: ";

	// Flag end
	if (argc > 1)
	{
		cmd = argv[1];
	}
	std::cout << cmd;
	std::cout << std::flush;

	std::string incoming;

	while (true)
	{
		// Incoming command
		std::getline(std::cin, incoming);

		// Use incoming data as output
		std::cout << incoming;

		// Flag end
		std::cout << " " << cmd << std::endl;
		std::cout << std::flush;

		if (incoming == "exit")
		{
			break;
		}
	}
	return 1;
}