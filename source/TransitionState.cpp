#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "Animation.hpp"
#include "Game.hpp"
#include "Globals.hpp"
#include "MainState.hpp"
#include "Math.hpp"
#include "Rendering.hpp"
#include "TransitionState.hpp"

TransitionState::TransitionState( int levelId ) :
	GameState(true),
	levelId(levelId),
	progress(0)
{
}

void TransitionState::onResume()
{
	getGame().popState();
}

void TransitionState::update()
{
	// Handle input
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

	// Rendering
	renderClearScreen();
	renderSetUnitsToPixels();
	RESOURCE_MANAGER.bindTextureAtlas();
	const Animation* text = GET_ANIMATION("mario_start_text");
	Animation::Frame f = text->getFrame(progress);
	glTranslatef(
		SETTINGS.getRenderedScreenWidth() / 2.0 - f.width / 2.0 * UNIT_SIZE,
		SETTINGS.getRenderedScreenHeight() - SETTINGS.getRenderedScreenHeight() / 2.0 * clamp(progress / 30.0, 0.0, 1.0) - (f.height / 2.0 * UNIT_SIZE),
		0.0f
	);
	text->renderFrame(f);
	SDL_GL_SwapWindow((SDL_Window*)window);

	// Check if we are done
	if( ++progress > 60 )
	{
		progress = 0;
		getGame().pushState(new MainState(levelId));
	}
}
