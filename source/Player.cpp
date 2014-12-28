#include <cmath>

#include "Block.hpp"
#include "Coin.hpp"
#include "Color.hpp"
#include "Enemy.hpp"
#include "Fireball.hpp"
#include "Flower.hpp"
#include "Globals.hpp"
#include "Item.hpp"
#include "Ladder.hpp"
#include "Leaf.hpp"
#include "LevelEnd.hpp"
#include "Math.hpp"
#include "Mushroom.hpp"
#include "Pipe.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "Rendering.hpp"
#include "ReserveItem.hpp"
#include "Shell.hpp"
#include "Sound.hpp"
#include "Star.hpp"
#include "TextParticle.hpp"
#include "Tile.hpp"
#include "World.hpp"

///@todo these were taken from MainState.cpp. Find a better place to put them.
#define VIEW_WIDTH (SETTINGS.screenWidth / (SETTINGS.scale * (double)UNIT_SIZE))
#define VIEW_HEIGHT (SETTINGS.screenHeight / (SETTINGS.scale * (double)UNIT_SIZE))

static const double DEAD_PLAYER_INITIAL_VELOCITY = 16.0; /**< The initial y velocity of the player upon death. */
static const double DEAD_PLAYER_GRAVITY = -32.0;         /**< The gravity of the player when dead. */

static const int    MINIMUM_SCORE_PARTICLE_POINTS = 100; /**< The minimum points required when scoring for a text particle to appear. */

static const double TEXT_PARTICLE_SPEED = 5.0;    /**< The speed of a text particle. */
static const double TEXT_PARTICLE_GRAVITY = -10.0; /**< The gravity of a text particle. */
static const double TEXT_PARTICLE_DURATION = 0.5;  /**< Number of seconds that a particle displaying points/1up will live. */

static const int    INVINCIBILITY_DURATION = 120; /**< The number of frames that the player is invincible after being hit. */

static const int    STAR_DURATION = 600; /**< The number of frames that invincibility from a star lasts. */

// Note: put initialization in the reset() method
Player::Player( int controllerPort ) :
	coins(0),
	controller(INPUT_MANAGER.getController(controllerPort)),
	lives(5),
	reserveItem(nullptr),
	score(0)
{
	setWidth(WIDTH);
	setHeight(SMALL_HEIGHT);
	disableGravity(); // We compute our own gravity
	setDeathBoundaryEnabled(false); // Don't destroy us if we are out of bounds

	reset();
	setState(SMALL);
	setAnimation(stillAnimation);
}

Player::~Player()
{
	delete reserveItem;
}

void Player::attemptWarp( Direction direction )
{
	// The player must be on the ground to warp in any direction other than up
	if( isFlying() && direction != UP )
	{
		return;
	}

	// Find the pipe in the requested direction
	const Tile* tile = nullptr;
	switch( direction )
	{
	case DOWN:
		tile = getWorld().getTile( static_cast<int>(std::floor(getCenterX())), static_cast<int>(std::floor(getBottom() - EPSILON)) );
		break;
	case LEFT:
		tile = getWorld().getTile( static_cast<int>(std::floor(getLeft() - EPSILON)), static_cast<int>(std::floor(getBottom())) );
		break;
	case RIGHT:
		tile = getWorld().getTile( static_cast<int>(std::floor(getRight() + EPSILON)), static_cast<int>(std::floor(getBottom())) );
		break;
	case UP:
		tile = getWorld().getTile( static_cast<int>(std::floor(getCenterX())), static_cast<int>(std::floor(getTop() + EPSILON)) );
		break;
	default:
		break;
	}

	// Did we find a pipe?
	if( tile == nullptr )
	{
		return;
	}

	// Also, check that the direction of warping matches the pipe direction
	const Pipe* pipe = dynamic_cast<const Pipe*>(tile);
	if( pipe == nullptr ||
		(pipe->getOrientation() == DOWN && direction != UP) ||
		(pipe->getOrientation() == LEFT && direction != RIGHT) ||
		(pipe->getOrientation() == RIGHT && direction != LEFT) ||
		(pipe->getOrientation() == UP && direction != DOWN) )
	{
		return;
	}

	// Check that the bounding box for the player fits in the pipe
	switch( pipe->getOrientation() )
	{
	case DOWN:
	case UP:
		if( std::fabs(pipe->getCenterX() - getCenterX()) > 0.25 ) ///@todo constant
		{
			return;
		}
		break;
	case LEFT:
	case RIGHT:
		if( pipe->getBottom() > getBottom() || pipe->getTop() < getTop() )
		{
			return;
		}
		break;
	default:
		break;
	}

	warpDestination = pipe->getWarpLocation();
	if( warpDestination.levelId == NULL_LEVEL )
	{
		// The pipe is non-functional
		return;
	}

	// This takes care of the warp pipe animation and setting the new location of the player
	switch( pipe->getOrientation() )
	{
	case DOWN:
		setXVelocity(0.0);
		setYVelocity(getHeight() * GAME_FPS / WARP_TIMER);
		setAnimation(frontAnimation);
		break;
	case LEFT:
		setXVelocity(getWidth() * GAME_FPS / WARP_TIMER);
		setYVelocity(0.0);
		if( heldSprite != nullptr )
		{
			setAnimation(carryWalkAnimation);
		}
		else
		{
			setAnimation(walkAnimation);
		}
		break;
	case RIGHT:
		setXVelocity(-1.0 * getWidth() * GAME_FPS / WARP_TIMER);
		setYVelocity(0.0);
		if( heldSprite != nullptr )
		{
			setAnimation(carryWalkAnimation);
		}
		else
		{
			setAnimation(walkAnimation);
		}
		break;
	case UP:
		// Disable ducking if we are big. This can happen on the same frame as the warp.
		if( isDucking() )
		{
			ducking = false;
			if( state != SMALL )
			{
				setHeight(SUPER_HEIGHT);
			}
		}

		setXVelocity(0.0);
		setYVelocity(-1.0 * getHeight() * GAME_FPS / WARP_TIMER);
		setAnimation(frontAnimation);
		break;
	}
	setPhase(PHASE_WARP_EXIT);
}

void Player::decrementFireballCount()
{
	fireballCount--;
	if( fireballCount < 0 )
	{
		fireballCount = 0;
	}
}

void Player::dropReserveItem()
{
	if( reserveItem != nullptr )
	{
		double x = camera.getPosition().x;
		double y = camera.getPosition().y;
		renderClampView(x, y, VIEW_WIDTH, VIEW_HEIGHT, getWorld().getWidth(), getWorld().getHeight());

		ReserveItem* reserve = new ReserveItem(reserveItem);
		reserve->setCenterX(x);
		reserve->setTop(y + VIEW_HEIGHT / 2.0 - 1.0);
		getWorld().addSprite(reserve);
		reserveItem = nullptr;
		playSound("reserve_item_drop");
	}
}

void Player::gainCoins( int coins )
{
	this->coins += coins;
	if( this->coins > 99 )
	{
		this->coins -= 100;
		gainLives(1);
	}
	else
	{
		playSound("coin_get");
	}
}

void Player::gainLives(int lives, double x, double y)
{
	this->lives += lives;

	playSound("1up", 0);

	if( isDead() )
	{
		return;
	}

	if( x == 0.0 )
	{
		x = getX() + getWidth() / 2.0;
	}
	if( y == 0.0 )
	{
		y = getY() + getHeight() / 2.0;
	}

	Sprite* particle;
	if( lives > 1 )
	{
		char text[32];
		sprintf( text, "%dup", lives );
		particle = new TextParticle(text, TEXT_PARTICLE_DURATION);
	}
	else
	{
		particle = new Particle( getAnimation("score_1up"), TEXT_PARTICLE_DURATION );
	}

	// Draw a text particle
	particle->setCollisionsEnabled(false);
	particle->setGravityEnabled(false);
	particle->setYVelocity(TEXT_PARTICLE_SPEED);
	particle->setYAcceleration(TEXT_PARTICLE_GRAVITY);
	particle->setLayer(1);
	getWorld().addSprite(x, y, particle);
}

const Camera& Player::getCamera() const
{
	return camera;
}

int Player::getCoins() const
{
	return coins;
}

int Player::getLives() const
{
	return lives;
}

int Player::getPMeterBars() const
{
	return pMeter * P_METER_FULL_BARS / P_METER_FULL_FRAMES;
}

Item* Player::getReserveItem()
{
	return reserveItem;
}

int Player::getScore() const
{
	return score;
}

Player::State Player::getState() const
{
	return state;
}

void Player::grow()
{
	setHeight(SUPER_HEIGHT);

	if( isSolidTileAbove() )
	{
		ducking = true;
	}
}

bool Player::hasStar() const
{
	return (starTimer > 0);
}

void Player::holdSprite( Sprite* sprite )
{
	if( isHoldingSprite() )
	{
		return;
	}

	heldSprite = sprite;
	heldSprite->setHolding(true, this);
	heldSprite->setGravityEnabled(false);
}

bool Player::isAscending() const
{
	return (state == TAIL && flightTimer > 0 && controller->getButtonState(BUTTON_A));
}

bool Player::isClimbing()
{
	const Tile* tile = getWorld().getTile( static_cast<int>(std::floor(getCenterX())), static_cast<int>(std::floor(getCenterY())) );
	bool wasClimbing = climbing;
	if( tile == nullptr || (tile != nullptr && dynamic_cast<const Ladder*>(tile) == nullptr) || heldSprite != nullptr )
	{
		climbing = false;
	}
	else if( (!climbing && controller->getButtonState(BUTTON_UP)) || (climbing && isFlying()) )
	{
		climbing = true;
	}
	else
	{
		climbing = false;
	}

	// If we stopped climbing, cap x velocity
	if( !climbing && wasClimbing )
	{
		airMomentum = MAX_WALK_VELOCITY;
	}

	return climbing;
}

bool Player::isClimbingLadder() const
{
	return climbing;
}

bool Player::isDead() const
{
	return dead;
}

bool Player::isDescending() const
{
	return (state == TAIL && getYVelocity() < 0.0 && controller->getButtonState(BUTTON_A) && !isUnderwater() && !isDucking());
}

bool Player::isDucking() const
{
	return ducking;
}

bool Player::isHoldingSprite() const
{
	return (heldSprite != nullptr);
}

bool Player::isInvincible() const
{
	return (invincibilityTimer > 0);
}

bool Player::isPMeterFilled() const
{
	return (pMeter == P_METER_FULL_FRAMES);
}

bool Player::isSkidding() const
{
	return skidding;
}

bool Player::isSliding() const
{
	return sliding;
}

bool Player::isSolidTileAbove() const
{
	// Check if there is a solid object above the player
	for( double x = std::floor(getLeft()); x < std::ceil(getRight()); x++ )
	{
		const Tile* tile = getWorld().getTile(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(getBottom() + SUPER_HEIGHT)));
		if( tile != nullptr && tile->getCollisionBehavior() == TILE_SOLID )
		{
			return true;
		}
	}

	return false;
}

bool Player::isUnderwater() const
{
	return getWorld().isUnderwater(getCenterX(), getCenterY());
}

void Player::loadAnimations( State state )
{
	switch( state )
	{
		case SMALL:
			stillAnimation       = getAnimation("player_small");
			walkAnimation        = getAnimation("player_small_walk");
			runAnimation         = getAnimation("player_small_run");
			sprintAnimation      = getAnimation("player_small_sprint");
			jumpAnimation        = getAnimation("player_small_jump");
			fallAnimation        = jumpAnimation;
			turnAnimation        = getAnimation("player_small_turn");
			throwAnimation       = nullptr;
			duckAnimation        = nullptr;
			carryStillAnimation  = getAnimation("player_small_carrying");
			carryWalkAnimation   = getAnimation("player_small_carrying_walk");
			carryRunAnimation    = getAnimation("player_small_carrying_run");
			carrySprintAnimation = getAnimation("player_small_carrying_sprint");
			carryJumpAnimation   = getAnimation("player_small_carrying_jump");
			kickAnimation        = getAnimation("player_small_kick");
			slideAnimation       = getAnimation("player_small_slide");
			swimAnimation        = getAnimation("player_small_swim");
			swimMoveAnimation    = getAnimation("player_small_swim_move");
			sprintJumpAnimation  = getAnimation("player_small_sprint_jump");
			climbAnimation       = getAnimation("player_small_climb");
			climbMoveAnimation   = getAnimation("player_small_climb_move");
			frontAnimation       = getAnimation("player_small_front");
			flightAnimation      = nullptr;
			descentAnimation     = nullptr;
			spinAnimation        = nullptr;
			break;

		case SUPER:
			stillAnimation       = getAnimation("player_super");
			walkAnimation        = getAnimation("player_super_walk");
			runAnimation         = getAnimation("player_super_run");
			sprintAnimation      = getAnimation("player_super_sprint");
			jumpAnimation        = getAnimation("player_super_jump");
			fallAnimation        = getAnimation("player_super_fall");
			turnAnimation        = getAnimation("player_super_turn");
			throwAnimation       = nullptr;
			duckAnimation        = getAnimation("player_super_duck");
			carryStillAnimation  = getAnimation("player_super_carrying");
			carryWalkAnimation   = getAnimation("player_super_carrying_walk");
			carryRunAnimation    = getAnimation("player_super_carrying_run");
			carrySprintAnimation = getAnimation("player_super_carrying_sprint");
			carryJumpAnimation   = getAnimation("player_super_carrying_jump");
			kickAnimation        = getAnimation("player_super_kick");
			slideAnimation       = getAnimation("player_super_slide");
			swimAnimation        = getAnimation("player_super_swim");
			swimMoveAnimation    = getAnimation("player_super_swim_move");
			sprintJumpAnimation  = getAnimation("player_super_sprint_jump");
			climbAnimation       = getAnimation("player_super_climb");
			climbMoveAnimation   = getAnimation("player_super_climb_move");
			frontAnimation       = getAnimation("player_super_front");
			flightAnimation      = nullptr;
			descentAnimation     = nullptr;
			spinAnimation        = nullptr;
			break;

		case FIRE:
			stillAnimation       = getAnimation("player_fire");
			walkAnimation        = getAnimation("player_fire_walk");
			runAnimation         = getAnimation("player_fire_run");
			sprintAnimation      = getAnimation("player_fire_sprint");
			jumpAnimation        = getAnimation("player_fire_jump");
			fallAnimation        = getAnimation("player_fire_fall");
			turnAnimation        = getAnimation("player_fire_turn");
			throwAnimation       = getAnimation("player_fire_throw");
			duckAnimation        = getAnimation("player_fire_duck");
			carryStillAnimation  = getAnimation("player_fire_carrying");
			carryWalkAnimation   = getAnimation("player_fire_carrying_walk");
			carryRunAnimation    = getAnimation("player_fire_carrying_run");
			carrySprintAnimation = getAnimation("player_fire_carrying_sprint");
			carryJumpAnimation   = getAnimation("player_fire_carrying_jump");
			kickAnimation        = getAnimation("player_fire_kick");
			slideAnimation       = getAnimation("player_fire_slide");
			swimAnimation        = getAnimation("player_fire_swim");
			swimMoveAnimation    = getAnimation("player_fire_swim_move");
			sprintJumpAnimation  = getAnimation("player_fire_sprint_jump");
			climbAnimation       = getAnimation("player_fire_climb");
			climbMoveAnimation   = getAnimation("player_fire_climb_move");
			frontAnimation       = getAnimation("player_fire_front");
			flightAnimation      = nullptr;
			descentAnimation     = nullptr;
			spinAnimation        = nullptr;
			break;

		case TAIL:
			stillAnimation       = getAnimation("player_tail");
			walkAnimation        = getAnimation("player_tail_walk");
			runAnimation         = getAnimation("player_tail_run");
			sprintAnimation      = getAnimation("player_tail_sprint");
			jumpAnimation        = getAnimation("player_tail_jump");
			fallAnimation        = getAnimation("player_tail_fall");
			turnAnimation        = getAnimation("player_tail_turn");
			throwAnimation       = nullptr;
			duckAnimation        = getAnimation("player_tail_duck");
			carryStillAnimation  = getAnimation("player_tail_carrying");
			carryWalkAnimation   = getAnimation("player_tail_carrying_walk");
			carryRunAnimation    = getAnimation("player_tail_carrying_run");
			carrySprintAnimation = getAnimation("player_tail_carrying_sprint");
			carryJumpAnimation   = getAnimation("player_tail_carrying_jump");
			kickAnimation        = getAnimation("player_tail_kick");
			slideAnimation       = getAnimation("player_tail_slide");
			swimAnimation        = getAnimation("player_tail_swim");
			swimMoveAnimation    = getAnimation("player_tail_swim_move");
			sprintJumpAnimation  = getAnimation("player_tail_sprint_jump");
			climbAnimation       = getAnimation("player_tail_climb");
			climbMoveAnimation   = getAnimation("player_tail_climb_move");
			frontAnimation       = getAnimation("player_tail_front");
			flightAnimation      = getAnimation("player_tail_flight");
			descentAnimation     = getAnimation("player_tail_descent");
			spinAnimation        = getAnimation("player_tail_spin");
			break;
	}
}

void Player::onButtonToggle( InputButton button, bool pressed )
{
	if( isDead() || phase != PHASE_DEFAULT )
	{
		return;
	}

	switch( button )
	{
		case BUTTON_A:
			if( pressed && isUnderwater() )
			{
				setYVelocity(clamp(getYVelocity() + SWIM_POWER_FACTOR, 0.0, VELOCITY(0x0200)));
				playAnimation(swimMoveAnimation);
				playSound("swim");
			}
			else if( pressed && (!isFlying() || climbing) )
			{
				climbing = false;
				airMomentum = getXVelocity();
				if( std::fabs(airMomentum) < JUMP_THRESHOLD_0 )
				{
					setYVelocity(JUMP_VELOCITY_0);
				}
				else if( std::fabs(airMomentum) < JUMP_THRESHOLD_1 )
				{
					setYVelocity(JUMP_VELOCITY_1);
				}
				else if( std::fabs(airMomentum) < JUMP_THRESHOLD_2 )
				{
					setYVelocity(JUMP_VELOCITY_2);
				}
				else
				{
					setYVelocity(JUMP_VELOCITY_3);
				}

				if( isPMeterFilled() && getState() == TAIL )
				{
					// Begin flight
					flightTimer = FLIGHT_DURATION;
				}

				playSound("jump");
			}
			break;

		case BUTTON_B:
			if( pressed && state == FIRE && !isDucking() && !isHoldingSprite() )
			{
				// We can only have two fireballs at once
				if( fireballCount > 1 ) ///@todo constant
				{
					break;
				}

				if( isFlying() )
				{
					///@todo should this truly be a separate animation?
					playAnimation(swimMoveAnimation);
				}
				else
				{
					playAnimation(throwAnimation);
				}

				double xv = Fireball::X_VELOCITY;
				if( orientation == -1 )
				{
					xv *= -1.0;
				}
				playSound("throw");
				Sprite* fireball = new Fireball(this, xv);
				getWorld().addSprite(getCenterX(), getCenterY(), fireball);
				fireball->setCenterX(getCenterX());
				fireball->setCenterY(getCenterY());
				fireballCount++;
			}
			else if( pressed && state == TAIL && !isDucking() && !isHoldingSprite() && !climbing )
			{
				// Spin the tail
				playAnimation(spinAnimation);
				playSound("spin");

				double left;
				double bottom = getBottom();
				double width = 0.9;
				double height = 1.0;
				if( orientation == 1 )
				{
					left = getRight();
				}
				else
				{
					left = getLeft() - width;
				}

				// Attack any enemies in front of us
				auto sprites = getWorld().getSpritesInBox(left, bottom, width, height);
				for( auto sprite : sprites )
				{
					Enemy* enemy = dynamic_cast<Enemy*>(sprite);
					if( enemy != nullptr )
					{
						enemy->bump();
						scorePoints( MULTIPLE_KILL_POINT_SEQUENCE[0], enemy->getX(), enemy->getY() );
						playSound("kick");
					}
				}

				// Bump any blocks in front of us
				auto tiles = getWorld().getTilesInBox(left, bottom, width, height);
				for( auto tile : tiles )
				{
					Block* block = dynamic_cast<Block*>(tile);
					if( block != nullptr )
					{
						block->bump(true, false, this);
					}
				}
			}
			else if( !pressed && isHoldingSprite() )
			{
				releaseHeldSprite(true);
			}
			break;

		case BUTTON_LEFT:
			if( pressed )
			{
				if( orientation != -1 )
				{
					orientationChangePosition = getX();
				}
				orientation = -1;
				setOrientation(LEFT);
			}
			break;

		case BUTTON_RIGHT:
			if( pressed )
			{
				if( orientation != 1 )
				{
					orientationChangePosition = getX();
				}
				orientation = 1;
				setOrientation(RIGHT);
			}
			break;

		case BUTTON_SELECT:
			if( pressed )
			{
				dropReserveItem();
			}
			break;

		default:
			break;
	}
}

void Player::onCollision(Sprite& sprite, Edge edge)
{
	// Pick up sprites that can be held
	if( sprite.isHoldingEnabled() && !isHoldingSprite() && !sprite.isHeld() && INPUT_MANAGER.getController(0)->getButtonState(BUTTON_B) && getYVelocity() <= 0.0 )
	{
		holdSprite(&sprite);
		return;
	}

	Enemy* enemy = dynamic_cast<Enemy*>(&sprite);
	if( enemy != nullptr )
	{
		// If we have a star, kill the enemy
		if( hasStar() && !enemy->isHeld() )
		{
			enemy->hit();
			if( MULTIPLE_KILL_POINT_SEQUENCE[starKills] != -1 )
			{
				// Play the appropriate sound
				char killSound[8];
				sprintf( killSound, "hit_%d", starKills );
				playSound( killSound );

				scorePoints(MULTIPLE_KILL_POINT_SEQUENCE[starKills], enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
				starKills++;
			}
			else
			{
				gainLives(1, enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
			}
		}
		// If we are sliding, kill the enemy
		else if( isSliding() && !enemy->isHeld() )
		{
			enemy->hit();
			if( MULTIPLE_KILL_POINT_SEQUENCE[slideKills] != -1 )
			{
				// Play the appropriate sound
				if( slideKills == 0 )
				{
					playSound("kick");
				}
				else
				{
					char killSound[8];
					sprintf( killSound, "hit_%d", slideKills );
					playSound( killSound );
				}

				scorePoints(MULTIPLE_KILL_POINT_SEQUENCE[slideKills], enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
				slideKills++;
			}
			else
			{
				gainLives(1, enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
			}
		}
		// Did we stomp the enemy?
		else if( isFlying() && (edge == EDGE_BOTTOM || getYVelocity() < 0.0) )
		{
			if( enemy->isStompable() )
			{
				enemy->stomp();
				playSound("stomp");
				setYVelocity(STOMP_VELOCITY);

				if( MULTIPLE_KILL_POINT_SEQUENCE[stompCount] != -1 )
				{
					scorePoints(MULTIPLE_KILL_POINT_SEQUENCE[stompCount], enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
					if( !isUnderwater() )
					{
						stompCount++;
					}
				}
				else
				{
					gainLives(1, enemy->getX() + enemy->getWidth() / 2.0, enemy->getY());
				}
			}
		}
		// Did the enemy injure us?
		else if( !enemy->isKickable() && !isInvincible() && !(getBottom() > enemy->getBottom() && enemy->isStompable()) )
		{
			// Don't let shells hurt us if we run into them from behind
			if( !(dynamic_cast<Shell*>(enemy) != nullptr &&
				((getCenterX() < enemy->getCenterX() && enemy->getXVelocity() > 0.0) ||
				(getCenterX() > enemy->getCenterX() && enemy->getXVelocity() < 0.0)))
			)
			{
				takeDamage();
			}
		}
		return;
	}
	Item* item = dynamic_cast<Item*>(&sprite);
	if( item != nullptr )
	{
		Coin* coin = dynamic_cast<Coin*>(item);
		ReserveItem* reserve = nullptr;
		if( coin != nullptr )
		{
			gainCoins(1);
			coin->sparkle();
		}
		else
		{
			reserve = dynamic_cast<ReserveItem*>(item);
			if( reserve != nullptr )
			{
				item = reserve->getItem();
				reserve->kill();
			}
			if( dynamic_cast<Mushroom*>(item) != nullptr )
			{
				setReserveItem(SUPER);
				if( state == SMALL )
				{
					setState( SUPER, true );
				}
			}
			else if( dynamic_cast<Flower*>(item) != nullptr )
			{
				setReserveItem(FIRE);
				setState( FIRE, true );
			}
			else if( dynamic_cast<Leaf*>(item) != nullptr )
			{
				setReserveItem(TAIL);
				setState( TAIL, true );
			}
			else if( dynamic_cast<Star*>(item) != nullptr )
			{
				starTimer = STAR_DURATION;
				if( starKills == 0 )
				{
					starKills = 1;
				}
				playMusic("starman", true);
			}
		}

		if( reserve == nullptr )
		{
			scorePoints( item->getPointValue(), item->getX() + item->getWidth(), item->getY() );
			item->kill();
		}
	}
	LevelEnd* end = dynamic_cast<LevelEnd*>(&sprite);
	if( end != nullptr )
	{
		// We are now done with the level
		end->kill();
		getWorld().endLevel();
	}
}

void Player::onGround()
{
	stompCount = 0;
	flightTimer = 0;
}

void Player::onPostUpdate()
{
	// Check if we are in a different phase and process accordingly
	switch( phase )
	{
	case PHASE_DEAD:
		// Do nothing...
		break;
	case PHASE_TRANSFORM:
		phaseTransform();
		break;
	case PHASE_WARP_ENTER:
		phaseWarpEnter();
		break;
	case PHASE_WARP_EXIT:
		phaseWarpExit();
		break;
	default:
		// Handle pipe warping
		if( controller->getButtonState(BUTTON_DOWN) )
		{
			attemptWarp(DOWN);
		}
		if( controller->getButtonState(BUTTON_LEFT) )
		{
			attemptWarp(LEFT);
		}
		if( controller->getButtonState(BUTTON_RIGHT) )
		{
			attemptWarp(RIGHT);
		}
		if( controller->getButtonState(BUTTON_UP) )
		{
			attemptWarp(UP);
		}
		break;
	}

	// Manage the held sprite (if we have one)
	if( heldSprite != nullptr )
	{
		if( (phase == PHASE_WARP_ENTER || phase == PHASE_WARP_EXIT) && getYVelocity() != 0.0 )
		{
			heldSprite->setCenterX(getCenterX());
		}
		else if( orientation == 1 )
		{
			heldSprite->setLeft(getRight());
		}
		else
		{
			heldSprite->setRight(getLeft());
		}
		heldSprite->setBottom(getBottom() + getHeight() / 4.0);
	}

	// Check if we fell into a pit
	if( getTop() < 0.0 )
	{
		takeDamage(true);
	}

	// Handle p-meter physics
	if( flightTimer > 0 ) // We're in flight mode
	{
		flightTimer--;
		if( flightTimer < 0 )
		{
			// When flight mode is over, the p-meter drops to zero
			pMeter = 0;
		}
	}
	else if( std::fabs(getXVelocity()) >= MAX_RUN_VELOCITY )
	{
		pMeter += 2;
	}
	else
	{
		pMeter--;
	}

	if( pMeter > P_METER_FULL_FRAMES )
	{
		pMeter = P_METER_FULL_FRAMES;
	}
	else if( pMeter < 0 )
	{
		pMeter = 0;
	}

	// Check if we died this frame and update accordingly
	if( dead && phase != PHASE_DEAD )
	{
		setPhase(PHASE_DEAD);
	}

	// Determine the animation to use
	double speed = std::fabs(getXVelocity());
	if( phase == PHASE_DEAD || phase == PHASE_WARP_ENTER || phase == PHASE_WARP_EXIT )
	{
		; // No change
	}
	else if( isAscending() && flightAnimation != nullptr && heldSprite == nullptr )
	{
		setAnimation(flightAnimation);
		if( getWorld().getFrameNumber() % flightAnimation->getLength() == 0 )
		{
			playSound("spin", 0);
		}
	}
	else if( isDescending() && descentAnimation != nullptr && heldSprite == nullptr )
	{
		setAnimation(descentAnimation);
			if( getWorld().getFrameNumber() % descentAnimation->getLength() == 0 )
		{
			playSound("spin", 0);
		}
	}
	else if( climbing )
	{
		if( speed != 0.0 || std::fabs(getYVelocity()) != 0.0 )
		{
			setAnimation(climbMoveAnimation);
		}
		else
		{
			setAnimation(climbAnimation);
		}
	}
	else if( isUnderwater() && isFlying() )
	{
		setAnimation(swimAnimation);
	}
	else if( isSliding() )
	{
		setAnimation( slideAnimation );

		if( getWorld().getFrameNumber() % 2 == 0 )
		{
			// Also create a smoke particle
			Particle* p = new Particle(getAnimation("slide_smoke"), true);
			if( getXVelocity() > 0.0 )
			{
				p->setCenterX(getLeft());
			}
			else
			{
				p->setCenterX(getRight());
			}
			p->setBottom(getBottom());
			p->setGravityEnabled(false);
			getWorld().addSprite(p);
		}
	}
	else if( heldSprite != nullptr )
	{
		if( isFlying() )
		{
			setAnimation( carryJumpAnimation );
		}
		else if( speed > MAX_RUN_VELOCITY )
		{
			setAnimation( carrySprintAnimation );
		}
		else if( speed > MAX_WALK_VELOCITY )
		{
			setAnimation( carryRunAnimation );
		}
		else if( speed > EPSILON )
		{
			setAnimation( carryWalkAnimation );
		}
		else
		{
			setAnimation( carryStillAnimation );
		}
	}
	else if( isDucking() )
	{
		setAnimation( duckAnimation );
	}
	else if( isFlying() )
	{
		///@todo this was copied from onPreUpdate(). Should this be the only copy?
		if( airMomentum == 0.0 )
		{
			airMomentum = getXVelocity();
		}

		if( std::fabs(airMomentum) > MAX_RUN_VELOCITY )
		{
			setAnimation( sprintJumpAnimation );
		}
		else if( getYVelocity() > 0.0 )
		{
			setAnimation( jumpAnimation );
		}
		else
		{
			setAnimation( fallAnimation );
		}
	}
	else if( isSkidding() )
	{
		setAnimation( turnAnimation );

		if( getWorld().getFrameNumber() % 2 == 0 )
		{
			// Also create a smoke particle
			Particle* p = new Particle(getAnimation("slide_smoke"), true);
			if( getXVelocity() > 0.0 )
			{
				p->setCenterX(getLeft());
			}
			else
			{
				p->setCenterX(getRight());
			}
			p->setBottom(getBottom());
			p->setGravityEnabled(false);
			getWorld().addSprite(p);
		}
	}
	else if( speed > MAX_RUN_VELOCITY )
	{
		setAnimation( sprintAnimation );
	}
	else if( speed > MAX_WALK_VELOCITY )
	{
		setAnimation( runAnimation );
	}
	else if( speed > EPSILON )
	{
		setAnimation( walkAnimation );
	}
	else
	{
		setAnimation( stillAnimation );
	}

	// Handle the star power up
	if( hasStar() )
	{
		starTimer--;
		if( !hasStar() )
		{
			getWorld().playBackgroundMusic();
			starKills = 0;
			invincibilityTimer = INVINCIBILITY_DURATION;
			setRedMask(1.0f);
			setGreenMask(1.0f);
			setBlueMask(1.0f);
		}
		else
		{
			// Create a sparkle particle
			Random& random = getWorld().getRandom();
			Particle* p = new Particle( getAnimation("coin_sparkle"), true );
			p->disableCollisions();
			p->disableGravity();
			p->setLayer(2);
			p->setCenterX(getX() - getWidth() / 2.0 + 2.0 * random.nextReal() * getWidth());
			p->setCenterY(getY() - getHeight() / 2.0 + 2.0 * random.nextReal() * getHeight());
			p->setXVelocity((random.nextReal() * 2.0 - 1.0) * 3.0);
			p->setYVelocity((random.nextReal() * 2.0 - 1.0) * 3.0);
			getWorld().addSprite( p );

			// Change colors for effect
			Color color = Color::hsv(std::sin(getWorld().getFrameNumber() / 10.0) / 2.0 + 0.5, 1.0f, 1.0f);
			setRedMask(color.normalizedR());
			setGreenMask(color.normalizedG());
			setBlueMask(color.normalizedB());
		}
	}

	// Handle invincibility
	if( isInvincible() )
	{
		invincibilityTimer--;
		setAlpha(0.5f);
	}
	else
	{
		setAlpha(1.0f);
	}

	// Update the camera
	if( !isFlying() )
	{
		camera.setYTarget(getBottom());
		cameraLocked = true;
	}
	else if( (isFlying() && isPMeterFilled()) || isUnderwater() || isClimbing() || !cameraLocked )
	{
		camera.setYTarget(getCenterY());
		cameraLocked = false;
	}

	if( (getX() - orientationChangePosition < -1.0 && cameraOrientation == 1) ||
		(getX() - orientationChangePosition > 1.0 && cameraOrientation == -1 ) )
	{
		cameraOrientation = orientation;
	}
	camera.setXTarget( cameraOrientation + getCenterX() );

	camera.setSpeed( getMaximumXSpeed() + 1.0 , getMaximumYSpeed() + 1.0 );
	camera.update();
}

///@todo Clean this up!
void Player::onPreUpdate()
{
	// This only handles the default phase
	if( phase != PHASE_DEFAULT )
	{
		return;
	}

	double maxVelocity = MAX_WALK_VELOCITY;
	skidding = false;

	bool downPressed = controller->getButtonState(BUTTON_DOWN);
	bool leftPressed = controller->getButtonState(BUTTON_LEFT);
	bool rightPressed = controller->getButtonState(BUTTON_RIGHT);
	bool upPressed = controller->getButtonState(BUTTON_UP);

	if( leftPressed && !rightPressed )
	{
		orientation = -1;
		setOrientation(LEFT);
	}
	else if( rightPressed && !leftPressed )
	{
		orientation = 1;
		setOrientation(RIGHT);
	}

	// Climbing
	if( isClimbing() )
	{
		if( leftPressed && !rightPressed )
		{
			setXVelocity(-1.0 * CLIMB_SPEED);
		}
		else if( rightPressed && !leftPressed )
		{
			setXVelocity(CLIMB_SPEED);
		}
		else
		{
			setXVelocity(0.0);
		}

		if( downPressed && !upPressed )
		{
			setYVelocity(-1.0 * CLIMB_SPEED);
		}
		else if( upPressed && !downPressed )
		{
			setYVelocity(CLIMB_SPEED);
		}
		else
		{
			setYVelocity(0.0);
		}
	}
	// Underwater
	else if( isUnderwater() )
	{
		// Swimming
		if( isFlying() )
		{
			maxVelocity = MAX_SWIM_SPEED;
		}
		// Walking underwater
		else
		{
			maxVelocity = MAX_UNDERWATER_WALK_SPEED;

			// Are we moving too slow? Stop.
			///@todo constant (happens to be the velocity after walking underwater for one frame)
			if( std::fabs(getXVelocity()) < 0.07 )
			{
				setXVelocity(0.0);
			}
		}

		// Are we on the surface?
		if( !getWorld().isUnderwater(getCenterX(), getTop()) && getYVelocity() > 0.0 )
		{
			if( (controller->getButtonState(BUTTON_A) && controller->getButtonState(BUTTON_UP)) || getYVelocity() > VELOCITY(0x0200) )
			{
				setYVelocity(SWIM_JUMP_SPEED);
				airMomentum = sign(getXVelocity()) * MAX_WALK_VELOCITY;
			}
			else
			{
				setYVelocity(0.0);
			}
			setYAcceleration(-1.0 * SWIM_Y_ACCELERATION_SURFACE);
		}
		else
		{
			if( getYVelocity() > 0.0 )
			{
				setYAcceleration(-1.0 * SWIM_Y_ACCELERATION_UP);
			}
			else
			{
				setYAcceleration(-1.0 * SWIM_Y_ACCELERATION_DOWN);
			}
		}

		// Compute x acceleration
		double direction = 0.0;
		if( leftPressed && !rightPressed )
		{
			direction = -1.0;
		}
		else if( rightPressed && !leftPressed )
		{
			direction = 1.0;
		}
		else if( !rightPressed && !leftPressed )
		{
			if( getXVelocity() > 0.0 )
			{
				direction = -1.0;
			}
			else if( getXVelocity() < 0.0 )
			{
				direction = 1.0;
			}
		}

		if( (getXVelocity() >= 0.0 && rightPressed) || (getXVelocity() <= 0.0 && leftPressed) )
		{
			setXAcceleration(direction * SWIM_X_ACCELERATION);
		}
		else if( (getXVelocity() <= 0.0 && rightPressed) || (getXVelocity() >= 0.0 && leftPressed) )
		{
			setXAcceleration(direction * SWIM_X_ACCELERATION_REVERSE);
		}
		else
		{
			setXAcceleration(direction * SWIM_X_ACCELERATION_REVERSE);
		}
	}
	// In the Air
	else if( isFlying() )
	{
		sliding = false;

		if( airMomentum == 0.0 )
		{
			airMomentum = getXVelocity();
		}

		// Compute x acceleration
		double direction = 0.0;
		if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_LEFT) )
		{
			direction = -1.0;
		}
		else if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_RIGHT) )
		{
			direction = 1.0;
		}
		if( direction != 0.0 )
		{
			double momentumDirection = sign(airMomentum);
			if( direction != momentumDirection )
			{
				setXAcceleration(-momentumDirection * AIR_DECELERATION);
			}
			else
			{
				setXAcceleration(momentumDirection * AIR_ACCELERATION);
			}

			if( std::fabs(airMomentum) <= MAX_WALK_VELOCITY )
			{
				maxVelocity = MAX_WALK_VELOCITY;
			}
			else if( std::fabs(airMomentum) <= MAX_RUN_VELOCITY )
			{
				maxVelocity = MAX_RUN_VELOCITY;
			}
			else if( std::fabs(airMomentum) <= MAX_SPRINT_VELOCITY )
			{
				maxVelocity = MAX_SPRINT_VELOCITY;
			}
		}
		else
		{
			setXAcceleration(0.0);
		}
	}
	// On the ground
	else
	{
		airMomentum = 0.0;

		// Check for ducking
		if( (INPUT_MANAGER.getController(0)->getButtonState(BUTTON_DOWN) && getState() != SMALL && !isHoldingSprite() && !sliding) ||
			(getState() != SMALL && isSolidTileAbove()) )
		{
			ducking = true;
			setHeight(SMALL_HEIGHT);
		}
		else
		{
			ducking = false;

			if( state != SMALL )
			{
				setHeight(SUPER_HEIGHT);
			}
		}

		// Compute acceleration
		if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_LEFT) && !isDucking() )
		{
			if( getXVelocity() <= 0 )
			{
				setXAcceleration(-1.0 * GROUND_ACCELERATION);
			}
			else
			{
				setXAcceleration(-1.0 * GROUND_SKID_DECELERATION);
				skidding = true;
			}

			if( isSliding() )
			{
				sliding = false;
			}
		}
		else if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_RIGHT) && !isDucking() )
		{
			if( getXVelocity() >= 0 )
			{
				setXAcceleration(GROUND_ACCELERATION);
			}
			else
			{
				setXAcceleration(GROUND_SKID_DECELERATION);
				skidding = true;
			}

			if( isSliding() )
			{
				sliding = false;
			}
		}
		else if( std::fabs(getXVelocity()) > EPSILON )
		{
			setXAcceleration(-sign(getXVelocity()) * GROUND_DECELERATION);
		}
		else if( !isSliding() )
		{
			setXAcceleration(0.0);
			setXVelocity(0.0);
		}

		if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_B) )
		{
			maxVelocity = MAX_RUN_VELOCITY;
			if( isPMeterFilled() )
			{
				maxVelocity = MAX_SPRINT_VELOCITY;
			}
		}

		// Are we on a slope?
		Tile* slope = getSlope();
		if( slope != nullptr )
		{
			// Determine how we are using the slope
			bool upSlope = (slope->getCollisionBehavior() == TILE_SLOPE_DOWN && getXVelocity() < 0.0) ||
				(slope->getCollisionBehavior() == TILE_SLOPE_UP && getXVelocity() > 0.0);
			bool downSlope = (slope->getCollisionBehavior() == TILE_SLOPE_DOWN && getXVelocity() > 0.0) ||
				(slope->getCollisionBehavior() == TILE_SLOPE_UP && getXVelocity() < 0.0);
			bool fullSlope = slope->getHeight() / slope->getWidth() > 0.5;

			// Slide down slopes
			if( (INPUT_MANAGER.getController(0)->getButtonState(BUTTON_DOWN) || isSliding()) && !isHoldingSprite() )
			{
				if( !isSliding() )
				{
					slideKills = 0;
				}

				sliding = true;
				maxVelocity = SLIDING_MAX_SPEED;

				if( slope->getCollisionBehavior() == TILE_SLOPE_UP )
				{
					if( fullSlope )
					{
						setXAcceleration(-1.0 * SLIDING_ACCELERATION_FULL_SLOPE);
					}
					else
					{
						setXAcceleration(-1.0 * SLIDING_ACCELERATION_HALF_SLOPE);
					}
				}
				else if( slope->getCollisionBehavior() == TILE_SLOPE_DOWN )
				{
					if( fullSlope )
					{
						setXAcceleration(SLIDING_ACCELERATION_FULL_SLOPE);
					}
					else
					{
						setXAcceleration(SLIDING_ACCELERATION_HALF_SLOPE);
					}
				}
			}
			else
			{
				// Cap speed when going up slopes
				if( upSlope )

				{
					if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_B) )
					{
						maxVelocity = MAX_UPHILL_RUN_VELOCITY;
					}
					else
					{
						maxVelocity = MAX_UPHILL_WALK_VELOCITY;
					}
				}
				// Increase speed when going down slopes
				else if( downSlope )
				{
					if( fullSlope )
					{
						maxVelocity += ADDED_DOWNHILL_VELOCITY_FULL_SLOPE;
					}
					else
					{
						maxVelocity += ADDED_DOWNHILL_VELOCITY_HALF_SLOPE;
					}
				}
			}
		}
		else if( isSliding() && std::fabs(getXVelocity()) <= EPSILON )
		{
			sliding = false;
		}
	}

	// Cap x velocity
	setMaximumXSpeed(maxVelocity);

	// Compute gravity and cap y velocity
	if( isAscending() )
	{
		setYAcceleration(0.0);
		setMaximumYSpeed(MAX_JUMP_VELOCITY);
		setYVelocity(FLIGHT_VELOCITY);
	}
	else if( isDescending() )
	{
		// Slowed descent
		setMaximumYSpeed(DESCENT_VELOCITY);
	}
	else if( climbing )
	{
		setXAcceleration(0.0);
		setMaximumXSpeed(CLIMB_SPEED);
		setYAcceleration(0.0);
		setMaximumYSpeed(CLIMB_SPEED);
	}
	else if( isUnderwater() )
	{
		if( getYVelocity() < 0.0 )
		{
			setMaximumYSpeed(MAX_SWIM_FALL_SPEED);
		}
		else
		{
			setMaximumYSpeed(0.0);
		}
	}
	else
	{
		if( INPUT_MANAGER.getController(0)->getButtonState(BUTTON_A) && getYVelocity() >= JUMP_GRAVITY_THRESHOLD )
		{
			setYAcceleration(-1.0 * JUMP_GRAVITY_0);
		}
		else
		{
			setYAcceleration(-1.0 * JUMP_GRAVITY_1);
		}

		setMaximumYSpeed(MAX_JUMP_VELOCITY);
	}
}

void Player::phaseTransform()
{
	// Are we done transforming yet?
	if( transformTimer > 0 )
	{
		transformTimer--;

		if( (transformTimer / 4) % 2 )
		{
			loadAnimations(transformationBeginState);
		}
		else
		{
			loadAnimations(transformationEndState);
		}
	}
	else
	{
		// We're done
		loadAnimations(transformationEndState);
		setPhase(PHASE_DEFAULT);
	}
}

void Player::phaseWarpEnter()
{
	// Are we done moving out of the pipe yet?
	if( warpTimer > 0 )
	{
		warpTimer--;
		return;
	}

	// Fix for a one frame glitch on up pipes
	if( getYVelocity() > 0.0 )
	{
		setBottom( std::floor(getBottom()) );
		setYVelocity(-0.1);
		setFlying(false);
	}

	// End the warp phase
	setPhase(PHASE_DEFAULT);
}

void Player::phaseWarpExit()
{
	// Are we done moving into the pipe yet?
	if( warpTimer > 0 )
	{
		warpTimer--;
		return;
	}

	// If the warp destination is not the current level, we have some extra work to do
	if( warpDestination.levelId != CURRENT_LEVEL )
	{
		// Remove the player from the world
		World& world = getWorld();
		world.removeSprite(this);
		if( heldSprite != nullptr )
		{
			world.removeSprite(heldSprite);
		}

		world.setLevel(warpDestination.levelId);

		// Add the player to the world
		world.addSprite(0, 0, this);
		if( heldSprite != nullptr )
		{
			world.addSprite(0, 0, heldSprite);
		}
	}

	// Find what's at the location
	// If it's another pipe, teleport to the end of it
	auto tile = getWorld().getTile( warpDestination.x, warpDestination.y );
	auto pipe = dynamic_cast<const Pipe*>(tile);
	if( tile != nullptr &&  pipe != nullptr )
	{
		switch( pipe->getOrientation() )
		{
		case DOWN:
			setCenterX( tile->getCenterX() );
			setTop( tile->getBottom() + getHeight() );
			setXVelocity(0.0);
			setYVelocity(-1.0 * getHeight() * GAME_FPS / WARP_TIMER);
			setAnimation(frontAnimation);
			break;
		case LEFT:
			setRight( tile->getLeft() + getWidth() );
			setBottom( tile->getBottom() );
			setXVelocity(-1.0 * getWidth() * GAME_FPS / WARP_TIMER);
			setYVelocity(0.0);
			setAnimation(walkAnimation);
			setOrientation(LEFT);
			orientation = -1;
			break;
		case RIGHT:
			setLeft( tile->getRight() - getWidth() );
			setBottom( tile->getBottom() );
			setXVelocity(getWidth() * GAME_FPS / WARP_TIMER);
			setYVelocity(0.0);
			setAnimation(walkAnimation);
			setOrientation(RIGHT);
			orientation = 1;
			break;
		case UP:
			setCenterX( tile->getCenterX() );
			setBottom( tile->getTop() - getHeight() );
			setXVelocity(0.0);
			setYVelocity(getHeight() * GAME_FPS / WARP_TIMER);
			setAnimation(frontAnimation);
			break;
		default:
			break;
		}

		// Start the enter animation
		setPhase(PHASE_WARP_ENTER);
	}
	else
	{
		setCenterX( warpDestination.x );
		setBottom( warpDestination.y );

		// No animation here, just resume execution
		setPhase(PHASE_DEFAULT);
	}
}

void Player::releaseHeldSprite( bool kickIfShell )
{
	if( heldSprite == nullptr )
	{
		return;
	}

	heldSprite->setHolding(false);
	heldSprite->setGravityEnabled(true);

	// Kick if it was a shell
	if( kickIfShell && dynamic_cast<Shell*>(heldSprite) != nullptr )
	{
		playAnimation(kickAnimation);
		scorePoints( 100, heldSprite->getX(), heldSprite->getY() );
	}

	// Check if the sprite is inside of a tile.
	auto tile = getWorld().getTile(fastFloor(orientation == 1 ? heldSprite->getRight() : heldSprite->getLeft()), fastFloor(heldSprite->getBottom()));
	if( tile != nullptr && tile->getCollisionBehavior() != TILE_NONE )
	{
		if( orientation == 1 )
		{
			heldSprite->setRight(getRight());
		}
		else
		{
			heldSprite->setLeft(getLeft());
		}
	}

	heldSprite = nullptr;
}

void Player::reset()
{
	airMomentum = 0.0;
	cameraLocked = false;
	climbing = false;
	dead = false;
	ducking = false;
	fireballCount = 0;
	flightTimer = 0;
	heldSprite = nullptr;
	invincibilityTimer = 0;
	orientation = -1;
	setOrientation(LEFT);
	phase = PHASE_DEFAULT;
	pMeter = 0;
	skidding= false;
	slideKills = 0;
	sliding = false;
	starKills = 0;
	starTimer = 0;
	stompCount = 0;
	transformTimer = 0;
	warpTimer = 0;

	setXVelocity(0.0);
	setYVelocity(0.0);
	setRedMask(1.0f);
	setGreenMask(1.0f);
	setBlueMask(1.0f);
	setAlpha(1.0f);
	setAnimationPaused(false);
	setCollisionsEnabled(true);
	setMotionEnabled(true);
	setLayer(0);

	setMaximumXSpeed(0.0);
	setMaximumYSpeed(MAX_JUMP_VELOCITY);

	camera.setPosition(getX(), getY());
	cameraOrientation = 1;
	orientationChangePosition = getX();
}

void Player::setPhase( Phase phase )
{
	switch( phase )
	{
	case PHASE_DEFAULT:
		getWorld().setTimeEnabled(true);
		setCollisionsEnabled(true);
		setMotionEnabled(true);
		setLayer(0);
		if( heldSprite != nullptr )
		{
			heldSprite->setLayer(0);
		}
		setAnimationPaused(false);
		break;

	case PHASE_DEAD:
		climbing = false;
		if( getTop() > 0.0 )
		{
			setAnimation( "player_dead" );
		}
		else
		{
			setAnimation(nullptr);
		}
		setCollisionsEnabled(false);
		setMaximumXSpeed(0.0);
		setMaximumYSpeed(MAX_JUMP_VELOCITY);
		setXVelocity(0.0);
		setXAcceleration(0.0);
		setYVelocity(DEAD_PLAYER_INITIAL_VELOCITY);
		setYAcceleration(DEAD_PLAYER_GRAVITY);
		break;

	case PHASE_TRANSFORM:
		getWorld().setTimeEnabled(false);
		setMotionEnabled(false);
		setCollisionsEnabled(false);
		setAnimationPaused(true, getWorld().getFrameNumber());
		transformTimer = TRANSFORM_TIMER;
		invincibilityTimer = 0;
		break;

	case PHASE_WARP_ENTER:
		camera.setPosition(getCenterX(), getBottom());
	case PHASE_WARP_EXIT:
		getWorld().setTimeEnabled(false);
		if( heldSprite != nullptr )
		{
			heldSprite->setLayer(-1);
			setLayer(-2);
		}
		else
		{
			setLayer(-1);
		}

		setCollisionsEnabled(false);
		setXAcceleration(0.0);
		setYAcceleration(0.0);

		warpTimer = WARP_TIMER;

		playSound( "powerdown" );
		break;

	default:
		break;
	}

	this->phase = phase;
}

void Player::setReserveItem( State itemState )
{
	// Set the reserve item only if we need to
	bool setReserve = false;
	Item* reserve = nullptr;

	switch( itemState )
	{
		case SUPER:
			if( state >= SUPER && (reserveItem == nullptr || dynamic_cast<Mushroom*>(reserveItem) != nullptr ) )
			{
				reserve = new Mushroom;
				setReserve = true;
			}
			break;

		case FIRE:
			if( state > SUPER )
			{
				if( state == FIRE )
				{
					reserve = new Flower;
				}
				else if( state == TAIL )
				{
					reserve = new Leaf;
				}
				setReserve = true;
			}
			else if( state == SUPER )
			{
				reserve = new Mushroom;
				setReserve = true;
			}
			break;

		case TAIL:
			if( state > SUPER )
			{
				if( state == FIRE )
				{
					reserve = new Flower;
				}
				else if( state == TAIL )
				{
					reserve = new Leaf;
				}
				setReserve = true;
			}
			else if( state == SUPER )
			{
				reserve = new Mushroom;
				setReserve = true;
			}
			break;

		default:
			return;
	}

	if( setReserve )
	{
		if( reserveItem != nullptr )
		{
			delete reserveItem;
		}
		reserveItem = reserve;
		if( state == SUPER && itemState > SUPER )
		{
			playSound("powerup");
		}
		else
		{
			playSound("reserve_item");
		}
	}
	else
	{
		playSound("powerup");
	}
}

void Player::setState( State state, bool transformAnimation )
{
	// Compute animations
	loadAnimations(state);

	if( state != SMALL )
	{
		grow();
	}
	else
	{
		shrink();
	}

	if( state != TAIL )
	{
		flightTimer = 0;
	}

	// Activate the transformation animation if we are switching states
	if( transformAnimation && this->state != state )
	{
		transformationBeginState = this->state;
		transformationEndState = state;
		setPhase(PHASE_TRANSFORM);
	}

	this->state = state;
}

void Player::scorePoints( int points, double x, double y )
{
	scorePointsWithoutEffect(points);

	if( points < MINIMUM_SCORE_PARTICLE_POINTS )
	{
		return;
	}

	Sprite* text;
	switch( points )
	{
		case 100:
		case 200:
		case 400:
		case 800:
		case 1000:
		case 2000:
		case 4000:
		case 8000:
			{
				char animationName[32];
				sprintf( animationName, "score_%d", points );
				text = new Particle( getAnimation(animationName), TEXT_PARTICLE_DURATION );
			}
			break;

		default:
			{
				char scoreText[32];
				sprintf( scoreText, "%d", points );
				text = new TextParticle(scoreText, TEXT_PARTICLE_DURATION);
			}
			break;
	}

	if( x == 0.0 )
	{
		x = getCenterX();
	}
	if( y == 0.0 )
	{
		y = getCenterY();
	}

	text->setCollisionsEnabled(false);
	text->setGravityEnabled(false);
	text->setYVelocity(TEXT_PARTICLE_SPEED);
	text->setYAcceleration(TEXT_PARTICLE_GRAVITY);
	text->setLayer(1);
	getWorld().addSprite(x, y, text);
}

void Player::scorePointsWithoutEffect( int points )
{
	score += points;
}

void Player::shrink()
{
	setHeight(SMALL_HEIGHT);
	ducking = false;
}

void Player::takeDamage( bool fatal )
{
	if( dead || (!fatal && (invincibilityTimer != 0 || starTimer != 0)) )
	{
		return;
	}

	if( state == TAIL )
	{
		setState(SUPER, true);
	}
	else if( state == FIRE )
	{
		setState(SUPER, true);
	}
	else if( state == SUPER )
	{
		setState(SMALL, true);
		if( !fatal )
		{
			dropReserveItem();
		}
	}
	else
	{
		fatal = true;
	}

	if( fatal )
	{
		dead = true;
		releaseHeldSprite(false);
		setState(SMALL, false);
		lives--;
	}
	else
	{
		invincibilityTimer = INVINCIBILITY_DURATION;
		playSound("powerdown");
	}
}
