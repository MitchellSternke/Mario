#ifndef GROWINGLADDER_HPP
#define GROWINGLADDER_HPP

#include "Sprite.hpp"

/**
 * A sprite that grows a ladder upward until it hits a block or the bounds
 * of the world.
 */
class GrowingLadder : public Sprite
{
public:
	static constexpr double GROWING_LADDER_SPEED = 4.0;

	GrowingLadder();

private:
	void grow( double yPosition );
	void onCollision( Tile& tile, Edge edge );
	void onPreUpdate();
};

#endif // GROWINGLADDER_HPP
