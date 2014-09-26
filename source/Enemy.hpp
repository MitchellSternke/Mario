#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Sprite.hpp"

static constexpr double BUMP_VELOCITY = 10.0; /**< The initial y velocity of a bumped enemy. */

/**
 * Base class for enemies of the player.
 */
class Enemy : public Sprite
{
public:
	Enemy();

	/**
	 * Bump an enemy from below.
	 */
	void bump();

	/**
	 * Hit the enemy with a shell (or another forceful object).
	 */
	void hit();

	/**
	 * Check if the enemy is resistant to fireballs.
	 */
	bool isFireproof() const;

	/**
	 * Check if an Enemy can be safely kicked from the side.
	 */
	bool isKickable() const;

	/**
	 * Check if an Enemy can be stomped (not necessarily if it will be
	 * dangerous if attempted).
	 */
	bool isStompable() const;

	/**
	 * Stomp an enemy from above.
	 */
	void stomp();

protected:
	virtual void onBump();
	virtual void onCollision(Sprite& sprite, Edge edge);
	virtual void onCollision(Tile& tile, Edge edge);
	virtual void onHit();
	virtual void onPreUpdate();
	void onRender();
	virtual void onStomp();
	void setBounceOffEnemiesEnabled( bool enabled );

	/**
	 * Set the default x velocity to set the enemy to if no movement is
	 * detected. This fixes some movement bugs, so be sure to use it
	 * when creating new enemies.
	 */
	void setDefaultXVelocity( double velocity );

	void setFireproof( bool fireproof );
	void setKickable( bool kickable );
	void setStompable( bool stompable );
	void setVelocityOrientationEnabled( bool enabled );

private:
	bool bounceOffEnemiesEnabled;
	bool fireproof;
	bool kickable;
	bool stompable;
	bool velocityOrientationEnabled;
	double defaultXVelocity;

	void createParticleAndDie();
};

#endif // ENEMY_HPP
