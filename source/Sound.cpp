#include "Sound.hpp"

Sound::Sound( const std::string& fileName )
{
	chunk = Mix_LoadWAV(fileName.c_str());
}

Sound::~Sound()
{
	Mix_FreeChunk(chunk);
}

bool Sound::isLoaded() const
{
	return (chunk != nullptr);
}

void Sound::play( int channel ) const
{
	Mix_PlayChannel(channel, chunk, 0);
}
