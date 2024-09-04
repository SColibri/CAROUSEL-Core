#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <filesystem>
#include <log4cplus/log4cplus.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <mutex>

namespace carousel
{
	namespace logging
	{
		/// <summary>
		/// Logger service for carousel
		/// </summary>
		class CarouselLogger
		{
		private:
			/// <summary>
			/// Logger
			/// </summary>
			log4cplus::Logger _log4cpp;

			/// <summary>
			/// True if logger is initialized
			/// </summary>
			bool _isInitialized{ false };

			/// <summary>
			/// Logging mutex
			/// </summary>
			std::mutex _logMutex;

			/// <summary>
			/// Private constructor (Singleton)
			/// </summary>
			CarouselLogger()
			{
				try
				{
					// Initialize logger
					log4cplus::initialize();

					// Create a logger instance
					_log4cpp = log4cplus::Logger::getRoot();

					// Path to logs
					std::string logDirectory = "logs";
					std::string logPrefix = "carousel";
					std::string logSuffix = ".log";

					// Specify appender properties (filename and mode)
					log4cplus::helpers::Properties appenderProperties;
					appenderProperties.setProperty(LOG4CPLUS_TEXT("File"), LOG4CPLUS_TEXT("logs/carousel.log"));
					appenderProperties.setProperty(LOG4CPLUS_TEXT("MaxFileSize"), LOG4CPLUS_TEXT("10MB"));
					appenderProperties.setProperty(LOG4CPLUS_TEXT("MaxBackupIndex"), LOG4CPLUS_TEXT("5"));
					appenderProperties.setProperty(LOG4CPLUS_TEXT("Append"), LOG4CPLUS_TEXT("true"));
					appenderProperties.setProperty(LOG4CPLUS_TEXT("CreateDirs"), LOG4CPLUS_TEXT("true"));

					// Create a FileAppender using properties
					log4cplus::SharedAppenderPtr appender(new log4cplus::RollingFileAppender(appenderProperties));

					// Create a PatternLayout
					std::unique_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(LOG4CPLUS_TEXT("[%d{%Y-%m-%d %H:%M:%S.%q}] [%p] %m%n")));

					// Set layout for the appender
					appender->setLayout(std::move(layout));

					// Add the appender to the logger
					_log4cpp.addAppender(appender);

					// Set logger level (optional)
					_log4cpp.setLogLevel(log4cplus::ALL_LOG_LEVEL);

					// Log a message to verify logging is working
					LOG4CPLUS_INFO(_log4cpp, "Carousel logger initialized successfully.");

					// Logger has been initialized
					_isInitialized = true;
				}
				catch (const std::exception& e)
				{
					// Handle other exceptions
					std::cerr << "Logger could not be initialized. Exception caught: " << e.what() << std::endl;
				}
			}

		public:

			/// <summary>
			/// Copy constructor (removed)
			/// </summary>
			CarouselLogger(const CarouselLogger&) = delete;

			/// <summary>
			/// Assignment operator (removed)
			/// </summary>
			void operator=(const CarouselLogger&) = delete;

			/// <summary>
			/// Gets the logger object
			/// </summary>
			static CarouselLogger& instance()
			{
				static CarouselLogger _logger;
				return _logger;
			}

		public:

			/// <summary>
			/// Log info
			/// </summary>
			/// <param name="message">Info message</param>
			void Info(const std::string& message)
			{
				if (_isInitialized)
				{
					std::lock_guard<std::mutex> guard(_logMutex);
					LOG4CPLUS_INFO(_log4cpp, message.c_str());
				}
			}

			/// <summary>
			/// Log debug
			/// </summary>
			/// <param name="message">Debug message</param>
			void debug(const std::string& message)
			{
				if (_isInitialized)
				{
					std::lock_guard<std::mutex> guard(_logMutex);
					LOG4CPLUS_DEBUG(_log4cpp, message.c_str());
				}
			}

			/// <summary>
			/// Log error
			/// </summary>
			/// <param name="message">Error message</param>
			void error(const std::string& message)
			{
				if (_isInitialized)
				{
					std::lock_guard<std::mutex> guard(_logMutex);
					LOG4CPLUS_ERROR(_log4cpp, message.c_str());
				}
			}

			/// <summary>
			/// Log warning
			/// </summary>
			/// <param name="message">Warning message</param>
			void warning(const std::string& message)
			{
				if (_isInitialized)
				{
					std::lock_guard<std::mutex> guard(_logMutex);
					LOG4CPLUS_WARN(_log4cpp, message.c_str());
				}
			}
		};
	}
}