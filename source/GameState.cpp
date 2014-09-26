#include "Game.hpp"
#include "GameState.hpp"

GameState::GameState( bool throttle ) :
	throttle(throttle)
{
}

Game& GameState::getGame()
{
	return *game;
}
