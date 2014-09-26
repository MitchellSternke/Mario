#include "Globals.hpp"
#include "LevelTheme.hpp"

void LevelTheme::addEntityType( const std::string& name, const ResourceManager* resourceGroup )
{
	// Is is a sprite?
	SpriteType spriteType = getSpriteType(name);
	if( spriteType != TYPE_SPRITE_NULL )
	{
		spriteTypes[spriteType].push_back(resourceGroup);
		return;
	}

	// Is it a tile?
	TileType tileType = getTileType(name);
	if( tileType != TYPE_TILE_NULL )
	{
		tileTypes[tileType].push_back(resourceGroup);
		return;
	}

	// We didn't recognize the type
	LOG << "Warning: attempted to add an entity type to an LevelTheme with invalid name \"" << name << "\".\n";
}

void LevelTheme::inherit( const LevelTheme& parent )
{
	for( int i = 0; i < NUM_SPRITE_TYPES; i++ )
	{
		if( spriteTypes[i].empty() )
		{
			spriteTypes[i] = parent.spriteTypes[i];
		}
	}
	for( int i = 0; i < NUM_TILE_TYPES; i++ )
	{
		if( tileTypes[i].empty() )
		{
			tileTypes[i] = parent.tileTypes[i];
		}
	}
}
