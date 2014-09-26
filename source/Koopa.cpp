#include "Globals.hpp"
#include "Koopa.hpp"
#include "Shell.hpp"
#include "World.hpp"

Koopa::Koopa()
{
	setDefaultXVelocity(-1.0 * MOVEMENT_SPEED);
	setWidth(WIDTH);
	setHeight(HEIGHT);

	setAnimation("koopa");
}

void Koopa::onBump()
{
	Shell* shell = new Shell;
	shell->setOrientation(DOWN);
	shell->setYVelocity(BUMP_VELOCITY);
	getWorld().addSprite(getX(), getY(), shell);
	kill();
}

void Koopa::onStomp()
{
	getWorld().addSprite(getX(), getY() + 0.1, new Shell);
	kill();
}
