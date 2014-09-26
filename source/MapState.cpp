#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>

#include "BitmapFont.hpp"
#include "Game.hpp"
#include "Globals.hpp"
#include "Map.hpp"
#include "MapState.hpp"
#include "Rendering.hpp"
#include "TransitionState.hpp"
#include "World.hpp"

MapState::MapState() :
	GameState(true),
	node(0),
	fadeInProgress(0),
	fadeOutProgress(0)
{
	PLAY_MUSIC("map_overworld", true);

	// Create a new test map
	static const int TEST_MAP_SIZE = 64;
	map = new Map(TEST_MAP_SIZE, TEST_MAP_SIZE);

	// Fill it with water
	const Animation* tiles = GET_ANIMATION("map_tiles");
	for( int x = 0; x < TEST_MAP_SIZE; x++ )
	{
		for( int y = 0; y < TEST_MAP_SIZE; y++ )
		{
			map->setTile(x, y, tiles, 0x1f6);
		}
	}

	// Create the first island
	map->setTile(1, 0, tiles, 0x2fc);
	map->setTile(2, 0, tiles, 0x2fd);
	map->setTile(0, 1, tiles, 0x2a3);
	map->setTile(1, 1, tiles, 0x2a6);
	map->setTile(2, 1, tiles, 0x2a7);
	map->setTile(3, 1, tiles, 0x2aa);
	map->setTile(0, 2, tiles, 0x235);
	map->setTile(1, 2, tiles, 0x273);
	map->setTile(2, 2, tiles, 0x274);
	map->setTile(3, 2, tiles, 0x23c);
	map->setTile(0, 3, tiles, 0x1f8);
	map->setTile(1, 3, tiles, 0x238);
	map->setTile(2, 3, tiles, 0x239);
	map->setTile(3, 3, tiles, 0x1ff);
	map->setTile(0, 4, tiles, 0x1bb);
	map->setTile(1, 4, tiles, 0x243);
	map->setTile(2, 4, tiles, 0x245);
	map->setTile(3, 4, tiles, 0x1c2);
	map->setTile(0, 5, tiles, 0x182);
	map->setTile(1, 5, tiles, 0x1c9);
	map->setTile(2, 5, tiles, 0x1cb);
	map->setTile(3, 5, tiles, 0x189);
	map->setTile(0, 6, tiles, 0x14d);
	map->setTile(1, 6, tiles, 0x11a);
	map->setTile(2, 6, tiles, 0x11b);
	map->setTile(3, 6, tiles, 0x154);

	// Create the first node
	int level = 1;
	int node = map->createNode(1, 4, level++);
	map->setPlayerPosition(node);

	// Start looping to create the nodes and events
	Direction direction = RIGHT;
	int x = 0;
	int y = 0;
	int px;
	int py;
	while( true )
	{
		// Determine change in position
		px = x;
		py = y;
		if( direction == RIGHT )
		{
			x += 10;
		}
		else if( direction == LEFT )
		{
			x -= 10;
		}
		else if( x == 0 )
		{
			direction = RIGHT;
			x += 10;
		}
		else
		{
			direction = LEFT;
			x -= 10;
		}

		if( x + 4 > TEST_MAP_SIZE || x < 0 )
		{
			if( x + 4 > TEST_MAP_SIZE )
			{
				x -= 10;
			}
			else
			{
				x += 10;
			}
			direction = UP;
			y += 10;
		}
		if( y + 7 > TEST_MAP_SIZE )
		{
			break;
		}

		// Now, create the next node
		int newNode = map->createNode(x + 1, y + 4, level++);

		// Create the next path
		int path = map->createPath(node, direction, newNode, getOppositeDirection(direction));
		switch( direction )
		{
			case LEFT:
			case RIGHT:
				map->createPathSegment(path, Map::PATH_WALK, px + 2, py + 5, x + 2, y + 5);
				break;

			case UP:
				map->createPathSegment(path, Map::PATH_WALK, px + 2, py + 5, px + 2, py + 7);
				map->createPathSegment(path, Map::PATH_SWIM, px + 2, py + 7, px + 2, py + 10);
				map->createPathSegment(path, Map::PATH_CLIMB, px + 2, py + 10, px + 2, py + 13);
				map->createPathSegment(path, Map::PATH_WALK, px + 2, py + 13, px + 2, py + 15);
				break;

			default:
				assert(false);
				break;
		}

		// Create the next event
		int frames = 11;
		if( direction == UP )
		{
			frames = 10;
		}
		int event = map->createEvent(node, path, frames);

		// Frame 0 - shared with all events
		map->createEventTile(event, 0, x + 0, y + 0, tiles, 0x1f8);
		map->createEventTile(event, 0, x + 1, y + 0, tiles, 0x238);
		map->createEventTile(event, 0, x + 2, y + 0, tiles, 0x239);
		map->createEventTile(event, 0, x + 3, y + 0, tiles, 0x1ff);
		map->createEventTile(event, 0, x + 0, y + 1, tiles, 0x1bb);
		map->createEventTile(event, 0, x + 1, y + 1, tiles, 0x243);
		map->createEventTile(event, 0, x + 2, y + 1, tiles, 0x245);
		map->createEventTile(event, 0, x + 3, y + 1, tiles, 0x1c2);
		map->createEventTile(event, 0, x + 0, y + 2, tiles, 0x182);
		map->createEventTile(event, 0, x + 1, y + 2, tiles, 0x1c9);
		map->createEventTile(event, 0, x + 2, y + 2, tiles, 0x1cb);
		map->createEventTile(event, 0, x + 3, y + 2, tiles, 0x189);
		map->createEventTile(event, 0, x + 0, y + 3, tiles, 0x14d);
		map->createEventTile(event, 0, x + 1, y + 3, tiles, 0x11a);
		map->createEventTile(event, 0, x + 2, y + 3, tiles, 0x11b);
		map->createEventTile(event, 0, x + 3, y + 3, tiles, 0x154);

		// Frame 1 - shared with all events
		map->createEventTile(event, 1, x + 0, y + 0, tiles, 0x235);
		map->createEventTile(event, 1, x + 1, y + 0, tiles, 0x273);
		map->createEventTile(event, 1, x + 2, y + 0, tiles, 0x274);
		map->createEventTile(event, 1, x + 3, y + 0, tiles, 0x23c);
		map->createEventTile(event, 1, x + 0, y + 1, tiles, 0x1f8);
		map->createEventTile(event, 1, x + 1, y + 1, tiles, 0x238);
		map->createEventTile(event, 1, x + 2, y + 1, tiles, 0x239);
		map->createEventTile(event, 1, x + 3, y + 1, tiles, 0x1ff);
		map->createEventTile(event, 1, x + 0, y + 2, tiles, 0x1bb);
		map->createEventTile(event, 1, x + 1, y + 2, tiles, 0x243);
		map->createEventTile(event, 1, x + 2, y + 2, tiles, 0x245);
		map->createEventTile(event, 1, x + 3, y + 2, tiles, 0x1c2);
		map->createEventTile(event, 1, x + 0, y + 3, tiles, 0x182);
		map->createEventTile(event, 1, x + 1, y + 3, tiles, 0x1c9);
		map->createEventTile(event, 1, x + 2, y + 3, tiles, 0x1cb);
		map->createEventTile(event, 1, x + 3, y + 3, tiles, 0x189);
		map->createEventTile(event, 1, x + 0, y + 4, tiles, 0x14d);
		map->createEventTile(event, 1, x + 1, y + 4, tiles, 0x11a);
		map->createEventTile(event, 1, x + 2, y + 4, tiles, 0x11b);
		map->createEventTile(event, 1, x + 3, y + 4, tiles, 0x154);

		// Frame 2 - shared with all events
		map->createEventTile(event, 2, x + 0, y + 0, tiles, 0x1f6);
		map->createEventTile(event, 2, x + 1, y + 0, tiles, 0x2fc);
		map->createEventTile(event, 2, x + 2, y + 0, tiles, 0x2fd);
		map->createEventTile(event, 2, x + 3, y + 0, tiles, 0x1f6);
		map->createEventTile(event, 2, x + 0, y + 1, tiles, 0x2a3);
		map->createEventTile(event, 2, x + 1, y + 1, tiles, 0x2a6);
		map->createEventTile(event, 2, x + 2, y + 1, tiles, 0x2a7);
		map->createEventTile(event, 2, x + 3, y + 1, tiles, 0x2aa);
		map->createEventTile(event, 2, x + 0, y + 2, tiles, 0x235);
		map->createEventTile(event, 2, x + 1, y + 2, tiles, 0x273);
		map->createEventTile(event, 2, x + 2, y + 2, tiles, 0x274);
		map->createEventTile(event, 2, x + 3, y + 2, tiles, 0x23c);
		map->createEventTile(event, 2, x + 0, y + 3, tiles, 0x1f8);
		map->createEventTile(event, 2, x + 1, y + 3, tiles, 0x238);
		map->createEventTile(event, 2, x + 2, y + 3, tiles, 0x239);
		map->createEventTile(event, 2, x + 3, y + 3, tiles, 0x1ff);
		map->createEventTile(event, 2, x + 0, y + 4, tiles, 0x1bb);
		map->createEventTile(event, 2, x + 1, y + 4, tiles, 0x243);
		map->createEventTile(event, 2, x + 2, y + 4, tiles, 0x245);
		map->createEventTile(event, 2, x + 3, y + 4, tiles, 0x1c2);
		map->createEventTile(event, 2, x + 0, y + 5, tiles, 0x182);
		map->createEventTile(event, 2, x + 1, y + 5, tiles, 0x1c9);
		map->createEventTile(event, 2, x + 2, y + 5, tiles, 0x1cb);
		map->createEventTile(event, 2, x + 3, y + 5, tiles, 0x189);
		map->createEventTile(event, 2, x + 0, y + 6, tiles, 0x14d);
		map->createEventTile(event, 2, x + 1, y + 6, tiles, 0x11a);
		map->createEventTile(event, 2, x + 2, y + 6, tiles, 0x11b);
		map->createEventTile(event, 2, x + 3, y + 6, tiles, 0x154);

		if( direction == RIGHT )
		{
			map->createEventTile(event, 3, px + 3, py + 4, tiles, 0x23f);
			map->createEventTile(event, 3, px + 3, py + 5, tiles, 0x202);
			map->createEventTile(event, 4, x, y + 4, tiles, 0x23e);
			map->createEventTile(event, 4, x, y + 5, tiles, 0x201);
			for( int i = 0; i < 6; i++ )
			{
				map->createEventTile(event, 5 + i, px + 4 + i, py + 4, tiles, 0x1b9);
				map->createEventTile(event, 5 + i, px + 4 + i, py + 5, tiles, 0x180);
			}
		}
		else if( direction == LEFT )
		{
			map->createEventTile(event, 3, px, py + 4, tiles, 0x23e);
			map->createEventTile(event, 3, px, py + 5, tiles, 0x201);
			map->createEventTile(event, 4, x + 3, y + 4, tiles, 0x23f);
			map->createEventTile(event, 4, x + 3, y + 5, tiles, 0x202);
			for( int i = 0; i < 6; i++ )
			{
				map->createEventTile(event, 5 + i, px - i - 1, py + 4, tiles, 0x1b9);
				map->createEventTile(event, 5 + i, px - i - 1, py + 5, tiles, 0x180);
			}
		}
		else
		{
			map->createEventTile(event, 3, px + 1, py + 6, tiles, 0x33);
			map->createEventTile(event, 3, px + 2, py + 6, tiles, 0x34);
			map->createEventTile(event, 4, x + 1, y + 3, tiles, 0x62);
			map->createEventTile(event, 4, x + 2, y + 3, tiles, 0x63);
			for( int i = 0; i < 3; i++ )
			{
				int t1 = 0x3be;
				int t2 = 0x3bf;
				if( i % 2 )
				{
					t1 = 0x3e4;
					t2 = 0x3e5;
				}
				map->createEventTile(event, 5 + i, px + 1, py + 7 + i, tiles, t1);
				map->createEventTile(event, 5 + i, px + 2, py + 7 + i, tiles, t2);
			}
			for( int i = 0; i < 2; i++ )
			{
				map->createEventTile(event, 8 + i, px + 1, py + 11 + i, tiles, 0xeb);
				map->createEventTile(event, 8 + i, px + 2, py + 11 + i, tiles, 0xec);
			}
		}

		node = newNode;
	}
}

MapState::~MapState()
{
	delete map;
}

void MapState::input()
{
	SDL_Event event;
	while( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
						getGame().quit();
						break;

					case SDLK_e:
						if( map->getNode() != 0 )
						{
							map->playEvent(map->getNode(), 0);
						}
						break;

					default:
						break;
				}
				break;

			case SDL_QUIT:
				getGame().quit();
				break;

			default:
				break;
		}
	}

	// Don't let the player move or do things during the transition
	if( fadeInProgress != 0 || fadeOutProgress != 0 || map->isPlayingEvent() )
	{
		return;
	}

	INPUT_MANAGER.update();

	int levelId = map->getLevelId();
	if( levelId != 0 && INPUT_MANAGER.getController(0)->getButtonState(BUTTON_A) )
	{
		fadeOutProgress = 1;
		PLAY_SOUND("level_enter");
		Mix_HaltMusic();
	}
}

void MapState::onResume()
{
	fadeOutProgress = 0;
	fadeInProgress = 1;
}

void MapState::render() const
{
	// View settings computed at run
	static const double VIEW_WIDTH = SETTINGS.screenWidth / (SETTINGS.scale * (double)HALF_UNIT_SIZE);
	static const double VIEW_HEIGHT = SETTINGS.screenHeight / (SETTINGS.scale * (double)HALF_UNIT_SIZE);

	// Clear and set OpenGL settings
	renderClearScreen();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render the map
	map->render(map->getPlayerX(), map->getPlayerY(), VIEW_WIDTH, VIEW_HEIGHT);

	// Render information
	if( map->getLevelId() != 0 && fadeInProgress == 0 && fadeOutProgress == 0 )
	{
		const BitmapFont* font = GET_FONT("font_bold");
		char tempString[64];
		sprintf(tempString, "Level %d", map->getLevelId());
		renderSetUnitsToPixels();
		glTranslatef(
			SETTINGS.getRenderedScreenWidth() / 2.0 - strlen(tempString) * font->getCharacterWidth() / 2.0,
			SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 1.5f,
			1.0f
		);
		font->renderText(tempString);
	}

	// Render the fade in / fade out effect
	if( fadeInProgress > 0 || fadeOutProgress > 0 )
	{
		double fade;
		if( fadeInProgress > 0 )
		{
			fade = fadeInProgress / (double)FADE_DURATION;
			fade = 1.0 - fade;
		}
		else if( fadeOutProgress > 0 )
		{
			fade = fadeOutProgress / (double)FADE_DURATION;
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.0f, 0.0f, 0.0f, fade);
		glBegin(GL_QUADS);
			glVertex2i(0, 0);
			glVertex2i(1, 0);
			glVertex2i(1, 1);
			glVertex2i(0, 1);
		glEnd();
	}

	renderSwapBuffers();
}

void MapState::update()
{
	input();

	// Are we fading in?
	if( fadeInProgress > 0 )
	{
		if( ++fadeInProgress > FADE_DURATION )
		{
			WorldStatus status = GAME_SESSION.world->getStatus();
			if( status.statusType == WORLD_LEVEL_ENDED )
			{
				map->playEvent( node, status.exit );
			}
			fadeInProgress = 0;
			PLAY_MUSIC("map_overworld", true);
		}
	}
	// Are we fading out?
	else if( fadeOutProgress > 0 )
	{
		if( ++fadeOutProgress > FADE_DURATION )
		{
			getGame().pushState(new TransitionState(map->getLevelId()));
			node = map->getNode();
		}
	}
	// Update the map as normal
	else
	{
		map->update();
	}

	render();
}
