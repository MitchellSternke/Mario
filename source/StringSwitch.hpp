#ifndef STRINGSWITCH_HPP
#define STRINGSWITCH_HPP

#include <map>
#include <string>

/**
* A class that can be used to create switch statements from strings.
*/
class StringSwitch
{
public:
	StringSwitch();

	/**
	* Add a string to the switch.
	*
	* @param str the string.
	* @param value a value to assign to the string.
	*/
	void addString(const std::string& str, int value);

	/**
	* Get the value that will be returned from a test on an
	* unrecognized string.
	*
	* @return the default value.
	*/
	int getDefaultValue() const;

	/**
	* Set the default value when an unrecognized string is tested.
	*
	* @param value the default value to use.
	*/
	void setDefaultValue(int value);

	/**
	* Test the switch given a string.
	*
	* @param str the string to test.
	* @return the integer index assigned to the string.
	*/
	int test(const std::string& str) const;

private:
	int defaultValue;
	std::map<std::string, int> stringMap;
};

#endif // STRINGSWITCH_HPP
