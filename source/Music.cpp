#include "Globals.hpp"
#include "Music.hpp"

Music::Music(const std::string& fileName)
{
	music = Mix_LoadMUS(fileName.c_str());
}

Music::~Music()
{
	Mix_FreeMusic(music);
}

bool Music::isLoaded() const
{
	return (music != nullptr);
}

void Music::play(bool loop) const
{
	if( SETTINGS.music )
	{
		Mix_HaltMusic();
		Mix_PlayMusic(music, loop ? -1 : 0);
	}
}
