#ifndef MAINSTATE_HPP
#define MAINSTATE_HPP

#include "GameState.hpp"

class DeadPlayer;
class Player;
class World;

/**
 * The main state of the Game.
 */
class MainState : public GameState
{
public:
	MainState(int level);
	~MainState();

	void update();

private:
	World* world;
	bool commandMode;
	bool paused;
	std::string commandString;

	Player* player;
	DeadPlayer* deadPlayer;
	bool playerDeathHandled;

	int endTimer; /**< Counts down until we pop the current game state. */
	int initialEndTimer; /**< The initial value of the end timer when it was set. */
	int remainingTime; /**< Used when calculating the player's bonus from level time. */

	void executeCommand();
	void input();
	void render();
};

#endif // MAINSTATE_HPP
