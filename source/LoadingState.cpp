#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "Game.hpp"
#include "Globals.hpp"
#include "InfinityState.hpp"
#include "LoadingState.hpp"
#include "MapState.hpp"
#include "Player.hpp"
#include "Rendering.hpp"
#include "Text.hpp"
#include "World.hpp"

#include "Episode.hpp"
#include "LevelGenerators/HillyLevelGenerator.hpp"
#include "LevelGenerators/SimpleLevelGenerator.hpp"
#include "LevelGenerators/SmbLevelLoader.hpp"
#include "LevelGenerators/TestLevelGenerator.hpp"
#include "LevelTheme.hpp"

void LoadingState::onLog( const std::string& text )
{
	if( SDL_GetTicks() - lastRenderTime < 16 )
	{
		return;
	}

	static int frame = 0;

	// Process events so that the window doesn't hang
	SDL_PumpEvents();

	// Draw a loading splash screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	renderSetUnitsToPixels();
	glTranslatef( 0, SETTINGS.getRenderedScreenHeight() - FONT_HEIGHT - 1, 0);

	glColor3ub(0xff, 0xff, 0xff);
	char tempString[64];
	char c = '|';
	switch( (frame++ / 2) % 4 )
	{
	case 0:
		c = '|';
		break;
	case 1:
		c = '/';
		break;
	case 2:
		c = '-';
		break;
	case 3:
		c = '\\';
		break;
	}
	sprintf(tempString, "Loading resources... %d%c", RESOURCE_MANAGER.getLoadingProgress(), c);
	drawText(tempString);

	SDL_GL_SwapWindow((SDL_Window*)window);

	lastRenderTime = SDL_GetTicks();
}

void LoadingState::update()
{
	lastRenderTime = 0;
	LOG.addListener(*this);
	RESOURCE_MANAGER.loadResources("resources.xml");
	LOG.removeListener(*this);
#if 0
	getGame().switchState(new MapState);
#else
	getGame().switchState(new InfinityState);
	return;
#endif

	///@todo remove this
#if 0
	///@todo move this elsewhere. Create an Episode Generator?
	std::vector<LevelGenerator*> generators;
	generators.push_back(new SimpleLevelGenerator);
	//generators.push_back(new HillyLevelGenerator);
	//generators.push_back(new SmbLevelLoader);
	//generators.push_back(new TestLevelGenerator);
	Episode* episode = new Episode;
	Random random;
	random.seedTime();
	const std::vector<LevelTheme*>& themes = RESOURCE_MANAGER.getLevelThemes();
	for( int i = 1 ; i <= 12; i++ )
	{
		// Pick a suitable combination theme and level generator
		const LevelGenerator& generator = *generators[random.nextInt() % generators.size()];
		while( true )
		{
			const LevelTheme& theme = *themes[random.nextInt() % themes.size()];
			bool dependenciesSatisfied = true;
			for( int type = 0; type < NUM_TILE_TYPES; type++ )
			{
				if( generator.hasDependency(static_cast<TileType>(type)) && theme.tileTypes[type].empty() )
				{
					dependenciesSatisfied = false;
					break;
				}
			}

			if( !dependenciesSatisfied )
			{
				continue;
			}
			episode->addLevel(i, generator, theme, random.nextInt());
			break;
		}
	}

	GAME_SESSION.episode = episode;
	GAME_SESSION.world = new World();
	GAME_SESSION.player = new Player(0);
#endif
}
