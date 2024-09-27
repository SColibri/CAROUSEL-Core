#pragma once

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/Xerces_autoconf_config.hpp>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <fstream>
#include "../Interfaces/ISerializableObject.h"

namespace carousel
{
	namespace core
	{
		/// <summary>
		/// Abstract class SerializableObject represents an object that can serialize into an XML document or string.
		/// </summary>
		class XmlSerializable : public ISerializableObject
		{
		private:
			/// <summary>
			/// Structure for node element object
			/// </summary>
			struct Element
			{
				/// <summary>
				/// Dom element
				/// </summary>
				xercesc::DOMElement* element;

				/// <summary>
				/// Dom element
				/// </summary>
				xercesc::DOMNode* child;

				/// <summary>
				/// Pointer to function that returns the value pointing to the DOM element
				/// as a string object
				/// </summary>
				std::function<std::string()> stringValue;

				/// <summary>
				/// Pointer to function for updating the model data
				/// </summary>
				std::function<void(const std::string&)> updateValue;
			};

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

			/// <summary>
			/// Node elements contained in root
			/// </summary>
			std::map<std::string, Element> _elements;

		public:
			/// <summary>
			/// Constructor
			/// </summary>
			XmlSerializable(const std::string& objectName)
			{
				// Initialize DOM
				XMLCh tempStr[100];
				xercesc::XMLString::transcode("LS", tempStr, 99);
				_implementation = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
				_document = _implementation->createDocument(0, xercesc::XMLString::transcode(objectName.c_str()), 0);
				_root = _document->getDocumentElement();
			}

#pragma region ISerializableObject 
			void saveToXml(const std::string& filepath) override
			{
				// Empty elements means that ISerializableObject is not implemented
				// correctly (Missing elements)
				if (_elements.empty() == 0)
				{
					std::string xmlContent = serialize();
					saveToFile(filepath, xmlContent);
				}
			}

			void loadFromXml(const std::string& filepath) override
			{
				// initialize parse with nullptr
				xercesc::XercesDOMParser* parser = nullptr;

				try
				{
					parser = parseFile(filepath);

					// Create document and fill nodes
					xercesc::DOMDocument* document = parser->getDocument();
					xercesc::DOMElement* root = document->getDocumentElement();

					// Check if root tags refer to the same object
					std::string loadedRootName(xercesc::XMLString::transcode(root->getTagName()));
					std::string currentRootName(xercesc::XMLString::transcode(_root->getTagName()));
					if (loadedRootName == currentRootName)
					{
						// Read each node and update child node
						xercesc::DOMNodeList* children = root->getChildNodes();
						size_t childrenCount = children->getLength();

						for (size_t i = 0; i < childrenCount; i++)
						{
							// Each element represents a property
							xercesc::DOMElement* childElement = dynamic_cast<xercesc::DOMElement*>(children->item(i));
							if (childElement != nullptr)
							{
								// Get property name and check if it is registered. If this property is registered
								// update child node.
								std::string childTagName(xercesc::XMLString::transcode(childElement->getTagName()));
								auto isContained = _elements.find(childTagName) != _elements.end();
								if (isContained)
								{
									// If type is of DOMText, update using replaceWholeText
									xercesc::DOMText* childText = dynamic_cast<xercesc::DOMText*>(_elements[childTagName].child);
									if (childText != nullptr)
									{
										childText->replaceWholeText(((xercesc::DOMText*)childElement->getFirstChild())->getWholeText());
									}
									else
									{
										// For now we keep this simple and just implement the DOMText
										throw std::runtime_error("Loading xml with children type other than DOMText is not yet implemented.");
									}
								}
							}
						}
					}
					else
					{
						// Wrong root name tag
						throw std::runtime_error("XmlSerializable can't load this file into this structure.");
					}

					// cleanup and update values to data model
					updateValuesToSource();
					delete parser;
				}
				catch (const xercesc::DOMException& e) {
					char* message = xercesc::XMLString::transcode(e.getMessage());
					std::string errorMessage = "DOMException: " + std::string(message);
					xercesc::XMLString::release(&message);
					delete parser;
					throw std::runtime_error(errorMessage);
				}
				catch (const xercesc::XMLException& e) {
					char* message = xercesc::XMLString::transcode(e.getMessage());
					std::string errorMessage = "XMLException: " + std::string(message);
					xercesc::XMLString::release(&message);
					delete parser;
					throw std::runtime_error(errorMessage);
				}
				catch (const std::exception& e) {
					delete parser;
					throw; // Rethrow the caught exception
				}
			}
#pragma endregion

		private:
			/// <summary>
			/// Saves string content into a file
			/// </summary>
			/// <param name="filename">full path</param>
			/// <param name="content">content to be saved into the file</param>
			void saveToFile(const std::string& filename, const std::string& content) {
				std::ofstream outFile(filename);

				if (outFile.is_open()) {
					outFile << content;
					outFile.close();
					std::cout << "File saved successfully." << std::endl;
				}
				else {
					std::cerr << "Error opening file for writing." << std::endl;
				}
			}

			xercesc::XercesDOMParser* parseFile(const std::string& filename)
			{
				// Create parser without schema validation
				xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
				parser->setValidationScheme(xercesc::XercesDOMParser::Val_Never);
				parser->setDoNamespaces(false);
				parser->setDoSchema(false);
				parser->setLoadExternalDTD(false);

				xercesc::DOMDocument* document = nullptr;

				try
				{
					// Parse file
					parser->parse(xercesc::XMLString::transcode(filename.c_str()));
				}
				catch (const std::exception& ex)
				{
					throw std::runtime_error(ex.what());
				}

				return parser;
			}

		protected:
			/// <summary>
			/// Registers a property to be saved or loaded
			/// </summary>
			/// <param name="propertyName">name of the property</param>
			/// <param name="valueFunction">Function that returns the value as a string</param>
			void registerProperty(const std::string& propertyName, std::function<std::string()> valueFunction, std::function<void(const std::string&)> updateFunction)
			{
				// Check if property is not already assigned
				if (_elements.count(propertyName) == 0)
				{
					// Create new node and add to root document
					xercesc::DOMElement* domElement = _document->createElement(xercesc::XMLString::transcode(propertyName.c_str()));
					_root->appendChild(domElement);

					xercesc::DOMText* nameText = _document->createTextNode(xercesc::XMLString::transcode(valueFunction().c_str()));
					domElement->appendChild(nameText);

					// Create reference to node for updating the value before save
					Element newElement;
					newElement.stringValue = valueFunction;
					newElement.updateValue = updateFunction;
					newElement.element = domElement;
					newElement.child = nameText;

					// Add into reference list
					_elements.insert(std::pair<std::string, Element>(propertyName, newElement));
				}
				else
				{
					throw std::exception("");
				}
			}

			/// <summary>
			/// Updates DOM document with current model values
			/// </summary>
			void updateValuesToSource()
			{
				for (auto& element : _elements)
				{
					auto fr = dynamic_cast<xercesc::DOMText*>(element.second.child);
					std::string testy(xercesc::XMLString::transcode(fr->getWholeText()));
					element.second.updateValue(testy);
				}
			}

			/// <summary>
			/// Updates model values to what this DOM contains
			/// </summary>
			void updateValuesFromSource()
			{
				for (auto& element : _elements)
				{
					updateValueFromSource(element.second);
				}
			}

			/// <summary>
			/// Updates DOM element using data from model
			/// </summary>
			/// <param name="element">Element, reference to property</param>
			void updateValueFromSource(const Element& element)
			{
				// If object is of type DOMText
				if (dynamic_cast<xercesc::DOMText*>(element.child) != nullptr)
				{
					((xercesc::DOMText*)element.child)->replaceWholeText(xercesc::XMLString::transcode(element.stringValue().c_str()));
				}
			}


		public:
			/// <summary>
			/// Serializes object to XML and returns the structure in a string
			/// </summary>
			virtual std::string serialize()
			{
				// update values before serialize
				updateValuesFromSource();

				// setup dom serializer and targets
				xercesc::DOMLSSerializer* serializer = ((xercesc::DOMImplementationLS*)_implementation)->createLSSerializer();
				if (serializer->getDomConfig()->canSetParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
					serializer->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
				}

				xercesc::DOMLSOutput* output = ((xercesc::DOMImplementationLS*)_implementation)->createLSOutput();
				xercesc::MemBufFormatTarget* target = new xercesc::MemBufFormatTarget();
				output->setByteStream(target);

				// Serialize the DOM document to memory buffer
				serializer->write(_document, output);

				// Convert memory buffer to std::string
				std::string xmlString = std::string((const char*)target->getRawBuffer(), target->getLen());

				// Clean up
				output->release();
				serializer->release();
				delete target;

				return xmlString;
			}
		};


	}
}