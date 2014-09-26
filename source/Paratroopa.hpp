#ifndef PARATROOPA_HPP
#define PARATROOPA_HPP

#include "Enemy.hpp"

/**
 * A flying koopa.
 */
class Paratroopa : public Enemy
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all paratroopas. */
	static constexpr double HEIGHT = 0.8; /**< The height of all paratroopas. */

	static constexpr double MOVEMENT_SPEED = 1.5; /**< The speed that paratroopas move at. */

	static constexpr double BOUNCE_SPEED = 8.0; /**< The speed that paratroopas bounce when they hit the ground. */

	static constexpr double GRAVITY = -16.0; /**< The gravity of a paratroopa. */

	Paratroopa();

private:
	void onBump();
	void onPostUpdate();
	void onStomp();
};

#endif // PARATROOPA_HPP
