#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "BitmapFont.hpp"
#include "Episode.hpp"
#include "Game.hpp"
#include "Globals.hpp"
#include "InfinityState.hpp"
#include "Player.hpp"
#include "Rendering.hpp"
#include "TransitionState.hpp"
#include "World.hpp"

// Level Generators
#include "LevelTheme.hpp"
#include "LevelGenerators/HillyLevelGenerator.hpp"
#include "LevelGenerators/SimpleLevelGenerator.hpp"
#include "LevelGenerators/TestLevelGenerator.hpp"

static const int BG_COLOR_CYCLE_FRAMES = 720; /**< The number of frames in a complete cycle of the background colors that are rendered. */

InfinityState::InfinityState() :
	GameState(true),
	fadeInProgress(0),
	fadeOutProgress(0),
	levelNumber(0)
{
	// Create the game session for this game
	Singleton<GameSession>::destroyInstance();
	Singleton<GameSession>::createInstance();
	GAME_SESSION.episode = nullptr;
	GAME_SESSION.world = new World();
	GAME_SESSION.player = new Player(0);

	generateNewLevel();
}

InfinityState::~InfinityState()
{
	Singleton<GameSession>::destroyInstance();
}

void InfinityState::generateNewLevel()
{
	// Clear the episode
	delete GAME_SESSION.episode;
	GAME_SESSION.episode = new Episode();

	// Generate a new level
	std::vector<LevelGenerator*> generators;
	//generators.push_back(new SimpleLevelGenerator);
	//generators.push_back(new HillyLevelGenerator);
	//generators.push_back(new SmbLevelLoader);
	generators.push_back(new TestLevelGenerator);
	Random random;
	random.seedTime();
	const std::vector<LevelTheme*>& themes = RESOURCE_MANAGER.getLevelThemes();

	// Pick a suitable combination theme and level generator
	const LevelGenerator& generator = *generators[random.nextInt() % generators.size()];
	Level* level = generator.generateLevel(random.nextInt());
	while( true )
	{
		const LevelTheme& theme = *themes[random.nextInt(themes.size())];
		if( !level->isThemeCompatible(theme) )
		{
			continue;
		}
		level->setTheme(theme, random);
		GAME_SESSION.episode->addLevel(1, level);
		break;
	}

	levelNumber++;

	for( auto g : generators )
	{
		delete g;
	}
}

void InfinityState::input()
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
			}
			break;

		case SDL_QUIT:
			getGame().quit();
			break;

		default:
			break;
		}
	}

	// Don't let the player do things during the transition
	if( fadeInProgress != 0 || fadeOutProgress != 0 )
	{
		return;
	}

	INPUT_MANAGER.update();
	InputManager::Controller* controller = INPUT_MANAGER.getController(0);
	if( controller->getButtonState(BUTTON_A) || controller->getButtonState(BUTTON_START) )
	{
		fadeOutProgress = 1;
		PLAY_SOUND("level_enter");
		///@todo stop music here if there is any playing
	}
}

void InfinityState::onResume()
{
	fadeOutProgress = 0;
	fadeInProgress = 1;

	if( GAME_SESSION.world->getStatus().statusType == WORLD_LEVEL_ENDED )
	{
		generateNewLevel();
	}
}

void InfinityState::render()
{
	Color backgroundColor = Color::hsv( (getGame().getFrameCount() % BG_COLOR_CYCLE_FRAMES) / (float)BG_COLOR_CYCLE_FRAMES, 1.0f, 1.0f );
	renderClearScreen( backgroundColor.normalizedR(), backgroundColor.normalizedG(), backgroundColor.normalizedB() );
	renderSetUnitsToPixels();

	// Draw status information
	auto font = GET_FONT("font_bold");
	std::string text = "Infinity Mode";
	glTranslatef( renderGetScreenWidth() / 2 - font->getTextWidth(text) / 2, renderGetScreenHeight() * 2.0f / 3.0f, 0.0f);
	font->renderText(text);
	glTranslatef( 0.0f, -2.0f * font->getCharacterHeight(), 0.0f );
	char buffer[128];
	sprintf( buffer, "Level %d\nLives %d\nScore %d\n\nPress Any Button", levelNumber, GAME_SESSION.player->getLives(), GAME_SESSION.player->getScore() );
	font->renderText(buffer);


	// Draw fade in/out effect
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

void InfinityState::update()
{
	input();

	// Are we fading in?
	if( fadeInProgress > 0 )
	{
		if( ++fadeInProgress > FADE_DURATION )
		{
			fadeInProgress = 0;
		}
	}
	// Are we fading out?
	else if( fadeOutProgress > 0 )
	{
		if( ++fadeOutProgress > FADE_DURATION )
		{
			getGame().pushState(new TransitionState(1));
		}
	}

	render();
}
