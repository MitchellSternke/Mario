#ifndef LADDER_HPP
#define LADDER_HPP

#include "Tile.hpp"

/**
 * A climbable tile.
 */
class Ladder : public Tile
{
public:
	Ladder();

private:
	void onInit();
};

#endif // LADDER_HPP
