#ifndef MAPSTATE_HPP
#define MAPSTATE_HPP

#include "GameState.hpp"

class Map;
class World;

/**
 * The game state that handles the player interacting with the world map.
 */
class MapState : public GameState
{
public:
	MapState();
	~MapState();

	void update();

private:
	Map* map;
	int node;
	int fadeInProgress;
	int fadeOutProgress;

	void input();
	void onResume();
	void render() const;
};

#endif // MAPSTATE_HPP
