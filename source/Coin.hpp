#ifndef COIN_HPP
#define COIN_HPP

#include "Item.hpp"

/**
 * A collectible coin.
 */
class Coin : public Item
{
public:
	/**
	 * The point value of all coins.
	 */
	static constexpr int POINT_VALUE = 50;

	static constexpr double COIN_WIDTH  = 1.0; /**< The width of all coins. */
	static constexpr double COIN_HEIGHT = 1.0; /**< The height of all coins. */

	static constexpr double COIN_SPARKLE_DELAY = 0.1; /**< The delay, in seconds, between coin sparkle particles. */

	/**
	 * Create a new Coin.
	 */
	Coin();

	/**
	 * Create sparkle effects around the coin.
	 */
	void sparkle();
};

#endif // COIN_HPP
