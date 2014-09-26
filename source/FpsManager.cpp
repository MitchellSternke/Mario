#include <SDL2/SDL.h>

#include "FpsManager.hpp"

FpsManager::FpsManager(float fps) :
	currentFps(0),
	desiredFps(fps),
	fpsTicks(SDL_GetTicks()),
	frameCount(0),
	ticks(SDL_GetTicks())
{
}

float FpsManager::getDesiredFrameRate() const
{
	return desiredFps;
}

float FpsManager::getFrameRate() const
{
	return currentFps;
}

bool FpsManager::update()
{
	//Manage framerate
	bool frameSkip = true;
	Uint32 tickDifference = SDL_GetTicks() - ticks;
	if( (float)tickDifference < (1000.0f / desiredFps) )
	{
		Uint32 delay = (1000 / (int)desiredFps) - tickDifference;
		SDL_Delay(delay);
		frameSkip = false;
	}
	ticks = SDL_GetTicks();

	frameCount++;

	//Calculate fps
	unsigned ticksSinceLastCheck = SDL_GetTicks() - fpsTicks;
	if( ticksSinceLastCheck >= 1000 )
	{
		currentFps = (float)frameCount / ((float)(ticksSinceLastCheck) / 1000.0f);
		frameCount = 0;
		fpsTicks = SDL_GetTicks();
	}

	return frameSkip;
}
