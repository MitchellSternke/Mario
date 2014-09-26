#ifndef MUSHROOM_HPP
#define MUSHROOM_HPP

#include "Item.hpp"

/**
 * A super mushroom.
 */
class Mushroom : public Item
{
public:
	static constexpr double MOVEMENT_SPEED = 3.0; /**< The movement speed of all super mushrooms. */

	/**
	 * Create a new super mushroom.
	 */
	Mushroom();
};

#endif // MUSHROOM_HPP
