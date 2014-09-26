#ifndef KOOPA_HPP
#define KOOPA_HPP

#include "Enemy.hpp"

/**
 * A turtle-like enemy with a kickable shell.
 */
class Koopa : public Enemy
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all koopas. */
	static constexpr double HEIGHT = 0.75; /**< The height of all koopas. */

	static constexpr double MOVEMENT_SPEED = 1.5; /**< The speed that koopas move at. */

	Koopa();

private:
	void onBump();
	void onStomp();
};

#endif // KOOPA_HPP
