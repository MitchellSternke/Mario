#ifndef GAMESESSION_HPP
#define GAMESESSION_HPP

class Episode;
class Player;
class World;

/**
 * A session of the game.
 */
struct GameSession
{
	Episode* episode;
	Player* player;
	World* world;

	~GameSession();
};

#endif // GAMESESSION_HPP
