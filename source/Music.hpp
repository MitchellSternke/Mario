#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <string>

#include <SDL2/SDL_mixer.h>

/**
 * A music track.
 */
class Music
{
public:
	/**
	 * Load a music track from a file.
	 *
	 * @param fileName the name of the file.
	 */
	Music(const std::string& fileName);

	~Music();

	/**
	 * Check if the music track has been loaded successfully.
	 */
	bool isLoaded() const;

	/**
	 * Play the music track.
	 *
	 * @param loop whether the track should loop infinitely or not.
	 */
	void play(bool loop) const;

private:
	Mix_Music* music;
};

#endif // MUSIC_HPP
