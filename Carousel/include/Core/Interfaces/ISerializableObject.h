#pragma once
#include <string>

namespace carousel
{
	namespace core
	{
		/// <summary>
		/// Interface for a serializable class
		/// </summary>
		class ISerializableObject
		{
		public:
			/// <summary>
			/// Save current object to XML
			/// </summary>
			virtual void saveToXml(const std::string& filepath) = 0;

			/// <summary>
			/// Loads from xml file
			/// </summary>
			virtual void loadFromXml(const std::string& filepath) = 0;
		};
	}
}