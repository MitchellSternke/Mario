#ifndef GAME_HPP
#define GAME_HPP

#include <list>

class GameState;

/**
 * A class that manages GameStates and execution flow.
 */
class Game
{
public:
	/**
	 * Create the Game with an initial state.
	 *
	 * @param initialState the initial GameState to execute.
	 */
	Game( GameState* initialState );

	~Game();

	/**
	 * Check if the Game is still running.
	 *
	 * @return whether there is a GameState on the stack.
	 */
	bool isRunning() const;

	/**
	 * Pop the current GameState off the stack.
	 */
	void popState();

	/**
	 * Push a GameState onto the stack. The current GameState remains paused
	 * until this new state is popped.
	 *
	 * @param state the GameState to push.
	 */
	void pushState( GameState* state );

	/**
	 * Pop all states off the stack and end execution.
	 */
	void quit();

	/**
	 * Switch the GameState to a different state. This is equivalent to popping
	 * and then pushing the new state to the stack.
	 *
	 * @param state the GameState to switch to.
	 */
	void switchState( GameState* state );

	/**
	 * Update the Game (run one iteration of the loop in the current state).
	 */
	void update();

private:
	std::list<GameState*> deadStateList;
	std::list<GameState*> stateStack;
};

#endif // GAME_HPP
