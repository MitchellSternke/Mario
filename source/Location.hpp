#ifndef LOCATION_HPP
#define LOCATION_HPP

/**
 * An exact location within the World.
 */
struct Location
{
	int levelId; /**< The Level id number. */
	int x;       /**< The x coordinate within the Level. */
	int y;       /**< The y coordinate within the Level. */

	Location();
};

static constexpr int CURRENT_LEVEL = -1; /**< The ID used to reference the current Level. */
static constexpr int NULL_LEVEL = 0;     /**< The ID used to reference no Level. */

#endif // LOCATION_HPP
