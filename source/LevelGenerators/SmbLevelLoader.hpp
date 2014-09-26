#ifndef SMBLEVELLOADER_HPP
#define SMBLEVELLOADER_HPP

#include "../LevelGenerator.hpp"

/**
 * Loads levels from the original SMB.
 */
class SmbLevelLoader : public LevelGenerator
{
public:
	Level* generateLevel( int seed ) const;
};

#endif // SMBLEVELLOADER_HPP
