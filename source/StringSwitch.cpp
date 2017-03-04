// @richelbilderbeek: corrected case
#include "StringSwitch.hpp"

using std::map;
using std::string;

StringSwitch::StringSwitch()
{
	this->defaultValue = -1;
}

void StringSwitch::addString(const string& str, int value)
{
	this->stringMap[str] = value;
}

int StringSwitch::getDefaultValue() const
{
	return defaultValue;
}

void StringSwitch::setDefaultValue(int value)
{
	this->defaultValue = value;
}

int StringSwitch::test(const string& str) const
{
	int returnVal = defaultValue;

	map<string, int>::const_iterator it;
	it = this->stringMap.find(str);
	if( it != this->stringMap.end() )
	{
		returnVal = (*it).second;
	}

	return returnVal;
}
