#ifndef SOUND_HPP
#define SOUND_HPP

#include <string>

#include <SDL2/SDL_mixer.h>

/**
 * A sound effect.
 */
class Sound
{
public:
	/**
	 * Load a sound effect from a file.
	 *
	 * @param fileName the name of the file.
	 */
	Sound(const std::string& fileName);

	~Sound();

	/**
	 * Check if the sound effect has loaded successfully.
	 */
	bool isLoaded() const;

	/**
	 * Play the sound effect.
	 */
	void play( int channel = -1) const;

private:
	Mix_Chunk* chunk;
};

#endif // SOUND_HPP
