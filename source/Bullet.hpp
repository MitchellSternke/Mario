#ifndef BULLET_HPP
#define BULLET_HPP

#include "Enemy.hpp"

/**
 * A Bullet launched from a Blaster.
 */
class Bullet : public Enemy
{
public:
	static constexpr double BULLET_SPEED = 4.0; /**< The speed of a bullet. */
	static constexpr double BULLET_HEIGHT = 0.8; /**< The height of a bullet. */

	Bullet();
};

#endif // BULLET_HPP
