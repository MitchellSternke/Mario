#ifndef FIREBALL_HPP
#define FIREBALL_HPP

#include "Sprite.hpp"

class Player;

/**
 * A fireball thrown by the Player.
 */
class Fireball : public Sprite
{
public:
	static constexpr double BOUNCE_VELOCITY = 16.0;  /**< The bounce velocity of a Fireball. */
	static constexpr double FIREBALL_SIZE = 0.5;     /**< The size of a Fireball. */
	static constexpr double X_VELOCITY = 12.0;       /**< The x velocity of a Fireball. */
	static constexpr double FIREBALL_DEATH_DISTANCE = 32.0; /**< The maximum distance a fireball can be from the player before it disintegrates. */

	/**
	 * Create a new fireball.
	 *
	 * @param player the Player that threw the fireball.
	 * @param xVelocity the starting x velocity of the fireball.
	 */
	Fireball(Player* player, double xVelocity);

private:
	Player* player;

	void onCollision(Sprite& sprite, Edge edge);
	void onCollision(Tile& tile, Edge edge);
	void onDestroy();
	void onPostUpdate();
	void onPreUpdate();
};

#endif // FIREBALL_HPP
