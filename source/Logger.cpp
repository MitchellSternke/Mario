#include "Logger.hpp"

Logger::Logger()
{
	file.open(LOG_FILE_NAME);
}

Logger::~Logger()
{
	file.close();
}

void Logger::addListener( Logger::Listener& listener )
{
	listeners.push_back(&listener);
}

void Logger::notifyListeners( const std::string& text )
{
	for( auto l : listeners )
	{
		l->onLog(text);
	}
}

void Logger::removeListener( Logger::Listener& listener )
{
	listeners.remove(&listener);
}
