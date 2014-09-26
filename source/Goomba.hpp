#ifndef GOOMBA_HPP
#define GOOMBA_HPP

#include "Enemy.hpp"

/**
 * A simple enemy.
 */
class Goomba : public Enemy
{
public:
	static constexpr double DEAD_PARTICLE_DURATION = 0.5; /**< The duration of the particle spawned when a goomba is stomped. */
	static constexpr double MOVEMENT_SPEED = 1.5;         /**< The movement speed of all goombas. */
	static constexpr double WIDTH = 0.75;                 /**< The width of all goombas. */
	static constexpr double HEIGHT = 0.75;                /**< The height of all goombas. */

	Goomba();

private:
	void onStomp();
};

#endif // GOOMBA_HPP
