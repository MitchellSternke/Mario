#include <cassert>

#include "Level.hpp"
#include "LevelTheme.hpp"
#include "Random.hpp"

// Files included for the create methods:
#include "Beetle.hpp"
#include "Blaster.hpp"
#include "Block.hpp"
#include "Coin.hpp"
#include "Flower.hpp"
#include "Globals.hpp"
#include "Goomba.hpp"
#include "GrowingLadder.hpp"
#include "HammerBro.hpp"
#include "Koopa.hpp"
#include "Ladder.hpp"
#include "Lakitu.hpp"
#include "Leaf.hpp"
#include "LevelEnd.hpp"
#include "Mushroom.hpp"
#include "Paratroopa.hpp"
#include "Pipe.hpp"
#include "Plant.hpp"
#include "Shell.hpp"
#include "Spiny.hpp"
#include "Star.hpp"
#include "Tile.hpp"

Level::Level(int width, int height) :
	width(width),
	height(height),
	time(DEFAULT_LEVEL_TIME),
	background(nullptr),
	music(nullptr)
{
	assert(width > 0);
	assert(height > 0);
	water.resize(width * height, false);

	ignoreSprites.insert(TYPE_BEETLE);
	ignoreSprites.insert(TYPE_COIN);
	ignoreSprites.insert(TYPE_FLOWER);
	ignoreSprites.insert(TYPE_GOOMBA);
	ignoreSprites.insert(TYPE_GROWING_LADDER);
	ignoreSprites.insert(TYPE_HAMMER_BRO);
	ignoreSprites.insert(TYPE_KOOPA);
	ignoreSprites.insert(TYPE_LAKITU);
	ignoreSprites.insert(TYPE_LEAF);
	ignoreSprites.insert(TYPE_LEVEL_END);
	ignoreSprites.insert(TYPE_MUSHROOM);
	ignoreSprites.insert(TYPE_PARATROOPA);
	ignoreSprites.insert(TYPE_PLANT);
	ignoreSprites.insert(TYPE_SHELL);
	ignoreSprites.insert(TYPE_SPINY);
	ignoreSprites.insert(TYPE_STAR);

	ignoreTiles.insert(TYPE_BLASTER);
	ignoreTiles.insert(TYPE_BRICK);
	ignoreTiles.insert(TYPE_LADDER);
	ignoreTiles.insert(TYPE_QUESTION_BLOCK);
	ignoreTiles.insert(TYPE_WATER);
}

int Level::addSprite(double x, double y, SpriteType sprite)
{
	return addSprite(x, y, sprite, nullptr);
}

int Level::addSprite(double x, double y, SpriteType sprite, const ResourceManager* resourceGroup)
{
	SpriteInfo info;
	info.x = x;
	info.y = y;
	info.sprite = sprite;
	info.resourceGroup = resourceGroup;

	int id = static_cast<int>(sprites.size());
	sprites.push_back(info);

	return id;
}

int Level::addTile( int x, int y, TileType tile, int width, int height )
{
	return addTile(x, y, tile, nullptr, width, height);
}

int Level::addTile(int x, int y, TileType tile, const ResourceManager* resourceGroup, int width, int height)
{
	TileInfo info;
	info.x = x;
	info.y = y;
	info.tile = tile;
	info.resourceGroup = resourceGroup;
	info.width = width;
	info.height = height;

	int id = static_cast<int>(tiles.size());
	tiles.push_back(info);

	// Also, add water status to the water array
	if( tile == TYPE_WATER )
	{
		for( int a = x; a < x + width; a++ )
		{
			for( int b = y; b < y + height; b++ )
			{
				setWater(a, b, true);
			}
		}
	}

	return id;
}

Sprite* Level::createSprite( int id ) const
{
	auto spriteInfo = sprites[id];
	Sprite* sprite = nullptr;
	switch( spriteInfo.sprite )
	{
	case TYPE_BEETLE:
		sprite = new Beetle;
		break;
	case TYPE_COIN:
		sprite = new Coin;
		break;
	case TYPE_FLOWER:
		sprite = new Flower;
		break;
	case TYPE_GOOMBA:
		sprite = new Goomba;
		break;
	case TYPE_GROWING_LADDER:
		sprite = new GrowingLadder;
		break;
	case TYPE_HAMMER_BRO:
		sprite = new HammerBro;
		break;
	case TYPE_KOOPA:
		sprite = new Koopa;
		break;
	case TYPE_LAKITU:
		sprite = new Lakitu;
		break;
	case TYPE_LEAF:
		sprite = new Leaf;
		break;
	case TYPE_LEVEL_END:
		sprite = new LevelEnd;
		break;
	case TYPE_MUSHROOM:
		sprite = new Mushroom;
		break;
	case TYPE_PARATROOPA:
		sprite = new Paratroopa;
		break;
	case TYPE_PLANT:
		{
			auto orientation = getSpriteData( id, "orientation" );
			if( orientation != nullptr )
			{
				sprite = new Plant( getDirection(*orientation) );
			}
			else
			{
				sprite = new Plant;
			}
		}
		break;
	case TYPE_SHELL:
		sprite = new Shell;
		break;
	case TYPE_SPINY:
		sprite = new Spiny;
		break;
	case TYPE_STAR:
		sprite = new Star;
		break;
	default:
		return nullptr;
	}

	if( spriteInfo.resourceGroup != nullptr )
	{
		sprite->setResourceManager( spriteInfo.resourceGroup );
	}

	return sprite;
}

Tile* Level::createTile( int id ) const
{
	auto tileInfo = tiles[id];
	Tile* tile = nullptr;
	switch( tileInfo.tile )
	{
	case TYPE_BLASTER:
		tile = new Blaster;
		break;
	case TYPE_BLOCK:
		tile = new Tile(TILE_SOLID, true);
		break;
	case TYPE_QUESTION_BLOCK:
	case TYPE_BRICK:
		{
			auto blockType = BLOCK_BRICK;
			if( tileInfo.tile == TYPE_QUESTION_BLOCK )
			{
				blockType = BLOCK_QUESTION;
			}
			auto coins = getTileData( id, "coins" );
			auto contents = getTileData( id, "contents" );
			auto scatterCoins = getTileData( id, "scatter_coins" );
			if( coins != nullptr )
			{
				bool scatter = false;
				if( scatterCoins != nullptr )
				{
					scatter = (std::stoi(*scatterCoins) != 0);
				}
				tile = new Block( blockType, std::stoi(*coins), scatter );
			}
			else if( contents != nullptr )
			{
				SpriteType sprite = getSpriteType(*contents);

				/// @todo SpriteType to Sprite* conversion function
				Sprite* s = nullptr;
				switch( sprite )
				{
				case TYPE_MUSHROOM:
					s = new Mushroom;
					break;
				case TYPE_FLOWER:
					tile = new Block( blockType, BLOCK_FLOWER );
					break;
				case TYPE_LEAF:
					tile = new Block( blockType, BLOCK_LEAF );
					break;
				case TYPE_STAR:
					s = new Star;
					break;
				case TYPE_GROWING_LADDER:
					s = new GrowingLadder;
					break;
				default:
					break;
				}

				if( s != nullptr )
				{
					tile = new Block( blockType , s );
				}
			}
			else
			{
				tile = new Block( blockType );
			}
		}
		break;
	case TYPE_GROUND:
		tile = new Tile(TILE_SOLID, true);
		break;
	case TYPE_LADDER:
		tile = new Ladder();
		break;
	case TYPE_LEDGE:
		tile = new Tile(TILE_PLATFORM, true);
		break;
	case TYPE_PIPE_DOWN:
	case TYPE_PIPE_LEFT:
	case TYPE_PIPE_RIGHT:
	case TYPE_PIPE_UP:
		{
			Direction direction;
			switch( tileInfo.tile )
			{
			case TYPE_PIPE_DOWN:
				direction = DOWN;
				break;
			case TYPE_PIPE_LEFT:
				direction = LEFT;
				break;
			case TYPE_PIPE_RIGHT:
				direction = RIGHT;
				break;
			case TYPE_PIPE_UP:
				direction = UP;
				break;
			default:
				break;
			}
			auto warpLevelId = getTileData( id, "warp_level_id" );
			auto warpX = getTileData( id, "warp_x" );
			auto warpY = getTileData( id, "warp_y" );
			if( warpLevelId != nullptr && warpX != nullptr && warpY != nullptr )
			{
				Location warpLocation;
				warpLocation.levelId = std::stoi(*warpLevelId);
				warpLocation.x = std::stoi(*warpX);
				warpLocation.y = std::stoi(*warpY);
				tile = new Pipe(direction, warpLocation);
			}
			else
			{
				tile = new Pipe(direction);
			}
		}
		break;

	case TYPE_SLOPE_DOWN:
		tile = new Tile(TILE_SLOPE_DOWN, true);
		break;
	case TYPE_SLOPE_UP:
		tile = new Tile(TILE_SLOPE_UP, true);
		break;


	case TYPE_WATER:
		tile = new Tile(TILE_NONE, true);
		break;
	default:
		return nullptr;
	}

	if( tileInfo.resourceGroup != nullptr )
	{
		tile->setResourceManager( tileInfo.resourceGroup );
	}
	tile->setWidth( tileInfo.width );
	tile->setHeight( tileInfo.height );

	return tile;
}

const std::string* Level::getSpriteData( int spriteId, const std::string& key ) const
{
	auto it = sprites[spriteId].data.find(key);
	if( it == sprites[spriteId].data.end() )
	{
		return nullptr;
	}
	return &((*it).second);
}

const std::string* Level::getTileData( int tileId, const std::string& key ) const
{
	auto it = tiles[tileId].data.find(key);
	if( it == tiles[tileId].data.end() )
	{
		return nullptr;
	}
	return &((*it).second);
}

bool Level::isThemeCompatible( const LevelTheme& theme ) const
{
	// Check that the theme works for sprites
	for( auto& sprite : sprites )
	{
        if( theme.spriteTypes[sprite.sprite].empty() && ignoreSprites.find(sprite.sprite) == ignoreSprites.end() )
		{
			return false;
		}
	}

	// Check that the theme works for tiles
	for( auto& tile : tiles )
	{
		if( theme.tileTypes[tile.tile].empty() && ignoreTiles.find(tile.tile) == ignoreTiles.end() )
		{
			return false;
		}
	}

	return true;
}

void Level::setBackground( const Background* background )
{
	this->background = background;
}

void Level::setMusic( const Music* music )
{
	this->music = music;
}

void Level::setSpriteData( int spriteId, const std::string& key, int value )
{
	///@todo error checking
	sprites[spriteId].data[key] = std::to_string(value);
}

void Level::setSpriteData( int spriteId, const std::string& key, const std::string& value )
{
	///@todo error checking
	sprites[spriteId].data[key] = value;
}

void Level::setTheme( const LevelTheme& theme, Random& random )
{
	// Set the background and the music
	setBackground( theme.backgrounds[random.nextInt(theme.backgrounds.size())] );
	setMusic( theme.musics[random.nextInt(theme.musics.size()) ] );

	// Set the theme for sprites
	for( auto& sprite : sprites )
	{
        if( !theme.spriteTypes[sprite.sprite].empty() )
		{
			sprite.resourceGroup = theme.spriteTypes[sprite.sprite][ random.nextInt(theme.spriteTypes[sprite.sprite].size()) ];
		}
	}

	// Set the theme for tiles
	for( auto& tile : tiles )
	{
		if( !theme.tileTypes[tile.tile].empty() )
		{
			tile.resourceGroup = theme.tileTypes[tile.tile][ random.nextInt(theme.tileTypes[tile.tile].size()) ];
		}
	}
}

void Level::setTileData( int tileId, const std::string& key, int value )
{
	///@todo error checking
	tiles[tileId].data[key] = std::to_string(value);
}

void Level::setTileData( int tileId, const std::string& key, const std::string& value )
{
	///@todo error checking
	tiles[tileId].data[key] = value;
}

void Level::setTimeLimit( int timeLimit )
{
	time = timeLimit;
}

void Level::setWater(int x, int y, bool water)
{
	this->water[y * width + x] = water;
}

