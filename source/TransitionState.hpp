#ifndef TRANSITIONSTATE_HPP
#define TRANSITIONSTATE_HPP

#include "GameState.hpp"

/**
 * The transition between the map and the main game.
 */
class TransitionState : public GameState
{
public:
	/**
	 * Create a new transition state.
	 *
	 * @param levelId the ID of the level to start in a MainState once the transition is over.
	 */
	TransitionState( int levelId );

	void update();

private:
	int levelId;
	int progress;

	void onResume();
};

#endif // TRANSITIONSTATE_HPP
