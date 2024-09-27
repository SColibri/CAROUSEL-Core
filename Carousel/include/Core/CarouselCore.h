#pragma once

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <xercesc/util/PlatformUtils.hpp>
#include "../Data/Models/Configurations.h"

namespace carousel
{
	namespace core
	{
		/// <summary>
		/// 
		/// </summary>
		class CarouselCore
		{
		private:
			/// <summary>
			/// Configurations
			/// </summary>
			// carousel::data::Configurations _config;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			CarouselCore() 
			{
				initializeCore();
			}

			/// <summary>
			/// Destructor
			/// </summary>
			~CarouselCore() 
			{
				pybind11::finalize_interpreter();
			}

		private:
			/// <summary>
			/// Initialize carousel core
			/// </summary>
			void initializeCore() 
			{
				// Initialize services
				pybind11::initialize_interpreter();
				xercesc::XMLPlatformUtils::Initialize();
			}

			/// <summary>
			/// 
			/// </summary>
			void initializeConfiguration() 
			{
			
			}
		};

		/// <summary>
		/// 
		/// </summary>
		static CarouselCore initializer;
	}
}