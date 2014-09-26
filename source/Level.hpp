#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <map>
#include <set>
#include <vector>

#include "EntityTypes.hpp"
#include "Location.hpp"

class Background;
class LevelTheme;
class Music;
class Random;
class ResourceManager;
class Sprite;
class Tile;

static const int INFINITE_LEVEL_TIME = -1; /**< Time used to indicate that the player has infinite time to complete a level. */
static const int DEFAULT_LEVEL_TIME = 400; /**< Default time that the player is given to complete a level. */

/**
 * An individual Level of the game.
 */
class Level
{
	friend class World;
public:
	/**
	 * Create an empty level.
	 *
	 * @param width the width of the level.
	 * @param height the height of the level.
	 */
	Level(int width, int height );

	/**
	 * Add a sprite to the level.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param sprite the sprite.
	 * @return the id of the sprite added.
	 */
	int addSprite( double x, double y, SpriteType sprite );

	/**
	 * Add a sprite to the level.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param sprite the sprite.
	 * @param resourceGroup the ResourceManager for the sprite to use.
	 * @return the id of the sprite added.
	 */
	int addSprite( double x, double y, SpriteType sprite, const ResourceManager* resourceGroup );

	/**
	 * Add a tile to the level.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param tile the tile.
	 * @param width the width of the tile.
	 * @param height the height of the tile.
	 * @return the id of the tile added.
	 */
	int addTile( int x, int y, TileType tile, int width = 1, int height = 1 );

	/**
	 * Add a tile to the level.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param tile the tile.
	 * @param resourceGroup the ResourceManager for the tile to use.
	 * @param width the width of the tile.
	 * @param height the height of the tile.
	 * @return the id of the tile added.
	 */
	int addTile( int x, int y, TileType tile, const ResourceManager* resourceGroup, int width = 1, int height = 1 );

	/**
	 * Instantiate the sprite with a given id.
	 */
	Sprite* createSprite( int id ) const;

	/**
	 * Instantiate the tile with a given id.
	 */
	Tile* createTile( int id ) const;

	/**
	 * Check if a theme can be applied to every entity in the level.
	 */
	bool isThemeCompatible( const LevelTheme& theme ) const;

	/**
	 * Set the background image for the Level.
	 '*/
	void setBackground( const Background* background );

	/**
	 * Set the music used by the Level.
	 */
	void setMusic( const Music* music );

	/**
	 * Set data used to create a sprite.
	 *
	 * @param spriteId the id returned from addSprite().
	 * @param key the key for the data.
	 * @param value the new value for the data.
	 */
	void setSpriteData( int spriteId, const std::string& key, int value );

	/**
	 * Set data used to create a sprite.
	 *
	 * @param spriteId the id returned from addSprite().
	 * @param key the key for the data.
	 * @param value the new value for the data.
	 */
	void setSpriteData( int spriteId, const std::string& key, const std::string& value );

	/**
	 * Set the theme used by the Level.
	 *
	 * @param theme the LevelTheme to use.
	 * @param random the source of randomness for picking themed items.
	 */
	void setTheme( const LevelTheme& theme, Random& random );

	/**
	 * Set data used to create a tile.
	 *
	 * @param tileId the id returned from addTile().
	 * @param key the key for the data.
	 * @param value the new value for the data.
	 */
	void setTileData( int tileId, const std::string& key, int value );

	/**
	 * Set data used to create a tile.
	 *
	 * @param tileId the id returned from addTile().
	 * @param key the key for the data.
	 * @param value the new value for the data.
	 */
	void setTileData( int tileId, const std::string& key, const std::string& value );

	/**
	 * Set the time limit for the Level.
	 *
	 * @param timeLimit the time limit, in seconds.
	 */
	void setTimeLimit( int timeLimit );

	/**
	 * Set whether there is water on the tile at (x, y).
	 */
	void setWater( int x, int y, bool water );

private:
	struct SpriteInfo
	{
		double x;
		double y;
		SpriteType sprite;
		const ResourceManager* resourceGroup;
		std::map< std::string, std::string > data;
	};

	struct TileInfo
	{
		int x;
		int y;
		int width;
		int height;
		TileType tile;
		const ResourceManager* resourceGroup;
		std::map< std::string, std::string > data;
	};

	std::vector<SpriteInfo> sprites;
	std::vector<TileInfo> tiles;
	std::vector<bool> water;
	int width;
	int height;
	int time;
	const Background* background;
	const Music* music;

	// These are used for theming (should be a static table instead at some point)
	std::set<SpriteType> ignoreSprites;
	std::set<TileType> ignoreTiles;

	const std::string* getSpriteData( int spriteId, const std::string& key ) const;
	const std::string* getTileData( int tileId, const std::string& key ) const;
};

#endif // LEVEL_HPP
