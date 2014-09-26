#ifndef BEETLE_HPP
#define BEETLE_HPP

#include "Enemy.hpp"

/**
 * A fireproof version of the Koopa.
 */
class Beetle : public Enemy
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all beetles. */
	static constexpr double HEIGHT = 0.75; /**< The height of all beetles. */

	static constexpr double MOVEMENT_SPEED = 1.5; /**< The speed that beetles move at. */

	Beetle();

private:
	void onBump();
	void onStomp();
};

#endif // BEETLE_HPP
