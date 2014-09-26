#include <GL/gl.h>

#include "Texture.hpp"

Texture::Texture( const Image& image )
{
	// Create a texture unit
	glGenTextures(1, &id);

	// Copy the image to a texture
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned numberOfBytes = 4 * image.getWidth() * image.getHeight();
	unsigned char* bytes = new unsigned char[numberOfBytes];
	unsigned byte = 0;

	for(int y = 0; y < image.getHeight(); y++)
	{
		for(int x = 0; x < image.getWidth(); x++)
		{
			Color pixel = image.getPixel(x, y);
			bytes[byte++] = pixel.r;
			bytes[byte++] = pixel.g;
			bytes[byte++] = pixel.b;
			bytes[byte++] = pixel.a;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, 4, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	delete [] bytes;
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::bind() const
{
	glBindTexture( GL_TEXTURE_2D, id );
}
