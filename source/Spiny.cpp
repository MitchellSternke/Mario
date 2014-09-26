#include "Spiny.hpp"

Spiny::Spiny()
{
	setStompable(false);
	setFireproof(true);

	setWidth(WIDTH);
	setHeight(HEIGHT);

	setDefaultXVelocity( -1.0 * MOVEMENT_SPEED );

	setAnimation("spiny");
}
