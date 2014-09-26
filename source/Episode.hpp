#ifndef EPISODE_HPP
#define EPISODE_HPP

#include <map>

class Level;
class LevelGenerator;
struct LevelTheme;

/**
 * A playable episode of the game, consisting of Levels and Maps.
 */
class Episode
{
public:
	~Episode();

	/**
	 * Add a level to the Episode.
	 *
	 * @param levelId the ID number of the Level.
	 * @param level the Level to add.
	 */
	void addLevel( int levelId, Level* level );

	/**
	 * Get the Level from the Episode by its ID.
	 *
	 * @param levelID the ID of the Level.
	 */
	const Level* getLevel( int levelId );

private:
	std::map< int, Level* > levels;
};

#endif // EPISODE_HPP
