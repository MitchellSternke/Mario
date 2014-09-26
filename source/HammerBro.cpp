#include "Block.hpp"
#include "HammerBro.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "World.hpp"

/**
 * A hammer thrown by a hammer bro.
 */
class Hammer : public Sprite
{
public:
	Hammer()
	{
		setTileCollisionsEnabled(false);
		setAnimation("hammer");
		setGravityEnabled(false);
		setYAcceleration(HammerBro::GRAVITY);
	}

private:
	void onCollision( Sprite& sprite, Edge edge )
	{
		Player* player = dynamic_cast<Player*>(&sprite);
		if( player != nullptr )
		{
			player->takeDamage();
		}
	}
};

HammerBro::HammerBro() :
	jumpTimer(JUMP_WAIT_TIME),
	moveTimer(MOVE_DURATION)
{
	setWidth(WIDTH);
	setHeight(HEIGHT);

	setBounceOffEnemiesEnabled(false);
	setVelocityOrientationEnabled(false);
	setGravityEnabled(false);
	setYAcceleration(GRAVITY);

	setXVelocity(-1 * MOVEMENT_SPEED);

	setAnimation("hammer_bro");
}

void HammerBro::onPreUpdate()
{
	// Handle movement
	if( moveTimer > 0 )
	{
		moveTimer--;
		if( moveTimer == 0 )
		{
			// Change directions
			setXVelocity(-1 * getXVelocity());

			// Reset the timer
			moveTimer = MOVE_DURATION;
		}
	}

	// Re-enable tile collisions once we start falling
	if( getYVelocity() < 0.0 )
	{
		setTileCollisionsEnabled(true);
	}

	// Handle jumping
	if( jumpTimer > 0 )
	{
		jumpTimer--;
		if( jumpTimer == 0 )
		{
			if( !isFlying() )
			{
				// If there is a brick below us, drop down
				const Tile* tile = getWorld().getTile( static_cast<int>(std::floor(getCenterX())), static_cast<int>(std::floor(getBottom() - 0.5)) );
				const Block* block = dynamic_cast<const Block*>(tile);
				if( tile != nullptr && block != nullptr && block->getBlockType() == BLOCK_BRICK )
				{
					setYVelocity(-1.0);
				}
				else
				{
					// Jump
					setYVelocity(JUMP_VELOCITY);
				}

				// Disable collisions until y velocity is below zero
				setTileCollisionsEnabled(false);
			}

			// Reset the jump timer
			jumpTimer = JUMP_WAIT_TIME;
		}
	}

	// Set orientation based upon player position
	// Also attempt to throw hammers
	Player* player = getWorld().getPlayer();
	if( player != nullptr )
	{
		Hammer* hammer = nullptr;
		if( getWorld().getRandom().nextInt() % 60 == 0 )
		{
			playAnimation("hammer_bro_throw", "hammer_bro");
			hammer = new Hammer;
			hammer->setCenterX(getCenterX());
			hammer->setBottom(getTop());
			hammer->setYVelocity(JUMP_VELOCITY);
			getWorld().addSprite(hammer);
		}

		if( player->getCenterX() > getCenterX() )
		{
			setOrientation(RIGHT);
			if( hammer != nullptr )
			{
				hammer->setXVelocity(2.0);
				hammer->setOrientation(RIGHT);
			}
		}
		else
		{
			setOrientation(LEFT);
			if( hammer != nullptr )
			{
				hammer->setXVelocity(-2.0);
				hammer->setOrientation(LEFT);
			}
		}
	}
}
