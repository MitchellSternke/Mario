#ifndef DAMAGEBLOCK_HPP
#define DAMAGEBLOCK_HPP

#include "Tile.hpp"

/**
 * A block that causes damage to the Player on collision.
 */
class DamageBlock : public Tile
{
public:
	/**
	 * Create a new damage block.
	 *
	 * @param fatal whether a collision will cause fatal damage to the player.
	 */
	DamageBlock( bool fatal );

private:
	bool fatal;

	void onCollision( Sprite& sprite, Edge edge );
};

#endif // DAMAGEBLOCK_HPP
