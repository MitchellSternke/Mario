#include "Episode.hpp"
#include "GameSession.hpp"
#include "Player.hpp"
#include "World.hpp"

GameSession::~GameSession()
{
	delete episode;
	delete player;
	delete world;
}
