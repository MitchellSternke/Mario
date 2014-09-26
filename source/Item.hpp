#ifndef ITEM_HPP
#define ITEM_HPP

#include "Sprite.hpp"

/**
 * An item/power-up usable by the Player.
 */
class Item : public Sprite
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all items. */
	static constexpr double HEIGHT = 0.75; /**< The height of all items. */

	static constexpr int POWERUP_POINT_VALUE = 1000;  /**< The default point value for powerups. */

	/**
	 * Get the point value of the Item when it is collected.
	 */
	int getPointValue() const;

protected:
	Item( int pointValue = POWERUP_POINT_VALUE );

	virtual void onCollision(Tile& tile, Edge edge);

private:
	int pointValue;
};

#endif // ITEM_HPP
