#include "Item.hpp"
#include "Globals.hpp"

Item::Item( int pointValue ) :
	pointValue(pointValue)
{
	setWidth(WIDTH);
	setHeight(HEIGHT);
}

int Item::getPointValue() const
{
	return pointValue;
}

void Item::onCollision(Tile& tile, Edge edge)
{
	bounceOffWalls(tile, edge);
}
