#ifndef MUSHROOM_HPP
#define MUSHROOM_HPP

#include "Item.hpp"

/**
 * Types of mushroom items.
 */
enum MushroomType
{
	MUSHROOM_SUPER, /**< Makes the player super. */
	MUSHROOM_1UP    /**< Grants the player an extra life. */
};

/**
 * A mushroom item.
 */
class Mushroom : public Item
{
public:
	static constexpr double MOVEMENT_SPEED = 3.0; /**< The movement speed of all mushrooms. */

	/**
	 * Create a new mushroom.
	 */
	Mushroom( MushroomType type = MUSHROOM_SUPER );

	/**
	 * Get the type of mushroom.
	 */
	MushroomType getType() const;

private:
	MushroomType type;
};

#endif // MUSHROOM_HPP
