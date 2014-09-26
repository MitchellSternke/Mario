#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <fstream>
#include <map>
#include <string>

/**
 * An INI file parser.
 */
class IniFile
{
public:
	/**
	 * Get a value from the INI file.
	 *
	 * @param key the name of the key.
	 * @param value a reference to a string that will store the value.
	 * @return true if the key-value pair exists.
	 */
	bool getValue(const std::string& key, std::string& value) const;

	/**
	 * Get a value from the INI file.
	 *
	 * @param section the name of the section.
	 * @param key the name of the key.
	 * @param value a reference to a string that will store the value.
	 * @return true if the key-value pair exists.
	 */
	bool getValue(const std::string& section, const std::string& key, std::string& value) const;

	/**
	 * Load an INI file and parse it.
	 *
	 * @param fileName the name of the file to load from.
	 */
	void load( const std::string& fileName );

	/**
	 * Save key-value pairs to a file.
	 *
	 * @param fileName the name of the file to save to.
	 */
	void save( const std::string& fileName ) const;

	/**
	 * Set the value of a key-value pair.
	 *
	 * @param key a string representing the key.
	 * @param value a string representing the value.
	 */
	void setValue(const std::string& key, const std::string& value);

	/**
	 * Set the value of a key-value pair.
	 *
	 * @param section the section of the INI file.
	 * @param key the name of the key.
	 * @param value the name of the value.
	 */
	void setValue(const std::string& section, const std::string& key, const std::string& value);

private:
	struct Section
	{
		std::map<std::string, std::string> pairs;
	};

	std::map<std::string, Section> sections;
	Section rootSection;

	bool getValueFromSection( const Section& section, const std::string& key, std::string& value ) const;
	void saveSection( std::ofstream& file, const Section& section ) const;
};

#endif // INIFILE_HPP
