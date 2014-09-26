#include "Game.hpp"
#include "GameState.hpp"
#include "Globals.hpp"

Game::Game( GameState* initialState )
{
	pushState( initialState );
}

Game::~Game()
{
	for( std::list<GameState*>::iterator it = stateStack.begin(); it != stateStack.end(); ++it )
	{
		delete (*it);
	}

	for( std::list<GameState*>::iterator it = deadStateList.begin(); it != deadStateList.end(); ++it )
	{
		delete (*it);
	}
}

bool Game::isRunning() const
{
	return !(stateStack.empty());
}

void Game::popState()
{
	deadStateList.push_back( stateStack.back() );
	stateStack.pop_back();
	if( !stateStack.empty() )
	{
		stateStack.back()->onResume();
	}
}

void Game::pushState( GameState* state )
{
	stateStack.push_back(state);
	state->game = this;
}

void Game::quit()
{
	while( !stateStack.empty() )
	{
		popState();
	}
}

void Game::switchState( GameState* state )
{
	popState();
	pushState(state);
}

void Game::update()
{
	stateStack.back()->update();

	for( std::list<GameState*>::iterator it = deadStateList.begin(); it != deadStateList.end(); ++it )
	{
		delete (*it);
	}
	deadStateList.clear();

	if( stateStack.empty() )
	{
		return;
	}

	if( stateStack.back()->throttle )
	{
		FPS_MANAGER.update();
	}
}
