#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include "Color.hpp"

/**
 * An array of pixels.
 */
class Image
{
public:
	/**
	 * Create an empty image.
	 *
	 * @param width the width of the image.
	 * @param height the height of the image.
	 */
	Image(int width, int height);

	/**
	 * Load an image from a file.
	 *
	 * @param fileName the name of the file to load from.
	 */
	Image(const std::string& fileName);

	~Image();

	/**
	 * Blit another image onto the image.
	 *
	 * @param source the source Image.
	 * @param x the left coordinate.
	 * @param y the top coordinate.
	 * @note This does not check bounds prior to blitting. Be careful.
	 */
	void blit( const Image& source, int x, int y );

	/**
	 * Get the height of the image.
	 */
	int getHeight() const;

	/**
	 * Get a pixel of the Image.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 */
	Color getPixel(int x, int y) const;

	/**
	 * Get the width of the image.
	 */
	int getWidth() const;

	/**
	 * Save the image to a new file.
	 *
	 * @param fileName the name of the file to save to.
	 */
	void save(const std::string& fileName);

	/**
	 * Set a pixel of the Image.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param pixel the color of the pixel.
	 */
	void setPixel(int x, int y, Color pixel);

	/**
	 * Write text onto the image using the default font.
	 *
	 * @param x the left coordinate.
	 * @param y the top coordinate.
	 * @param color the color of the text.
	 * @param text the string of text.
	 */
	void writeText( int x, int y, Color color, const std::string& text );

private:
	Color* pixels;
	int width;
	int height;
};

#endif // IMAGE_HPP
