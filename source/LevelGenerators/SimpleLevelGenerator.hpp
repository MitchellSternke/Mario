#ifndef SIMPLELEVELGENERATOR_HPP
#define SIMPLELEVELGENERATOR_HPP

#include "../LevelGenerator.hpp"

/**
 * Generates simple levels with flat ground.
 */
class SimpleLevelGenerator : public LevelGenerator
{
public:
	Level* generateLevel( int seed ) const;
};

#endif // SIMPLELEVELGENERATOR_HPP
