#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

class Game;

/**
 * A state of the Game. This could be a title screen, menu, the main game
 * itself, etc.
 */
class GameState
{
	friend class Game;
public:
	/**
	 * Create a new GameState.
	 *
	 * @param throttle whether the fps of the game state should be throttled.
	 */
	GameState( bool throttle = false );

	virtual ~GameState() {}

	/**
	 * Called each time a state is expected to be executed.
	 */
	virtual void update()=0;

protected:
	/**
	 * Get the Game managing the GameState.
	 *
	 * @return the Game object.
	 */
	Game& getGame();

	/**
	 * Event called when the game state is resumed after it reaches the top of
	 * the stack.
	 */
	virtual void onResume() {}

private:
	Game* game;
	bool throttle;
};

#endif // GAMESTATE_HPP
