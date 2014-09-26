#ifndef LEVELTHEME_HPP
#define LEVELTHEME_HPP

#include <string>
#include <vector>

#include "EntityTypes.hpp"

class Background;
class Music;
class ResourceManager;

/**
 * Determines the theming (resources used) when generating a random Level.
 */
struct LevelTheme
{
	std::vector<const Background*> backgrounds;
	std::vector<const Music*> musics;
	std::vector<const ResourceManager*> spriteTypes[NUM_SPRITE_TYPES];
	std::vector<const ResourceManager*> tileTypes[NUM_TILE_TYPES];

	/**
	 * Add an entity type to the theme to be randomly selected.
	 *
	 * @param name the name of the entity type.
	 * @param resourceGroup the ResourceManager for the entity to use.
	 */
	void addEntityType( const std::string& name, const ResourceManager* resourceGroup );

	/**
	 * Inherit the theming resource groups from a parent level theme. Any
	 * type that the parent has a resource specified for that this
	 * instance lacks will be copied over.
	 */
	void inherit( const LevelTheme& parent );
};

#endif // LEVELTHEME_HPP
