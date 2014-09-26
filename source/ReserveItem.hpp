#ifndef RESERVEITEM_HPP
#define RESERVEITEM_HPP

#include "Item.hpp"

/**
 * A reserve item dropped from the box on the top of the screen.
 */
class ReserveItem : public Item
{
public:
	static constexpr double RESERVE_Y_VELOCITY = -2.0; /**< The y velocity of a reserve item when it is dropped. */

	/**
	 * Create a new reserve item.
	 *
	 * @param item the type of reserve item.
	 */
	ReserveItem( Item* item );

	~ReserveItem();

	/**
	 * Get the Item stored by the reserve item.
	 */
	Item* getItem();

private:
	Item* item;
};

#endif // RESERVEITEM_HPP
