#ifndef INFINITYSTATE_HPP
#define INFINITYSTATE_HPP

#include "GameState.hpp"

/**
 * The game state that manages infinity mode.
 */
class InfinityState : public GameState
{
public:
	InfinityState();
	~InfinityState();

	void update();

private:
	int fadeInProgress;
	int fadeOutProgress;
	int levelNumber;

	void generateNewLevel();
	void input();
	void onResume();
	void render();
};

#endif // INFINITYSTATE_HPP
