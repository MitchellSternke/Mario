#include "GrowingLadder.hpp"
#include "Ladder.hpp"
#include "Tile.hpp"
#include "World.hpp"

GrowingLadder::GrowingLadder()
{
	setGravityEnabled(false);
	setSpriteCollisionsEnabled(false);
	setAnimation("growing_ladder");
	setYVelocity(GROWING_LADDER_SPEED);
}

void GrowingLadder::grow( double yPosition )
{
	// Get the tile at the bottom of us
	int x = static_cast<int>(std::floor(getCenterX()));
	int y = static_cast<int>(std::floor(yPosition));

	if( y >= getWorld().getHeight() )
	{
		kill();
		return;
	}

	const Tile* tile = getWorld().getTile(x, y);

	if( tile == nullptr )
	{
		getWorld().setTile(x, y, new Ladder());
	}
}

void GrowingLadder::onCollision( Tile& tile, Edge edge )
{
	// If we collide with a tile, it's time to stop on the next frame...
	if( tile.getCollisionBehavior() != TILE_NONE )
	{
		grow(getCenterY());
		kill();
	}
}

void GrowingLadder::onPreUpdate()
{
	grow(getBottom());
}
