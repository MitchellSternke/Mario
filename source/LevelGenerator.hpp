#ifndef LEVELGENERATOR_HPP
#define LEVELGENERATOR_HPP

// These are included for convenience
#include "Level.hpp"
#include "Random.hpp"

class LevelTheme;

/**
 * Generates a Level given a seed.
 */
class LevelGenerator
{
public:
	virtual ~LevelGenerator() {}

	/**
	 * Generate a random Level.
	 *
	 * @param seed the seed to use.
	 */
	virtual Level* generateLevel( int seed ) const =0;
};

#endif // LEVELGENERATOR_HPP
