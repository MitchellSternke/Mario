#include "IniFile.hpp"

bool IniFile::getValue(const std::string& key, std::string& value) const
{
	return getValueFromSection( rootSection, key, value );
}

bool IniFile::getValue(const std::string& section, const std::string& key, std::string& value) const
{
	auto it = sections.find(section);
	if( it == sections.end() )
	{
		return false;
	}

	return getValueFromSection((*it).second, key, value);
}

bool IniFile::getValueFromSection(const Section& section, const std::string& key, std::string& value) const
{
	auto it = section.pairs.find(key);
	if( it == section.pairs.end() )
	{
		return false;
	}

	value = (*it).second;
	return true;
}

void IniFile::load(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());

	// Line-by-line loading
	std::string section = "";
	while( !file.eof() )
	{
		std::string line;
		std::getline( file, line );

		if( line.empty() )
		{
			continue;
		}
		else if( line[0] == ';' )
		{
			// Comment
			continue;
		}
		else if( line[0] == '[' )
		{
			//Section
			std::size_t pos = line.find_first_of(']');
			if( pos == std::string::npos )
			{
				section = line.substr(1);
			}
			else
			{
				section = line.substr(1, pos - 1);
			}
			continue;
		}

		//Key value pair
		std::size_t pos = line.find_first_of('=');
		if( pos != std::string::npos )
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			if( section.empty() )
			{
				setValue(key, value);
			}
			else
			{
				setValue(section, key, value);
			}
		}
	}
}

void IniFile::save(const std::string& fileName) const
{
	std::ofstream file(fileName.c_str());

	// Save the root section first
	saveSection(file, rootSection);

	// Save other sections
	for( auto section : sections )
	{
		file << '[' << section.first << ']' << std::endl;
		saveSection(file, section.second);
	}
}

void IniFile::saveSection( std::ofstream& file, const Section& section ) const
{
	for( auto keyValuePair : section.pairs )
	{
		file << keyValuePair.first << '=' << keyValuePair.second << std::endl;
	}
	file << std::endl;
}

void IniFile::setValue(const std::string& key, const std::string& value)
{
	rootSection.pairs[key] = value;
}

void IniFile::setValue(const std::string& section, const std::string& key, const std::string& value)
{
	(sections[section]).pairs[key] = value;
}
