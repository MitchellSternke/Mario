#include "Pipe.hpp"

Pipe::Pipe( Direction orientation ) :
	Tile( TILE_SOLID, true ),
	orientation( orientation )
{
}

Pipe::Pipe( Direction orientation, Location warpLocation ) :
	Tile( TILE_SOLID, true ),
	orientation( orientation ),
	warpLocation( warpLocation )
{
}

Direction Pipe::getOrientation() const
{
	return orientation;
}

const Location& Pipe::getWarpLocation() const
{
	return warpLocation;
}
