#include "Star.hpp"

Star::Star()
{
	setAnimation("star");
	setXVelocity(MOVEMENT_SPEED);
	setGravityEnabled(false);
	setYAcceleration(GRAVITY);
}

void Star::onPostUpdate()
{
	if( !isFlying() )
	{
		setYVelocity(BOUNCE_SPEED);
	}
}
