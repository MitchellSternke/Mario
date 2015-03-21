#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Camera.hpp"
#include "InputManager.hpp"
#include "Location.hpp"
#include "Sprite.hpp"

class Item;

/**
 * A Player of the Game.
 */
class Player : public Sprite, public InputManager::ControllerListener
{
	friend class Fireball;
	friend class Sprite;
public:
	/**
	 * The current mode of operation for the player.
	 */
	enum Phase
	{
		PHASE_DEFAULT,    /**< Default phase. Player behaves as normal. */

		PHASE_DEAD,       /**< The player has died. */
		PHASE_TRANSFORM,  /**< Changing state from a powerup or hit from an enemy. */
		PHASE_WARP_ENTER, /**< Entering via warp pipe. */
		PHASE_WARP_EXIT,  /**< Exiting via warp pipe. */
	};

	/**
	 * The powerup state that the player can be in.
	 */
	enum State
	{
		SMALL, /**< Small size. */
		SUPER, /**< Super size. */
		FIRE,  /**< Fireball ability active. */
		TAIL   /**< Player has tail for attacking enemies and for flight. */
	};

	static constexpr int P_METER_FULL_BARS = 7; /**< The number of bars in a full p-meter. */
	static constexpr int P_METER_FULL_FRAMES = 112; /**< The number of frames in a full p-meter. */

	Player( int controllerPort );

	~Player();

	/**
	 * Drop the reserve item.
	 */
	void dropReserveItem();

	/**
	 * Have the player gain coin(s).
	 *
	 * @param coins the number of Coin(s) to gain.
	 */
	void gainCoins( int coins );

	/**
	 * Have the player gain live(s).
	 *
	 * @param lives the number of lives to gain.
	 */
	void gainLives( int lives, double x = 0.0, double y = 0.0 );

	/**
	 * Get the camera used to display the player.
	 */
	const Camera& getCamera() const;

	/**
	 * Get the number of coins that the player currently has.
	 */
	int getCoins() const;

	/**
	 * Get the number of lives that the player currently has.
	 */
	int getLives() const;

	/**
	 * Get the number of filled bars on the p-meter.
	 */
	int getPMeterBars() const;

	/**
	 * Get the reserve item stored by the player.
	 */
	Item* getReserveItem();

	/**
	 * Get the player's current score.
	 */
	int getScore() const;

	/**
	 * Get the current state of the player.
	 */
	State getState() const;

	/**
	 * Whether the player is flying up in tail mode.
	 */
	bool isAscending() const;

	/**
	 * Check whether the player is currently climbing a ladder tile.
	 */
	bool isClimbingLadder() const;

	/**
	 * Check if the player has died.
	 */
	bool isDead() const;

	/**
	 * Whether the player is slowly descending in tail mode.
	 */
	bool isDescending() const;

	/**
	 * Returns true if the Player is holding a Sprite currently.
	 */
	bool isHoldingSprite() const;

	/**
	 * Returns true if the Player cannot be hurt.
	 */
	bool isInvincible() const;

	/**
	 * Returns true if the Player's p-meter is filled.
	 */
	bool isPMeterFilled() const;

	/**
	 * Returns true if the Player is underwater.
	 */
	bool isUnderwater() const;

	/**
	 * Returns true if the Player has a star power up.
	 */
	bool hasStar() const;

	/**
	 * Reset the player after removing it from the world.
	 */
	void reset();

	/**
	 * Have the player score points.
	 *
	 * @param points the number of points to score.
	 */
	void scorePoints( int points, double x = 0.0, double y = 0.0 );

	/**
	 * Have the player score points without creating particle effects, ever.
	 *
	 * @param points the number of points to score.
	 */
	void scorePointsWithoutEffect( int points );

	/**
	 * Have the player take one unit of damage.
	 *
	 * @param fatal whether the damage should cause instant death.
	 */
	void takeDamage( bool fatal = false );

///@todo we no longer need a protected section
protected:
	static constexpr double EPSILON = static_cast<double>(0x0010) / static_cast<double>(0x1000) * 60.0;

	static constexpr double FPS = 60.0;

	static constexpr double SMALL_HEIGHT = 0.8;
	static constexpr double SUPER_HEIGHT = 1.6;
	static constexpr double WIDTH = 0.75;

	static constexpr int    P_METER_GAIN_FRAMES = 8;
	static constexpr int    P_METER_LOSS_FRAMES = 24;

	static constexpr int    FLIGHT_DURATION = 255; /**< Number of frames of flight duration. */

///@todo figure out c++11 magic to make this not a macro
#define VELOCITY( x ) (static_cast<double>(x) / static_cast<double>(0x1000) * FPS)
#define ACCELERATION( x ) (VELOCITY(x) * FPS)

	static constexpr double GROUND_ACCELERATION = ACCELERATION(0x000e);
	static constexpr double GROUND_DECELERATION = GROUND_ACCELERATION;
	static constexpr double GROUND_SKID_DECELERATION = ACCELERATION(0x0020);
	static constexpr double AIR_ACCELERATION = ACCELERATION(0x000e);
	static constexpr double AIR_DECELERATION = ACCELERATION(0x0020);

	static constexpr double MAX_WALK_VELOCITY = VELOCITY(0x0180);
	static constexpr double MAX_RUN_VELOCITY = VELOCITY(0x0280);
	static constexpr double MAX_SPRINT_VELOCITY = VELOCITY(0x0380);

	static constexpr double MAX_UPHILL_WALK_VELOCITY = VELOCITY(0x00d0);
	static constexpr double MAX_UPHILL_RUN_VELOCITY = VELOCITY(0x0160);

	static constexpr double ADDED_DOWNHILL_VELOCITY_HALF_SLOPE = VELOCITY(0x0020);
	static constexpr double ADDED_DOWNHILL_VELOCITY_FULL_SLOPE = VELOCITY(0x0030);

	static constexpr double SLIDING_ACCELERATION_HALF_SLOPE = ACCELERATION(0x0010);
	static constexpr double SLIDING_ACCELERATION_FULL_SLOPE = ACCELERATION(0x0020);
	static constexpr double SLIDING_MAX_SPEED = VELOCITY(0x03f0);

	static constexpr double JUMP_THRESHOLD_0 = VELOCITY(0x0100);
	static constexpr double JUMP_THRESHOLD_1 = VELOCITY(0x0200);
	static constexpr double JUMP_THRESHOLD_2 = VELOCITY(0x0300);

	static constexpr double JUMP_VELOCITY_0 = VELOCITY(0x0370);
	static constexpr double JUMP_VELOCITY_1 = VELOCITY(0x0390);
	static constexpr double JUMP_VELOCITY_2 = VELOCITY(0x03b0);
	static constexpr double JUMP_VELOCITY_3 = VELOCITY(0x03f0);

	static constexpr double JUMP_GRAVITY_0 = ACCELERATION(0x0010);
	static constexpr double JUMP_GRAVITY_1 = ACCELERATION(0x0050);

	static constexpr double JUMP_GRAVITY_THRESHOLD = VELOCITY(0x0200);

	static constexpr double MAX_JUMP_VELOCITY = VELOCITY(0x0450);

	static constexpr double STOMP_VELOCITY = VELOCITY(0x0400);

	static constexpr double MAX_UNDERWATER_WALK_SPEED = VELOCITY(0x0100);
	static constexpr double MAX_SWIM_SPEED = VELOCITY(0x0300);

	static constexpr double SWIM_X_ACCELERATION = ACCELERATION(0x0006);
	static constexpr double SWIM_X_DECELERATION = ACCELERATION(0x0002);
	static constexpr double SWIM_X_ACCELERATION_REVERSE = ACCELERATION(0x0008);

	static constexpr double SWIM_Y_ACCELERATION_UP = ACCELERATION(0x0010);
	static constexpr double SWIM_Y_ACCELERATION_DOWN = ACCELERATION(0x0008);
	static constexpr double SWIM_Y_ACCELERATION_SURFACE = ACCELERATION(0x000c);

	static constexpr double MAX_SWIM_FALL_SPEED = VELOCITY(0x0200);

	static constexpr double SWIM_POWER_FACTOR = VELOCITY(0x01f0);

	static constexpr double SWIM_JUMP_SPEED = VELOCITY(0x0330);

	static constexpr double FLIGHT_VELOCITY = VELOCITY(0x0180);
	static constexpr double DESCENT_VELOCITY = VELOCITY(0x0100);

	static constexpr double CLIMB_SPEED = 4.0;

	static constexpr int    TRANSFORM_TIMER = 40; /**< The number of frames for a transformation to take. */
	static constexpr int    WARP_TIMER  = 40; /**< The number of frames for a warp entrance/exit to take. */

	const Animation* stillAnimation;
	const Animation* walkAnimation;
	const Animation* runAnimation;
	const Animation* sprintAnimation;
	const Animation* jumpAnimation;
	const Animation* fallAnimation;
	const Animation* turnAnimation;
	const Animation* throwAnimation;
	const Animation* duckAnimation;
	const Animation* carryStillAnimation;
	const Animation* carryWalkAnimation;
	const Animation* carryRunAnimation;
	const Animation* carrySprintAnimation;
	const Animation* carryJumpAnimation;
	const Animation* kickAnimation;
	const Animation* slideAnimation;
	const Animation* swimAnimation;
	const Animation* swimMoveAnimation;
	const Animation* sprintJumpAnimation;
	const Animation* climbAnimation;
	const Animation* climbMoveAnimation;
	const Animation* frontAnimation;
	const Animation* flightAnimation;
	const Animation* descentAnimation;
	const Animation* spinAnimation;

	bool isDucking() const;
	bool isSkidding() const;
	bool isSliding() const;

private:
	double airMomentum; /**< Maximum midair velocity. */
	Camera camera;
	bool cameraLocked; /**< Whether the y position of the camera is locked. */
	int cameraOrientation; /**< -1 for left, 1 for right. */
	bool climbing; /**< Whether the player is currently climbing. */
	int coins;
	InputManager::Controller* controller;
	bool dead; /**< Whether the player is dead or not. */
	bool ducking; /**< Whether the player is ducking or not. */
	int fireballCount; /**< Number of active thrown fireballs. */
	int flightTimer; /**< Number of frames remaining for leaf flight. */
	Sprite* heldSprite; /**< The Sprite being held by the Player. */
	int invincibilityTimer; /**< The number of frames that the player has left for invincibility. */
	int lives;
	int orientation; /**< -1 for left, 1 for right. */
	double orientationChangePosition; /**< The x coordinate where the player last changed orientation. */
	Phase phase;
	int pMeter; /**< The number of frames in the p-meter.*/
	Item* reserveItem;
	int score;
	int stompCount; /**< Counter for when the player is stomping multiple enemies. */
	bool skidding; /**< Whether the player is skidding on the ground or not. */
	bool sliding; /**< Whether the player is sliding down a hill or not. */
	int slideKills; /**< The number of kills that the player has accumulated from sliding. */
	int starKills; /**< The number of kills that the player has accumulated from using a star. */
	int starTimer; /**< The number of frames remaining from an invincibility star. */
	State state;
	State transformationBeginState;
	State transformationEndState;
	int transformTimer; /**< The number of frames remaining in a transformation. */
	Location warpDestination; /**< The location we are about to warp to. */
	int warpTimer; /**< The time until a warp phase (enter/exit) completes. */

	void attemptWarp( Direction direction );
	void decrementFireballCount();
	void grow();
	void holdSprite( Sprite* sprite );
	bool isClimbing(); /**< @todo this should be renamed since it is misleading. */
	bool isSolidTileAbove() const;
	void loadAnimations( State state );
	void onButtonToggle( InputButton button, bool pressed );
	void onCollision(Sprite& sprite, Edge edge);
	void onGround();
	void onPostUpdate();
	void onPreUpdate();
	void phaseTransform();
	void phaseWarpEnter();
	void phaseWarpExit();
	void releaseHeldSprite( bool kickIfShell );
	void setPhase( Phase phase );
	void setReserveItem( State itemState );
	void setState( State state, bool transformAnimation = false );
	void shrink();
	void updateCamera();
};

#endif // PLAYER_HPP
