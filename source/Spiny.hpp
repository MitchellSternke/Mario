#ifndef SPINY_HPP
#define SPINY_HPP

#include "Enemy.hpp"

/**
 * A spiky enemy that cannot be jumped on.
 */
class Spiny : public Enemy
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all spinies. */
	static constexpr double HEIGHT = 0.75; /**< The height of all spinies. */

	static constexpr double MOVEMENT_SPEED = 1.5; /**< The speed that spinies move at. */

	Spiny();
};

#endif // SPINY_HPP
