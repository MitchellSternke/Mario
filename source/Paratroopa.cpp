#include "Koopa.hpp"
#include "Paratroopa.hpp"
#include "World.hpp"

Paratroopa::Paratroopa()
{
	setDefaultXVelocity(-1.0 * MOVEMENT_SPEED);
	setWidth(WIDTH);
	setHeight(HEIGHT);
	setGravityEnabled(false);
	setYAcceleration(GRAVITY);

	setAnimation("paratroopa");
}

void Paratroopa::onBump()
{
	Koopa* k = new Koopa;
	k->setX(getX());
	k->setY(getY());
	k->setYVelocity(BUMP_VELOCITY);
	getWorld().addSprite(k);
	kill();
}

void Paratroopa::onPostUpdate()
{
	if( !isFlying() )
	{
		setYVelocity(BOUNCE_SPEED);
	}
}

void Paratroopa::onStomp()
{
	Koopa* k = new Koopa;
	k->setX(getX());
	k->setY(getY());

	getWorld().addSprite(k);

	kill();
}
