#include "Episode.hpp"
#include "Exception.hpp"
#include "Globals.hpp"
#include "Level.hpp"

Episode::~Episode()
{
	for( auto it : levels )
	{
		delete it.second;
	}
}

void Episode::addLevel( int levelId, Level* level )
{
	auto it = levels.find( levelId );
	if( it == levels.end() )
	{
		levels[levelId] = level;
	}
	else
	{
		LOG << "Warning: tried to add a level to an episode using an ID that already existed.\n";
	}
}

const Level* Episode::getLevel( int levelId )
{
	auto it = levels.find( levelId );
	if( it != levels.end() )
	{
		Level* level = (*it).second;
		if( level != nullptr )
		{
			return level;
		}
		else
		{
			throw Exception() << "Invalid level ID [" << levelId << "] passed to Episode::getLevel()";
		}
	}
	else
	{
		throw Exception() << "Invalid level ID [" << levelId << "] passed to Episode::getLevel()";
	}
}
