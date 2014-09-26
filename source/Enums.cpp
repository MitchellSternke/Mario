#include "Enums.hpp"
#include "StringSwitch.hpp"

static bool directionSwitchInitialized = false;
static StringSwitch directionNameSwitch;

static void initializeDirectionSwitch()
{
	directionNameSwitch.addString( "down", DOWN );
	directionNameSwitch.addString( "left", LEFT );
	directionNameSwitch.addString( "right", RIGHT );
	directionNameSwitch.addString( "up", UP );
	directionNameSwitch.setDefaultValue(UP);
	directionSwitchInitialized = true;
}

Direction getDirection( const std::string& name )
{
	if( !directionSwitchInitialized )
	{
		initializeDirectionSwitch();
	}

	return static_cast<Direction>(directionNameSwitch.test(name));
}

Direction getOppositeDirection(Direction direction)
{
	switch( direction )
	{
		case DOWN:
			return UP;
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
		case UP:
			return DOWN;
	}

	// Default; suppresses warning
	return UP;
}

Edge getOppositeEdge(Edge edge)
{
	switch( edge )
	{
		case EDGE_BOTTOM:
			return EDGE_TOP;
		case EDGE_LEFT:
			return EDGE_RIGHT;
		case EDGE_RIGHT:
			return EDGE_LEFT;
		case EDGE_TOP:
			return EDGE_BOTTOM;
	}

	// Default; suppresses warning
	return EDGE_TOP;
}

Vector2<int> getVector( Direction direction )
{
	Vector2<int> v;

	switch( direction )
	{
	case DOWN:
		v.y = -1;
		break;
	case LEFT:
		v.x = -1;
		break;
	case RIGHT:
		v.x = 1;
		break;
	case UP:
		v.y = 1;
		break;
	}

	return v;
}

bool isHorizontal(Direction direction)
{
	return (direction == LEFT || direction == RIGHT);
}

bool isVertical(Direction direction)
{
	return (direction == DOWN || direction == UP);
}
