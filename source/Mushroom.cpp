#include "Mushroom.hpp"

Mushroom::Mushroom( MushroomType type ) :
	Item( type == MUSHROOM_SUPER ? POWERUP_POINT_VALUE : 0 ), // Only super mushrooms give the player points
	type(type)
{
	switch( type )
	{
		case MUSHROOM_1UP:
			setAnimation("mushroom_1up");
			break;
		case MUSHROOM_SUPER:
		default:
			setAnimation("mushroom");
			break;
	}
	setXVelocity(MOVEMENT_SPEED);
}

MushroomType Mushroom::getType() const
{
	return type;
}
