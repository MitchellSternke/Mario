#include "Ladder.hpp"

Ladder::Ladder() :
	Tile(TILE_NONE)
{
}

void Ladder::onInit()
{
	setAnimation("ladder");
}
