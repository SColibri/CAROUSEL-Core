#pragma once

#include <string>

namespace carousel
{
	namespace core 
	{
		/// <summary>
		/// Carousel sandbox is just a class used for playing around with Carousel
		/// functionality
		/// </summary>
		class CarouselSandbox
		{
		public:
			CarouselSandbox() = default;

			/// <summary>
			/// Says hello
			/// </summary>
			/// <tag>#pythonMethod</tag>
			std::string hello_carousel() {
				return "Hi from CAROUSEL core module :)";
			}
		};

	}
}