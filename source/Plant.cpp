#include "Globals.hpp"
#include "Plant.hpp"
#include "Player.hpp"
#include "World.hpp"

Plant::Plant( Direction orientation ) :
	orientation(orientation)
{
	if( isVertical(orientation) )
	{
		setAnimation("plant_vertical");
		setHeight(2.0);
	}
	else
	{
		setAnimation("plant_horizontal");
		setWidth(1.5);
	}

	setOrientation(orientation);

	phase = PHASE_WAIT;
	phaseTimer = PHASE_DURATION;

	setTileCollisionsEnabled(false);
	setGravityEnabled(false);
	setVelocityOrientationEnabled(false);
	setStompable(false);
	setKickable(false);
	setLayer(-1);
	setBounceOffEnemiesEnabled(false);
}

void Plant::onPreUpdate()
{
	phaseTimer--;
	if( phaseTimer == 0 )
	{
		// Compute some useful quantities
		Vector2<int> v = getVector(orientation);
		double metric;
		if( isVertical(orientation) )
		{
			metric = getHeight() / PHASE_DURATION * GAME_FPS;
		}
		else
		{
			metric = getWidth() / PHASE_DURATION * GAME_FPS;
		}

		// Determine the next phase and set it up
		switch( phase )
		{
		case PHASE_WAIT:
			{
				// Check if the player is too close - if so, keep waiting
				Player* player = getWorld().getPlayer();
				if( player != nullptr )
				{
					Vector2<double> distance( getCenterX() - player->getCenterX(), getCenterY() - player->getCenterY() );
					if( distance.getMagnitudeSquared() < WAIT_DISTANCE * WAIT_DISTANCE )
					{
						// Stay in this phase, the player is too close
						break;
					}
				}
				phase = PHASE_EXIT;
				setXVelocity(metric * v.x);
				setYVelocity(metric * v.y);
			}
			break;
		case PHASE_EXIT:
			phase = PHASE_ATTACK;
			setXVelocity(0);
			setYVelocity(0);
			break;
		case PHASE_ATTACK:
			phase = PHASE_ENTER;
			setXVelocity(metric * v.x * -1);
			setYVelocity(metric * v.y * -1);
			break;
		case PHASE_ENTER:
			phase = PHASE_WAIT;
			setXVelocity(0);
			setYVelocity(0);
			break;
		}

		phaseTimer = PHASE_DURATION;
	}
}
