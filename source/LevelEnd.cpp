#include "Globals.hpp"
#include "LevelEnd.hpp"
#include "Particle.hpp"
#include "Random.hpp"
#include "World.hpp"

LevelEnd::LevelEnd()
{
	setAnimation("level_end_block");
	setGravityEnabled(false);
}

void LevelEnd::onPreUpdate()
{
	if( getWorld().getFrameNumber() % 4 != 0 )
	{
		return;
	}

	Random& random = getWorld().getRandom();
	Particle* p = new Particle( getAnimation("coin_sparkle"), true );
	p->disableCollisions();
	p->disableGravity();
	p->setLayer(2);
	p->setCenterX(getLeft() + random.nextReal() * getWidth());
	p->setCenterY(getBottom() + random.nextReal() * getHeight());
	p->setXVelocity((random.nextReal() * 2.0 - 1.0) * 3.0);
	p->setYVelocity((random.nextReal() * 2.0 - 1.0) * 3.0);
	getWorld().addSprite( p );
}
