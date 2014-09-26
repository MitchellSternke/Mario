#ifndef HAMMERBRO_HPP
#define HAMMERBRO_HPP

#include "Enemy.hpp"

/**
 * An enemy that jumps and throws hammers at the player.
 */
class HammerBro : public Enemy
{
public:
	static constexpr double WIDTH = 0.75; /**< The width of all hammer bros. */
	static constexpr double HEIGHT = 1.0; /**< The height of all hammer bros. */
	static constexpr double MOVEMENT_SPEED = 1.5; /**< The speed that all hammer bros move at. */
	static constexpr double JUMP_VELOCITY = 12.0; /**< The velocity of hammer bro jumps. */
	static constexpr double GRAVITY = -16.0; /**< The gravity experienced by hammer bros. */

	static constexpr int JUMP_WAIT_TIME = 300; /**< The time in frames that a hammer bro waits between jumps. */
	static constexpr int MOVE_DURATION = 60; /**< The time in frames that a hammer bro spends moving in one direction before turning around. */

	HammerBro();

private:
	int jumpTimer;
	int moveTimer;

	void onPreUpdate();
};

#endif // HAMMERBRO_HPP
