#ifndef STAR_HPP
#define STAR_HPP

#include "Item.hpp"

/**
 * An invincibility star.
 */
class Star : public Item
{
public:
	static constexpr double GRAVITY = -16.0;       /**< The gravity of a star. */
	static constexpr double MOVEMENT_SPEED = 3.0; /**< The speed that a star moves at. */
	static constexpr double BOUNCE_SPEED = 10.0;  /**< The speed that a star bounces at when it hits the ground. */

	/**
	 * Create a new star.
	 */
	Star();

private:
	void onPostUpdate();
};

#endif // STAR_HPP
