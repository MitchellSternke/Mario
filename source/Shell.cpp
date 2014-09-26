#include "Enemy.hpp"
#include "Globals.hpp"
#include "Particle.hpp"
#include "Player.hpp"
#include "Shell.hpp"
#include "Sound.hpp"
#include "World.hpp"

Shell::Shell(bool fireproof) :
	kickTimer(0),
	kills(0),
	player(nullptr)
{
	setFireproof(fireproof);
	setWidth(WIDTH);
	setHeight(HEIGHT);
	if( fireproof )
	{
		setAnimation("beetle_shell");
	}
	else
	{
		setAnimation("shell");
	}
	setKickable(true);
	setStompable(false);
	setHoldingEnabled(true);
}

Player* Shell::getPlayer()
{
	return player;
}

void Shell::onCollision(Sprite& sprite, Edge edge)
{
	if( isHeld() )
	{
		Enemy* enemy = dynamic_cast<Enemy*>(&sprite);
		if( enemy != nullptr )
		{
			getHoldingPlayer()->scorePoints(200, getX(), getY());
			enemy->hit();
			hit();
		}
		return;
	}

	Player* p = dynamic_cast<Player*>(&sprite);
	if( p != nullptr )
	{
		if( p->getYVelocity() > 0.0 )
		{
			return;
		}

		if( getXVelocity() == 0.0 )
		{
			p->scorePoints(100, getX(), getY());
		}
		startMoving( p );

		return;
	}

	// Kill enemies if we are moving
	if( getXVelocity() == 0.0 )
	{
		return;
	}

	Enemy* enemy = dynamic_cast<Enemy*>(&sprite);
	if( enemy != nullptr )
	{
		enemy->hit();

		// Create an effect for getting hit
		const Animation* a = getAnimation("hit");
		Particle* p = new Particle(a, true);
		p->setCenterX(getCenterX());
		p->setCenterY(getCenterY());
		p->setGravityEnabled(false);
		getWorld().addSprite(p);

		// Get points for the player that kicked the shell
		if( player != nullptr )
		{
			int points = MULTIPLE_KILL_POINT_SEQUENCE[kills];
			if( points != -1 )
			{
				char killSound[8];
				sprintf( killSound, "hit_%d", kills );
				playSound( killSound, 0 );
				player->scorePoints(points, enemy->getCenterX(), enemy->getCenterY());
				kills++;
			}
			else
			{
				player->gainLives(1, enemy->getCenterX(), enemy->getCenterY());
			}
		}
	}
}

void Shell::onCollision(Tile& tile, Edge edge)
{
	bounceOffWalls(tile, edge);
}

void Shell::onPreUpdate()
{
	// Only allow stomping after a certain number of frames after the shell has been kicked (this prevents a weird logic bug)
	if( kickTimer > 0 && --kickTimer == 0 )
	{
		setStompable(true);

		// Also make it unsafe to kick now
		// We wait until this timer goes off in case the player kicks the shell too close to a tile
		setKickable(false);
	}
}

void Shell::onRelease()
{
	startMoving(getHoldingPlayer());
}

void Shell::onStomp()
{
	stopMoving();
	kills = 0;
}

void Shell::startMoving( Player* kicker )
{
	if( getXVelocity() != 0.0 )
	{
		return;
	}
	else if( kicker->getCenterX() < getCenterX() )
	{
		setXVelocity(MOVEMENT_SPEED);
	}
	else
	{
		setXVelocity(-1.0 * MOVEMENT_SPEED);
	}

	setHoldingEnabled(false);

	if( isFireproof() )
	{
		setAnimation("beetle_shell_spinning");
	}
	else
	{
		setAnimation("shell_spinning");
	}
	playSound("kick");
	player = kicker;

	kickTimer = KICK_TIMER;
	kills = 1;
}

void Shell::stopMoving()
{
	setXVelocity(0.0);
	setKickable(true);
	setStompable(false);
	setHoldingEnabled(true);
	setAnimationPaused(true, getWorld().getFrameNumber());

	player = nullptr;
}
