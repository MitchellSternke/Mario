#include "Beetle.hpp"
#include "Globals.hpp"
#include "Shell.hpp"
#include "World.hpp"

Beetle::Beetle()
{
	setDefaultXVelocity(-1.0 * MOVEMENT_SPEED);
	setWidth(WIDTH);
	setHeight(HEIGHT);
	setFireproof(true);

	setAnimation("beetle");
}

void Beetle::onBump()
{
	Shell* shell = new Shell(true);
	shell->setOrientation(DOWN);
	shell->setYVelocity(BUMP_VELOCITY);
	getWorld().addSprite(getX(), getY(), shell);
	kill();
}

void Beetle::onStomp()
{
	getWorld().addSprite(getX(), getY() + 0.1, new Shell(true));
	kill();
}
