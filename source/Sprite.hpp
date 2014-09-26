#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <list>

#include "Enums.hpp"
#include "Entity.hpp"
#include "Vector2.hpp"

class Player;
class Tile;
class World;

/**
 * An Entity that moves.
 */
class Sprite : public Entity
{
	friend class World;
public:
	Sprite();
	virtual ~Sprite() {}

	/**
	 * Disable collision checking for the sprite.
	 */
	void disableCollisions();

	/**
	 * Disable gravity for the sprite.
	 * @todo deprecate this.
	 */
	void disableGravity();

	double getHeight() const;

	/**
	 * Get the maximum x speed of the sprite.
	 */
	double getMaximumXSpeed() const;

	/**
	 * Get the maximum y speed of the sprite.
	 */
	double getMaximumYSpeed() const;

	double getWidth() const;

	double getX() const;

	/**
	 * Get the x acceleration of the sprite.
	 */
	double getXAcceleration() const;

	/**
	 * Get the x velocity of the sprite.
	 */
	double getXVelocity() const;

	double getY() const;

	/**
	 * Get the y acceleration of the sprite.
	 */
	double getYAcceleration() const;

	/**
	 * Get the y velocity of the sprite.
	 */
	double getYVelocity() const;

	/**
	 * Returns true if the sprite is not on the ground.
	 */
	bool isFlying() const;

	/**
	 * Returns true if a Player is holding the Sprite.
	 */
	bool isHeld() const;

	/**
	 * Returns true if the Sprite can be held by a Player.
	 */
	bool isHoldingEnabled() const;

	/**
	 * Destroy the sprite.
	 */
	void kill(); ///@todo rename to destroy()

	/**
	 * Set the bottom coordinate of the sprite.
	 */
	void setBottom( double y );

	/**
	 * Set the center x coordinate of the sprite.
	 */
	void setCenterX( double x );

	/**
	 * Set the center y coordinate of the sprite.
	 */
	void setCenterY( double y );

	/**
	 * Enable/disable collisions for the Sprite.
	 */
	void setCollisionsEnabled( bool enabled );

	/**
	 * Enable/disable automatic destruction of the sprite if it is out of bounds.
	 */
	void setDeathBoundaryEnabled( bool enabled );

	/**
	 * Enable/disable gravity for the Sprite.
	 */
	void setGravityEnabled( bool enabled );

	/**
	 * Set the height of the sprite.
	 */
	void setHeight( double height );

	/**
	 * Set the state of the Sprite being held by the Player.
	 */
	void setHolding( bool held, Player* player = nullptr );

	void setLayer( int layer );

	/**
	 * Set the left coordinate of the sprite.
	 */
	void setLeft( double x );

	/**
	 * Set the maximum x speed of the sprite.
	 */
	void setMaximumXSpeed( double speed );

	/**
	 * Set the maximum y speed of the sprite.
	 */
	void setMaximumYSpeed( double speed );

	/**
	 * Set the right coordinate of the sprite.
	 */
	void setRight( double x );

	/**
	 * Set the top coordinate of the sprite.
	 */
	void setTop( double y );

	/**
	 * Set the width of the sprite.
	 */
	void setWidth( double width );

	/**
	 * Set the x position of the sprite.
	 */
	void setX( double x );

	/**
	 * Set the x acceleration of the sprite.
	 */
	void setXAcceleration( double a );

	/**
	 * Set the x velocity of the sprite.
	 */
	void setXVelocity( double v );

	/**
	 * Set the y position of the sprite.
	 */
	void setY( double y );

	/**
	 * Set the y acceleration of the sprite.
	 */
	void setYAcceleration( double a );

	/**
	 * Set the y velocity of the sprite.
	 */
	void setYVelocity( double v );

protected:
	static constexpr double DEFAULT_GRAVITY = -1.0 * static_cast<double>(0x0050) / static_cast<double>(0x1000) * 60.0 * 60.0; /**< The default gravitational acceleration for sprites. */
	static constexpr double DEFAULT_MAXIMUM_Y_SPEED = static_cast<double>(0x0450) / static_cast<double>(0x1000) * 60.0;

	/**
	 * Handle collision detection logic for avoiding falling off cliffs. Put
	 * this in the onPreUpdate() function for proper usage.
	 */
	void avoidCliffs();

	void bounceOffWalls(Tile& tile, Edge edge);

	/**
	 * Get the Player that is holding the Sprite.
	 */
	Player* getHoldingPlayer();

	/**
	 * Get the slope that the sprite is on.
	 */
	Tile* getSlope();

	/**
	 * Called when the Sprite collides with another Sprite.
	 * @todo deprecate the edge parameter.
	 */
	virtual void onCollision(Sprite& sprite, Edge edge) {}

	/**
	 * Called when the Sprite collides with a Tile.
	 * @todo deprecate the edge parameter.
	 */
	virtual void onCollision(Tile& tile, Edge edge) {}

	/**
	 * Called when the Sprite is destroyed in the World for any reason.
	 */
	virtual void onDestroy() {}

	/**
	 * Called when the Sprite touches the ground after being in the air.
	 */
	virtual void onGround() {}

	/**
	 * Called when the Sprite is added to the World.
	 */
	virtual void onInit() {}

	/**
	 * Called after the Sprite is updated for a frame.
	 */
	virtual void onPostUpdate() {}

	/**
	 * Called before the Sprite is updated for a frame.
	 */
	virtual void onPreUpdate() {}

	/**
	 * Called when the Sprite is released from holding by a Player.
	 */
	virtual void onRelease() {}

	/**
	 * Set whether the sprite is currently flying (off the ground).
	 */
	void setFlying( bool flying );

	/**
	 * Enable/disable holding for the Sprite by Players.
	 */
	void setHoldingEnabled( bool enabled );

	/**
	 * Enable/disable Sprite movement.
	 */
	void setMotionEnabled( bool enabled );

	/**
	 * Enable/disable collisions with sprites.
	 */
	void setSpriteCollisionsEnabled( bool enabled );

	/**
	 * Enable/disable collisions with tiles.
	 */
	void setTileCollisionsEnabled( bool enabled );

private:
	Vector2<double> position;
	Vector2<double> velocity;
	Vector2<double> acceleration;
	Vector2<double> size;

	Vector2<double> maximumSpeed;

	std::list< Vector2<int> > occupiedCells; /**< Cells that the Sprite is on. */
	Tile* slope; /**< The slope the sprite is on (if any). */
	bool exitedSlopeThisFrame; /**< Whether the sprite just exited a slope tile on the current frame. */

	bool dead; /**< Whether the Sprite is dead and should be destroyed. */
	bool flying;
	bool held; /**< Whether the Sprite is being held by a player or not. */
	bool mirrored; /**< Whether the current animation is mirrored along the y-axis or not. */

	bool deathBoundaryEnabled;
	bool gravityEnabled;
	bool holdingEnabled; /**< Whether the Sprite can be held by a player. */
	bool motionEnabled; /**< Whether the Sprite will undergo motion. */
	bool spriteCollisionsEnabled;
	bool tileCollisionsEnabled;

	Player* holdingPlayer; /**< The player currently holding the Sprite. */
};

#endif // SPRITE_HPP
