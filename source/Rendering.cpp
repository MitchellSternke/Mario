#include <SDL2/SDL_opengl.h>

#include "Globals.hpp"
#include "Rendering.hpp"

void renderClampView( double& viewX, double& viewY, double viewWidth, double viewHeight, int width, int height )
{
	if( viewX - viewWidth / 2.0 < 0 )
	{
		viewX = viewWidth / 2.0;
	}
	else if( viewX + viewWidth / 2.0 > width )
	{
		viewX = static_cast<double>(width) - viewWidth / 2.0;
	}
	if( viewY - viewHeight / 2.0 < 0 )
	{
		viewY = viewHeight / 2.0;
	}
	else if( viewY + viewHeight / 2.0 > height )
	{
		viewY = static_cast<double>(height) - viewHeight / 2.0;
	}
}

void renderClearScreen()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

int renderGetScreenHeight()
{
	return SETTINGS.screenHeight / SETTINGS.scale;
}

int renderGetScreenWidth()
{
	return SETTINGS.screenWidth / SETTINGS.scale;
}

void renderSetUnitsToPixels()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, SETTINGS.getRenderedScreenWidth(), 0.0, SETTINGS.getRenderedScreenHeight(), -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void renderSwapBuffers()
{
	SDL_GL_SwapWindow((SDL_Window*)window);
}
