#pragma once

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <iostream>
#include <string>

namespace carousel
{
	namespace data
	{
		/// <summary>
		/// Interface for serializable objects 
		/// </summary>
		class SerializableObject
		{
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			SerializableObject(const std::string& objectName)
			{
				// Initialize DOM
				XMLCh tempStr[100];
				xercesc::XMLString::transcode("LS", tempStr, 99);
				_implementation = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
				_document = _implementation->createDocument(0, xercesc::XMLString::transcode(objectName.c_str()), 0);
				_root = _document->getDocumentElement();

			}

			/// <summary>
			/// Save current object to XML
			/// </summary>
			virtual void saveToXml(const std::string& filepath) = 0;

			/// <summary>
			/// Loads from xml file
			/// </summary>
			virtual void loadFromXml(const std::string& filepath) = 0;

		private:
			std::string serialize() 
			{
				xercesc::DOMLSSerializer* serializer = ((xercesc::DOMImplementationLS*)_implementation)->createLSSerializer();
				xercesc::DOMLSOutput* output = ((xercesc::DOMImplementationLS*)_implementation)->createLSOutput();
				xercesc::MemBufFormatTarget* target = new xercesc::MemBufFormatTarget();
				output->setByteStream(target);

				// Convert memory buffer to std::string
				std::string xmlString = std::string((const char*)target->getRawBuffer(), target->getLen());

				// Clean up
				output->release();
				serializer->release();
				delete target;

				

				return xmlString;
			}

			/// <summary>
			/// DOM document
			/// </summary>
			xercesc::DOMDocument* _document;

			/// <summary>
			/// Object Root node
			/// </summary>
			xercesc::DOMElement* _root;

			/// <summary>
			/// XML implementation
			/// </summary>
			xercesc::DOMImplementation* _implementation;
		};
	}
}