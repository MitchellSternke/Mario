#include "Player.hpp"
#include "Sprite.hpp"
#include "Tile.hpp"
#include "World.hpp"

Sprite::Sprite() :
	size(1.0, 1.0),
	slope(nullptr),
	exitedSlopeThisFrame(false),
	dead(false),
	flying(false),
	held(false),
	mirrored(false),
	deathBoundaryEnabled(true),
	gravityEnabled(true),
	holdingEnabled(false),
	motionEnabled(true),
	spriteCollisionsEnabled(true),
	tileCollisionsEnabled(true),
	holdingPlayer(nullptr)
{
	setMaximumYSpeed(DEFAULT_MAXIMUM_Y_SPEED);
	setLayer(0);
}

void Sprite::avoidCliffs()
{
	if( isFlying() || velocity.x == 0.0 )
	{
		return;
	}

	const Tile* tile = nullptr;
	if( velocity.x < 0.0 )
	{
		tile = getWorld().getTile(std::floor(getLeft()), std::floor(getBottom() - 0.5));
	}
	else if( velocity.x > 0.0 )
	{
		tile = getWorld().getTile(std::floor(getRight()), std::floor(getBottom() - 0.5));
	}

	if( tile == nullptr )
	{
		velocity.x *= -1.0;
	}
}

void Sprite::bounceOffWalls( Tile& tile, Edge edge )
{
	if( tile.getCollisionBehavior() == TILE_SOLID && (edge == EDGE_LEFT || edge == EDGE_RIGHT) )
	{
		if( (velocity.x > 0 && position.x + size.x < tile.getX() + tile.getWidth()) || (velocity.x < 0 && position.x > tile.getX()) )
		{
			velocity.x *= -1.0;
		}
	}
}

void Sprite::disableCollisions()
{
	setCollisionsEnabled(false);
}

void Sprite::disableGravity()
{
	gravityEnabled = false;
}

double Sprite::getHeight() const
{
	return size.y;
}

Player* Sprite::getHoldingPlayer()
{
	return holdingPlayer;
}

double Sprite::getMaximumXSpeed() const
{
	return maximumSpeed.x;
}

double Sprite::getMaximumYSpeed() const
{
	return maximumSpeed.y;
}

Tile* Sprite::getSlope()
{
	return slope;
}

double Sprite::getWidth() const
{
	return size.x;
}

double Sprite::getX() const
{
	return position.x;
}

double Sprite::getXAcceleration() const
{
	return acceleration.x;
}

double Sprite::getXVelocity() const
{
	return velocity.x;
}

double Sprite::getY() const
{
	return position.y;
}

double Sprite::getYAcceleration() const
{
	return acceleration.y;
}

double Sprite::getYVelocity() const
{
	return velocity.y;
}

bool Sprite::isFlying() const
{
	return flying;
}

bool Sprite::isHeld() const
{
	return held;
}

bool Sprite::isHoldingEnabled() const
{
	return holdingEnabled;
}

void Sprite::kill()
{
	dead = true;
	if( held )
	{
		holdingPlayer->releaseHeldSprite(false);
	}
}

void Sprite::setBottom( double y )
{
	setY( y );
}

void Sprite::setCenterX( double x )
{
	position.x = x - size.x / 2.0;
}

void Sprite::setCenterY( double y )
{
	position.y = y - size.y / 2.0;
}

void Sprite::setCollisionsEnabled( bool enabled )
{
	setSpriteCollisionsEnabled(enabled);
	setTileCollisionsEnabled(enabled);
}

void Sprite::setDeathBoundaryEnabled( bool enabled )
{
	deathBoundaryEnabled = enabled;
}

void Sprite::setFlying( bool flying )
{
	this->flying = flying;
}

void Sprite::setGravityEnabled( bool enabled )
{
	gravityEnabled = enabled;
}

void Sprite::setHeight( double height )
{
	size.y = height;
}

void Sprite::setHolding( bool held, Player* player )
{
	if( !held )
	{
		onRelease();
	}
	else
	{
		slope = nullptr;
	}

	this->held = held;
	holdingPlayer = player;
}

void Sprite::setHoldingEnabled( bool enabled )
{
	holdingEnabled = enabled;
}

void Sprite::setLayer( int layer )
{
	this->layer = layer * 2 + 1;
}

void Sprite::setLeft( double x )
{
	setX( x );
}

void Sprite::setMaximumXSpeed(double speed)
{
	maximumSpeed.x = std::fabs(speed);
}

void Sprite::setMaximumYSpeed(double speed)
{
	maximumSpeed.y = std::fabs(speed);
}

void Sprite::setMotionEnabled( bool enabled )
{
	motionEnabled = enabled;
}

void Sprite::setRight( double x )
{
	position.x = x - size.x;
}

void Sprite::setSpriteCollisionsEnabled( bool enabled )
{
	spriteCollisionsEnabled = enabled;
}

void Sprite::setTileCollisionsEnabled( bool enabled )
{
	tileCollisionsEnabled = enabled;
}

void Sprite::setTop( double y )
{
	position.y = y - size.y;
}

void Sprite::setWidth( double width )
{
	size.x = width;
}

void Sprite::setX( double x )
{
	position.x = x;
}

void Sprite::setXAcceleration(double a)
{
	acceleration.x = a;
}

void Sprite::setXVelocity(double v)
{
	velocity.x = v;
}

void Sprite::setY( double y )
{
	position.y = y;
}

void Sprite::setYAcceleration(double a)
{
	acceleration.y = a;
}

void Sprite::setYVelocity(double v)
{
	velocity.y = v;
}
