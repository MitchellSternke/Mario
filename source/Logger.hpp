#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>

#define LOG_FILE_NAME "log.txt"

/**
 * Logs things to stdout and a text file.
 */
class Logger
{
public:
	/**
	 * Interface for objects that can receive real-time logging text.
	 */
	class Listener
	{
	public:
		virtual void onLog( const std::string& text )=0;
	};

	Logger();
	~Logger();

	/**
	 * Add a Listener to the Logger.
	 */
	void addListener( Listener& listener );

	/**
	 * Remove a Listener from the Logger.
	 */
	void removeListener( Listener& listener );

	/**
	 * Generic stream operator.
	 */
	template <typename T>
	Logger& operator << (T value)
	{
		std::cout << value;
		file << value;

		std::stringstream str;
		str << value;
		notifyListeners(str.str());

		return *this;
	}

	/**
	 * Overload of the stream operator to allow the use of std::endl.
	 */
	Logger& operator << (std::ostream& (*functionPointer)(std::ostream&))
	{
		functionPointer(std::cout);
		functionPointer(file);

		std::stringstream str;
		functionPointer(str);
		notifyListeners(str.str());

		return *this;
	}

private:
	std::ofstream file;
	std::list<Listener*> listeners;

	void notifyListeners( const std::string& text );
};

#endif // LOGGER_HPP
