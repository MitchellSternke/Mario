#include "Blaster.hpp"
#include "Bullet.hpp"
#include "Particle.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "World.hpp"

Blaster::Blaster() :
	Tile(TILE_SOLID),
	lastBlastFrame(0)
{
	setLayer(1);
}

void Blaster::onInit()
{
	if( getHeight() == 2 )
	{
		setAnimation("blaster");
	}
	else if( getHeight() == 1 )
	{
		setAnimation("blaster_short");
	}
}

void Blaster::onRender()
{
	// Blast bullets a minimum of every three seconds, with some randomness
	if( getWorld().getFrameNumber() - lastBlastFrame > 180 && getWorld().getRandom().nextInt() % 60 == 0 )
	{
		// Determine which way the player is facing
		int sign = 1;
		Player* player = getWorld().getPlayer();
		if( player != nullptr && player->getCenterX() < getCenterX() )
		{
			sign = -1;
		}

		// Create the bullet
		Bullet* bullet = new Bullet;
		bullet->setXVelocity( sign * Bullet::BULLET_SPEED );
		bullet->setCenterX( getCenterX() );
		bullet->setBottom( getTop() - 1.0 );
		getWorld().addSprite(bullet);

		// Add some backblast
		Particle* p = new Particle( getAnimation("fireball_smoke"), true );
		p->setGravityEnabled(false);
		p->setXVelocity( -1 * sign * Bullet::BULLET_SPEED );
		p->setCenterX( getCenterX() );
		p->setTop( getTop() );
		getWorld().addSprite(p);

		playSound("blast");

		lastBlastFrame = getWorld().getFrameNumber();
	}
}
