#ifndef SHELL_HPP
#define SHELL_HPP

#include "Enemy.hpp"

class Player;

/**
 * A Koopa shell.
 */
class Shell : public Enemy
{
public:
	static constexpr double WIDTH = 0.75;  /**< The width of all shells. */
	static constexpr double HEIGHT = 0.75; /**< The height of all shells. */

	static constexpr double MOVEMENT_SPEED = 12.0; /**< The speed that shells move at when kicked. */

	static constexpr int KICK_TIMER = 10; /**< Number of frames after a shell is kicked that it cannot be stomped. */

	Shell( bool fireproof = false );

	/**
	 * Get the player that last kicked the shell.
	 */
	Player* getPlayer();

private:
	int kickTimer; /**< Timer set after a shell is kicked to prevent stomping immediately until the shell has moved out of range. */
	int kills; /**< Number of kills accumulated since the shell was kicked by a player. */
	Player* player; /**< The player that last kicked the shell. */

	void onCollision(Sprite& sprite, Edge edge);
	void onCollision(Tile& tile, Edge edge);
	void onPreUpdate();
	void onRelease();
	void onStomp();
	void startMoving( Player* kicker );
	void stopMoving();
};

#endif // SHELL_HPP
