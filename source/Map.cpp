#include <algorithm>
#include <cmath>

#include <GL/gl.h>

#include "Animation.hpp"
#include "Globals.hpp"
#include "Math.hpp"
#include "Map.hpp"
#include "Rendering.hpp"
#include "Text.hpp"

static const int FRAMES_PER_EVENT_FRAME = 12;

Map::Cell::Cell() :
	animation(nullptr),
	animationFrame(0),
	node(0)
{
}

Map::Node::Node()
{
	for( int i = 0; i < 4; i++ )
	{
		paths[i] = 0;
		events[i] = 0;
	}
}

Map::Map( int width, int height ) :
	width(width),
	height(height),
	currentEvent(0),
	currentEventFrame(0),
	currentEventProgress(0),
	playerX(0),
	playerY(0),
	playerNode(0),
	playerPath(0),
	playerPathSegment(0),
	playerPathProgress(0)
{
	cells.resize(width * height);
}

int Map::createEvent( int node, int path, int frames, int exit )
{
	events.push_back(Event());
	Event& e = events.back();
	e.frames.resize(frames);
	e.path = path;
	nodes[node - 1].events[exit] = events.size();

	return events.size();
}

void Map::createEventTile( int event, int eventFrame, int x, int y, const Animation* animation, int animationFrame )
{
	Event& e = events[event - 1];
	EventTile t;
	t.x = x;
	t.y = y;
	t.animation = animation;
	t.animationFrame = animationFrame;
	e.frames[eventFrame].tiles.push_back(t);
}

int Map::createNode( int x, int y, int levelId )
{
	// Check that cells are open
	for( int a = 0; a < 2; a++ )
	{
		for( int b = 0; b < 2; b++ )
		{
			Cell* cell = getCell(x + a, y + b);
			if( cell == nullptr || (cell && cell->node != 0) )
			{
				return 0;
			}
		}
	}

	// Create the node
	Node node;
	node.x = x;
	node.y = y;
	node.levelId = levelId;
	nodes.push_back(node);
	int id = nodes.size();

	// Add the node to cells
	for( int a = 0; a < 2; a++ )
	{
		for( int b = 0; b < 2; b++ )
		{
			Cell* cell = getCell(x + a, y + b);
			cell->node = id;
		}
	}

	return id;
}

int Map::createPath( int node1, Direction direction1, int node2, Direction direction2 )
{
	Path path;
	path.nodes[0] = node1;
	path.nodes[1] = node2;
	path.active = false;
	paths.push_back(path);
	int id = paths.size();

	nodes[node1 - 1].paths[direction1] = id;
	nodes[node2 - 1].paths[direction2] = id;

	return id;
}

void Map::createPathSegment( int path, PathType type, double x1, double y1, double x2, double y2 )
{
	PathSegment s;
	s.type = type;
	s.x1 = x1;
	s.y1 = y1;
	s.x2 = x2;
	s.y2 = y2;
	paths[path - 1].segments.push_back(s);
}

Map::Cell* Map::getCell(int x, int y)
{
	if( x < 0 || x >= width || y < 0 || y >= height )
	{
		return nullptr;
	}

	return &cells[y * width + x];
}

const Map::Cell* Map::getCell( int x, int y ) const
{
	if( x < 0 || x >= width || y < 0 || y >= height )
	{
		return nullptr;
	}

	return &cells[y * width + x];
}

int Map::getLevelId() const
{
	if( playerPath != 0 )
	{
		return 0;
	}

	return nodes[playerNode - 1].levelId;
}

int Map::getNode() const
{
	if( playerPath != 0 )
	{
		return 0;
	}

	return playerNode;
}

double Map::getPlayerX() const
{
	return playerX;
}

double Map::getPlayerY() const
{
	return playerY;
}

bool Map::isPlayingEvent() const
{
	return (currentEvent != 0);
}

void Map::playEvent( int event )
{
	if( event != 0 && !paths[events[event - 1].path - 1].active )
	{
		currentEvent = event;
	}
}

void Map::playEvent( int node, int eventNumber )
{
	playEvent(nodes[node - 1].events[eventNumber]);
}

void Map::render( double viewX, double viewY, double viewWidth, double viewHeight ) const
{
	static int frame = 0;

	// bind the texture atlas
	RESOURCE_MANAGER.bindTextureAtlas();

	// clamp the view
	renderClampView(viewX, viewY, viewWidth, viewHeight, width, height);

	// render cells - layer 1
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for( int x = std::floor(viewX - viewWidth / 2.0); x <= std::ceil(viewX + viewWidth / 2.0); ++x )
	{
		for( int y = std::floor(viewY - viewHeight / 2.0); y <= std::ceil(viewY + viewHeight / 2.0); ++y )
		{
			const Cell* cell = getCell(x, y);
			if( cell == nullptr )
			{
				continue;
			}

			if( cell->animation != nullptr )
			{
				int f = cell->animationFrame;
				if( f == -1 )
				{
					f = frame;
				}

				glPushMatrix();
				glTranslatef(x - (viewX - viewWidth / 2.0), y - (viewY - viewHeight / 2.0), 0.0f);
				glScalef(1.0f / HALF_UNIT_SIZE, 1.0f / HALF_UNIT_SIZE, 1.0f);
				cell->animation->renderFrame(f);
				glPopMatrix();
			}
		}
	}

	// render any active events
	if( currentEvent != 0 )
	{
		float alpha = (FRAMES_PER_EVENT_FRAME - currentEventProgress) / static_cast<float>(FRAMES_PER_EVENT_FRAME);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		for( const EventTile& t : events[currentEvent - 1].frames[currentEventFrame].tiles )
		{
			glPushMatrix();
			glTranslatef(t.x - (viewX - viewWidth / 2.0), t.y - (viewY - viewHeight / 2.0), 1.0f );
			glScalef(1.0f / HALF_UNIT_SIZE, 1.0f / HALF_UNIT_SIZE, 1.0f);
			t.animation->renderFrame(t.animationFrame);
			glPopMatrix();
		}

		// If we are on the final event frame, also fade in the level icon
		if( currentEventFrame == static_cast<int>(events[currentEvent - 1].frames.size()) - 1 )
		{
			const Path& p = paths[events[currentEvent - 1].path - 1];
			for( int i = 0; i < 2; i++ )
			{
				glPushMatrix();
				glTranslatef(nodes[p.nodes[i] - 1].x - (viewX - viewWidth / 2.0), nodes[p.nodes[i] - 1].y - (viewY - viewHeight / 2.0), 2.0f);
				glScalef(1.0f / HALF_UNIT_SIZE, 1.0f / HALF_UNIT_SIZE, 1.0f);
				GET_ANIMATION("level_single_exit")->renderFrame(frame);
				glPopMatrix();
			}
		}
	}

	// render level icons - layer 2
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 3.0f);
	for( int x = std::floor(viewX - viewWidth / 2.0); x <= std::ceil(viewX + viewWidth / 2.0); ++x )
	{
		for( int y = std::floor(viewY - viewHeight / 2.0); y <= std::ceil(viewY + viewHeight / 2.0); ++y )
		{
			const Cell* cell = getCell(x, y);
			if( cell == nullptr )
			{
				continue;
			}

			if( cell->node != 0 )
			{
				const Node& n = nodes[cell->node - 1];
				bool active = false;
				for( int i = 0; i < 4; i++ )
				{
					if( n.paths[i] != 0 && paths[n.paths[i] - 1].active )
					{
						active = true;
						break;
					}
				}
				if( !active && playerNode != cell->node )
				{
					continue;
				}
				if( n.levelId != 0 && x == n.x && y == n.y )
				{
					glPushMatrix();
					glTranslatef(x - (viewX - viewWidth / 2.0), y - (viewY - viewHeight / 2.0), 0.0f);
					glScalef(1.0f / HALF_UNIT_SIZE, 1.0f / HALF_UNIT_SIZE, 1.0f);
					GET_ANIMATION("level_single_exit")->renderFrame(frame);
					glPopMatrix();
				}
			}
		}
	}
	glPopMatrix();

	// draw the player
	glPushMatrix();
	glTranslatef(playerX - (viewX - viewWidth / 2.0), playerY - (viewY - viewHeight / 2.0), 4.0f);
	glScalef(1.0f / HALF_UNIT_SIZE, 1.0f / HALF_UNIT_SIZE, 1.0f);
	const Animation* animation = nullptr;
	bool mirrorX = false;
	if( playerPath != 0 )
	{
		// Get the path we are on
		const Path& p = paths[playerPath - 1];
		int n2 = p.nodes[1];
		int segment = playerPathSegment;
		bool swapped = false;
		if( n2 == playerNode )
		{
			swapped = true;
			segment = p.segments.size() - 1 - playerPathSegment;
		}

		// Determine which direction we are moving
		const PathSegment& s = p.segments[segment];
		double x1 = s.x1;
		double x2 = s.x2;
		double y1 = s.y1;
		double y2 = s.y2;
		if( swapped )
		{
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		double dx = x2 - x1;
		double dy = y2 - y1;
		if( std::fabs(dx) > std::fabs(dy) )
		{
			switch( s.type )
			{
				case PATH_CLIMB:
					animation = GET_ANIMATION("map_mario_climb");
					break;
				case PATH_SWIM:
					animation = GET_ANIMATION("map_mario_swim_side");
					break;
				case PATH_WALK:
					animation = GET_ANIMATION("map_mario_walk_side");
					break;
			}
			if( dx < 0 )
			{
				mirrorX = true;
			}
		}
		else
		{
			if( dy < 0 )
			{
				switch( s.type )
				{
					case PATH_CLIMB:
						animation = GET_ANIMATION("map_mario_climb");
						break;
					case PATH_SWIM:
						animation = GET_ANIMATION("map_mario_swim_down");
						break;
					case PATH_WALK:
						animation = GET_ANIMATION("map_mario_walk_down");
						break;
				}
			}
			else
			{
				switch( s.type )
				{
					case PATH_CLIMB:
						animation = GET_ANIMATION("map_mario_climb");
						break;
					case PATH_SWIM:
						animation = GET_ANIMATION("map_mario_swim_up");
						break;
					case PATH_WALK:
						animation = GET_ANIMATION("map_mario_walk_up");
						break;
				}
			}
		}
	}
	// We aren't moving on the map
	if( animation == nullptr )
	{
		animation = GET_ANIMATION("map_mario");
	}
	Animation::Frame f = animation->getFrame(frame);
	glTranslatef(f.width / -2.0 * UNIT_SIZE, 0.0f, 0.0f);
	animation->renderFrame(f, mirrorX);
	glPopMatrix();

	if( SETTINGS.debugMode )
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		renderSetUnitsToPixels();
		glTranslatef(0.0f, SETTINGS.getRenderedScreenHeight() - FONT_HEIGHT - 2, 0.0f);
		char tempString[128];
		sprintf(tempString,
			"Debug Mode\nNode:%d\nPath:%d\nProgress:%d\nLevel:%d",
			playerNode,
			playerPath,
			playerPathProgress,
			playerPath == 0 ? nodes[playerNode - 1].levelId : 0
		);
		drawBorderedText(tempString);
	}

	frame++;
}

void Map::setPlayerPosition( int node )
{
	Node& n = nodes[node - 1];
	playerX = n.x + 1;
	playerY = n.y + 1;
	playerNode = node;
}

void Map::setTile( int x, int y, const Animation* animation, int frame )
{
	Cell* cell = getCell(x, y);
	if( cell == nullptr )
	{
		return;
	}

	cell->animation = animation;
	cell->animationFrame = frame;
}

void Map::update()
{
	// Is an event occurring?
	if( currentEvent != 0 )
	{
		if( currentEventProgress == 0 )
		{
			currentEventProgress = FRAMES_PER_EVENT_FRAME;
			Event& e = events[currentEvent - 1];
			for( EventTile& t : e.frames[currentEventFrame].tiles )
			{
				setTile(t.x, t.y, t.animation, t.animationFrame);
			}
			PLAY_SOUND_CHANNEL("map_reveal", 0);

			// Are we done?
			if( ++currentEventFrame >= (int)(e.frames.size()) )
			{
				currentEvent = 0;
				currentEventFrame = 0;
				currentEventProgress = 0;

				// Open the path for the event
				paths[e.path - 1].active = true;
				return;
			}
		}

		currentEventProgress--;
	}
	// Is the player stationary?
	else if( playerPath == 0 && playerNode != 0 )
	{
		InputManager::Controller* c = INPUT_MANAGER.getController(0);
		Direction d;
		if( c->getButtonState( BUTTON_DOWN ) )
		{
			d = DOWN;
		}
		else if( c->getButtonState( BUTTON_LEFT ) )
		{
			d = LEFT;
		}
		else if( c->getButtonState( BUTTON_RIGHT ) )
		{
			d = RIGHT;
		}
		else if( c->getButtonState( BUTTON_UP ) )
		{
			d = UP;
		}
		else
		{
			return;
		}

		Node& node = nodes[playerNode - 1];
		if( node.paths[d] != 0 && paths[node.paths[d] - 1].active )
		{
			playerPath = node.paths[d];
			playerPathSegment = 0;
			playerPathProgress = 0;
		}
	}
	// The player is moving
	else
	{
		static const double MOVEMENT_VELOCITY = 0.1;

		// Determine the path we are on
		Path& p = paths[playerPath - 1];
		int node1 = p.nodes[0];
		int node2 = p.nodes[1];

		// Determine the start and end nodes of the path depending on where we came from
		bool swapped = false;
		if( node1 != playerNode )
		{
			std::swap(node1, node2);
			swapped = true;
		}

		// Check if we are done
		if( playerPathSegment >= (int)p.segments.size() )
		{
			playerPath = 0;
			setPlayerPosition(node2);
			PLAY_SOUND("map_move");
			return;
		}

		// Find the segment of the path that we are on
		int segment = playerPathSegment;
		if( swapped )
		{
			segment = p.segments.size() - 1 - playerPathSegment;
		}
		PathSegment& s = p.segments[segment];
		Vector2<double> v(s.x1 - s.x2, s.y1 - s.y2);
		double segmentLength = v.getMagnitude();
		double x1 = s.x1;
		double x2 = s.x2;
		double y1 = s.y1;
		double y2 = s.y2;
		if( swapped )
		{
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		// Check if we are too far through the path
		double progress = playerPathProgress * MOVEMENT_VELOCITY;
		double t;
		if( progress > segmentLength)
		{
			t = 1.0;
			playerPathSegment++;
			playerPathProgress = 0;
		}
		else
		{
			t = progress / segmentLength;
		}
		playerPathProgress++;

		// Interpolate the player position
		playerX = lerp(x1, x2, t);
		playerY = lerp(y1, y2, t);
	}
}
