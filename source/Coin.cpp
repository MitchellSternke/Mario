#include "Coin.hpp"
#include "Globals.hpp"
#include "Particle.hpp"
#include "World.hpp"

Coin::Coin() :
	Item( POINT_VALUE )
{
	setAnimation("coin");
	disableGravity();
	setWidth(COIN_WIDTH);
	setHeight(COIN_HEIGHT);
}

void Coin::sparkle()
{
	// Spawn coin sparkles
	for( int i = 0; i < 4; i++ )
	{
		Particle* p = new Particle( getAnimation("coin_sparkle"), COIN_SPARKLE_DELAY * i, true );
		p->setCollisionsEnabled(false);
		p->setGravityEnabled(false);
		double x = 0.0;
		double y = 0.0;
		switch( i )
		{
		case 0:
			x = -0.25;
			break;
		case 1:
			y = -0.5;
			break;
		case 2:
			x = 0.25;
			break;
		case 3:
			y = 0.5;
			break;
		}
		p->setCenterX(getCenterX() + x);
		p->setCenterY(getCenterY() + y);
		getWorld().addSprite(p);
	}
}
