#include "Globals.hpp"

void* window = nullptr;

void createGlobals()
{
	Singleton<Logger>::setInstance(new Logger); // This always goes first
	Singleton<FpsManager>::setInstance(new FpsManager(GAME_FPS));
	Singleton<Settings>::createInstance();
	Singleton<ResourceManager>::createInstance();
	Singleton<InputManager>::createInstance();
	//Singleton<GameSession>::createInstance();
}

void destroyGlobals()
{
	//Singleton<GameSession>::destroyInstance();
	Singleton<InputManager>::destroyInstance();
	Singleton<FpsManager>::destroyInstance();
	Singleton<Settings>::destroyInstance();
	Singleton<ResourceManager>::destroyInstance();
	Singleton<Logger>::destroyInstance(); // This always goes last
}
