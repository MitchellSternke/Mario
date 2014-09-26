#ifndef HILLYLEVELGENERATOR_HPP
#define HILLYLEVELGENERATOR_HPP

#include "../LevelGenerator.hpp"

/**
 * Generates hilly levels.
 */
class HillyLevelGenerator : public LevelGenerator
{
public:
	Level* generateLevel( int seed ) const;
};

#endif // HILLYLEVELGENERATOR_HPP
