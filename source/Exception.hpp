#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <sstream>
#include <string>

/**
* A custom bare-bones exception class.
*/
class Exception : public std::exception
{
public:
	Exception() throw()
	{
	}

	Exception(const std::string& message) throw()
	{
		this->message = message;
	}

	~Exception() throw() {}

	/**
	 * Generic stream operator.
	 */
	template <typename T>
	Exception& operator << (T value) throw()
	{
		std::stringstream stream;
		stream << this->message << value;
		this->message = stream.str();
		return *this;
	}

	const char* what() const throw()
	{
		return this->message.c_str();
	}

private:
	std::string message;
};

#endif // EXCEPTION_HPP
