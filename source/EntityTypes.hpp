/**
 * @file
 * @brief defines Entity type enumerations and functions for conversion to them.
 */
#ifndef ENTITYTYPES_HPP
#define ENTITYTYPES_HPP

#include <string>

/**
 * The types of sprite objects that can be created.
 *
 * @note whenever a new enumeration constant is added, be sure to add it
 * to the string switch in EntityTypes.cpp. Also, add the
 * entity to the createSprite() function in the Level class.
 * Also, add it to the ignore list in Level::Level() if appropriate.
 */
enum SpriteType
{
	TYPE_SPRITE_NULL,

	TYPE_BEETLE,
	TYPE_COIN,
	TYPE_FLOWER,
	TYPE_GOOMBA,
	TYPE_GROWING_LADDER,
	TYPE_HAMMER_BRO,
	TYPE_KOOPA,
	TYPE_LAKITU,
	TYPE_LEAF,
	TYPE_LEVEL_END,
	TYPE_MUSHROOM,
	TYPE_PARATROOPA,
	TYPE_PLANT,
	TYPE_SHELL,
	TYPE_SPINY,
	TYPE_STAR,

	NUM_SPRITE_TYPES
};

/**
 * The types of tile objects that can be created.
 *
 * @note whenever a new enumeration constant is added, be sure to add it
 * to the string switch in EntityTypes.cpp. Also, add the
 * entity to the createTile() function in the Level class.
 * * Also, add it to the ignore list in Level::Level() if appropriate.
 */
enum TileType
{
	TYPE_TILE_NULL,

	TYPE_BLASTER,
	TYPE_BLOCK,
	TYPE_BRICK,
	TYPE_DAMAGE_BLOCK,
	TYPE_DAMAGE_BLOCK_FATAL,
	TYPE_GROUND,
	TYPE_LADDER,
	TYPE_LEDGE,
	TYPE_PIPE_DOWN,
	TYPE_PIPE_LEFT,
	TYPE_PIPE_RIGHT,
	TYPE_PIPE_UP,
	TYPE_SLOPE_DOWN,
	TYPE_SLOPE_UP,
	TYPE_QUESTION_BLOCK,
	TYPE_WATER,

	NUM_TILE_TYPES
};

SpriteType getSpriteType( const std::string& type );

TileType getTileType( const std::string& type );

#endif // ENTITYTYPES_HPP
