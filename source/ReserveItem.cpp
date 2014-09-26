#include "ReserveItem.hpp"

ReserveItem::ReserveItem( Item* item ) :
	item(item)
{
	setGravityEnabled(false);
	setTileCollisionsEnabled(false);
	setYVelocity(RESERVE_Y_VELOCITY);
	setAlpha(0.5f);
	setAnimation( item->getActiveAnimation() );
}

ReserveItem::~ReserveItem()
{
	delete item;
}

Item* ReserveItem::getItem()
{
	return item;
}
