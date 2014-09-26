#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>

#include "Vector2.hpp"

/**
 * A cardinal direction.
 */
enum Direction
{
	DOWN,  /**< Down. */
	LEFT,  /**< Left. */
	RIGHT, /**< Right. */
	UP     /**< Up. */
};

/// @todo repurpose this in certain use cases
enum Edge
{
	EDGE_BOTTOM,
	EDGE_LEFT,
	EDGE_RIGHT,
	EDGE_TOP
};

/**
 * Get a direction constant from a string value.
 */
Direction getDirection( const std::string& name );

Direction getOppositeDirection( Direction direction );
Edge getOppositeEdge( Edge edge );

/**
 * Get a unit vector in the direction specified.
 */
Vector2<int> getVector( Direction direction );

/**
 * Check if a Direction is horizontal (LEFT or RIGHT).
 */
bool isHorizontal( Direction direction );

/**
 * Check if a Direction is vertical (UP or DOWN).
 */
bool isVertical( Direction direction );

#endif // ENUMS_HPP
