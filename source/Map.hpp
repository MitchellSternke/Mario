#ifndef MAP_HPP
#define MAP_HPP

#include <list>
#include <vector>

#include "Enums.hpp"
#include "Types.hpp"

class Animation;

/**
 * A map that allows access to levels in the world.
 */
class Map
{
public:
	/**
	 * Types of paths.
	 */
	enum PathType
	{
		PATH_WALK,  /**< The path can be traversed on foot. */
		PATH_CLIMB, /**< The path can be climbed. */
		PATH_SWIM   /**< The path can be swam. */
	};

	/**
	 * Create a new Map.
	 *
	 * @param width the width of the map, in tiles.
	 * @param height the height of the map, in tiles.
	 */
	Map( int width, int height );

	/**
	 * Add a new event to the map.
	 *
	 * @param node the id of the node to add the event for.
	 * @param path the path that the event will open up.
	 * @param frames the number of frames in the event.
	 * @param exit the exit number on the node that will trigger the event.
	 * @return the id of the new event.
	 */
	int createEvent( int node, int path, int frames, int exit = 0 );

	/**
	 * Create a tile in an event frame.
	 *
	 * @param event the id of the event.
	 * @param eventFrame the frame of the event.
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param animation the animation for the tile.
	 * @param animationFrame the frame of the animation to use.
	 * @note use -1 to animate the entire animation instead of using a single frame.
	 */
	void createEventTile( int event, int eventFrame, int x, int y, const Animation* animation, int animationFrame );

	/**
	 * Add a new node to the map.
	 *
	 * @param x the left coordinate.
	 * @param y the bottom coordinate.
	 * @param levelId the id of the level to have access to, if any.
	 * @return the id of the new node.
	 */
	int createNode(int x, int y, int levelId);

	/**
	 * Add a new path to the map.
	 *
	 * @param node1 the first node on the path.
	 * @param node2 the second node on the path.
	 * @return the id of the new path.
	 */
	int createPath(int node1, Direction direction1, int node2, Direction direction2);

	/**
	 * Add a segment to an existing path.
	 */
	void createPathSegment( int path, PathType type, double x1, double y1, double x2, double y2 );

	/**
	 * Get the ID of the level that the player can currently enter (if possible).
	 */
	int getLevelId() const;

	/**
	 * Get the ID of the node that the player is currently on.
	 */
	int getNode() const;

	/**
	 * Get the x position of the player.
	 */
	double getPlayerX() const;

	/**
	 * Get the y position of the player.
	 */
	double getPlayerY() const;

	/**
	 * Returns true if the map is currently playing an event.
	 */
	bool isPlayingEvent() const;

	/**
	 * Play an event.
	 */
	void playEvent( int event );

	/**
	 * Play an event associated with a node.
	 */
	void playEvent( int node, int eventNumber );

	/**
	 * Render the Map.
	 *
	 * @param viewX the center x coordinate.
	 * @param viewY the center y coordinate.
	 * @param viewWidth the width to display.
	 * @param viewHeight the height to display.
	 */
	void render(double viewX, double viewY, double viewWidth, double viewHeight) const;

	/**
	 * Set the player's current position on the map to a node.
	 */
	void setPlayerPosition( int node );

	/**
	 * Set the tile at a position of the world.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param animation the animation.
	 * @param frame the frame of the animation to use.
	 * @note if frame == -1, then the tile will be animated.
	 */
	void setTile( int x, int y, const Animation* animation, int frame = -1 );

	/**
	 * Call this every frame to update map processing.
	 */
	void update();

private:
	/**
	 * An individual tile of an EventFrame.
	 */
	struct EventTile
	{
		int x;
		int y;
		const Animation* animation;
		int animationFrame;
	};

	/**
	 * An individual frame of an event.
	 */
	struct EventFrame
	{
		std::list<EventTile> tiles;
	};

	/**
	 * A change in the map that occurs as a result of the player completing a level.
	 */
	struct Event
	{
		std::vector<EventFrame> frames;
		int path;
	};

	/**
	 * A node that can store a level or just serve as a junction.
	 */
	struct Node
	{
		int x;
		int y;
		int levelId;
		int paths[4];
		int events[4];

		Node();
	};

	/**
	 * A piece of a path.
	 */
	struct PathSegment
	{
		PathType type;
		double x1;
		double y1;
		double x2;
		double y2;
	};

	/**
	 * A path between two nodes.
	 */
	struct Path
	{
		int nodes[2];
		std::vector<PathSegment> segments;
		bool active;
	};

	/**
	 * A cell in the grid that makes up the map.
	 */
	struct Cell
	{
		const Animation* animation; /**< The animation used for terrain. */
		int animationFrame;         /**< The particular frame used from the animation. */
		int node;                   /**< The id of the node on the cell (if any). */

		Cell();
	};

	int width;
	int height;

	std::vector<Cell> cells;
	std::vector<Path> paths;
	std::vector<Node> nodes;
	std::vector<Event> events;

	int currentEvent; /**< The currently active event. */
	int currentEventFrame;
	int currentEventProgress;

	double playerX;
	double playerY;
	int playerNode; /**< The node that the player is currently on. */
	int playerPath; /**< The path that the player is currently on. */
	int playerPathSegment; /**< The of the path that the player is currently on. */
	int playerPathProgress; /**< The progress that the player has on the current path segment they are moving through. */

	Cell* getCell( int x, int y );
	const Cell* getCell( int x, int y ) const;
};

#endif // MAP_HPP
