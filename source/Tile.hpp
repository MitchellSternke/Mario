#ifndef TILE_HPP
#define TILE_HPP

#include <string>

#include "Entity.hpp"
#include "Enums.hpp"

class Sprite;

/**
 * Tile collision behavior.
 */
enum TileCollisionBehavior
{
	TILE_NONE,       /**< Does not cause collisions. */

	TILE_PLATFORM,   /**< One-way vertical platform. Collisions only happen from above. */
	TILE_SLOPE_DOWN, /**< Downward slope. */
	TILE_SLOPE_UP,   /**< Upward slope. */
	TILE_SOLID       /**< Solid on all sides. */
};

/**
 * Entities that don't move.
 */
class Tile : public Entity
{
	friend class World;
public:
	/**
	 * Create a new tile.
	 *
	 * @param type the collision behavior of the Tile.
	 * @param tilesetEnabled whether the Tile should be rendered based on surrounding tiles.
	 */
	Tile(TileCollisionBehavior type, bool tilesetEnabled = false);

	/**
	 * Get the collision type of the tile.
	 */
	TileCollisionBehavior getCollisionBehavior() const;

	/**
	 * Get the edge state of a tile.
	 *
	 * @param edge the edge to get the state of.
	 * @return true if the edge is solid.
	 */
	bool getEdgeState(Edge edge) const;

	double getHeight() const;

	/**
	 * Get the height as an integer.
	 */
	int getHeightInt() const;

	double getWidth() const;

	/**
	 * Get the tileset animation for a unit tile of the Tile.
	 *
	 * @param x the x offset.
	 * @param y the y offset.
	 */
	const Animation* getTilesetAnimation( int x, int y ) const;

	/**
	 * Get the width as an integer.
	 */
	int getWidthInt() const;

	double getX() const;

	/**
	 * Get the x coordinate as an integer.
	 */
	int getXInt() const;

	double getY() const;

	/**
	 * Get the y coordinate as an integer.
	 */
	int getYInt() const;

	/**
	 * Check if the tile behaves as a slope.
	 */
	bool isSlope() const;

	/**
	 * Set the height of the tile.
	 */
	void setHeight(int height);

	void setLayer( int layer );

	/**
	 * Set the width of the tile.
	 */
	void setWidth(int width);

protected:
	virtual void onCollision(Sprite& sprite, Edge edge) {}
	virtual void onInit() {}

private:
	int x;
	int y;
	int width;
	int height;
	TileCollisionBehavior collisionBehavior;
	bool tilesetEnabled;
};

#endif // TILE_HPP
