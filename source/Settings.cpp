#include "Settings.hpp"

Settings::Settings()
{
	// Hard-coded defaults
	screenWidth = 800;
	screenHeight = 600;
	scale = 2;
	fullscreen = false;
	sound = true;
	music = true;
	debugMode = false;
}

int Settings::getRenderedScreenHeight() const
{
	return screenHeight / scale;
}

int Settings::getRenderedScreenWidth() const
{
	return screenWidth / scale;
}
