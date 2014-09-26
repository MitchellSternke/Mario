#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Image.hpp"

/**
 * A texture stored in graphics memory.
 */
class Texture
{
public:
	/**
	 * Create a new texture from an image.
	 */
	Texture( const Image& image );

	~Texture();

	/**
	 * Bind the texture for usage.
	 */
	void bind() const;

private:
	unsigned int id;
};

#endif // TEXTURE_HPP
