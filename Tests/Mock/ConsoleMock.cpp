#pragma once

#include <iostream>
#include <string>

/// <summary>
/// ConsoleMock application
/// </summary>
int main(int argc, char* argv[])
{
	// Start application
	std::cout << "Console application started ne version" << std::endl;

	// Flag end
	if (argc > 0)
	{
		std::cout << "MC:" << std::endl;
	}
	std::cout << "MC:" << std::endl << std::feof;
	std::cout << std::flush;

	std::string incoming;

	while (true)
	{
		// Incoming command
		std::cin >> incoming;
		std::cin >> incoming;

		// Use incoming data as output
		std::cout << incoming << std::feof;

		// Flag end
		if (argc > 0)
		{
			std::cout << "MC:" << std::endl;
		}

		std::cout << "MC:" << std::endl;
		std::cout << std::flush;

		if (incoming == "exit")
		{
			break;
		}
	}
	return 1;
}