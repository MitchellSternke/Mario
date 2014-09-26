#ifndef BITMAPFONT_HPP
#define BITMAPFONT_HPP

#include <string>
#include <vector>

#include "Image.hpp"
#include "Vector2.hpp"

/**
 * A bitmap font that can render text.
 */
class BitmapFont
{
public:
	/**
	 * Create a new bitmap font using an image.
	 *
	 * @param imageFileName the name of the image file.
	 * @param charWidth the width of each character.
	 * @param charHeight the height of each character.
	 */
	BitmapFont( const std::string& imageFileName, int charWidth, int charHeight );

	/**
	 * Add a single character to the bitmap font.
	 *
	 * @param ch the character to add.
	 * @param x the left coordinate on the image.
	 * @param y the top coordinate on the image.
	 */
	void addCharacter( char ch, int x, int y );

	/**
	 * Add multiple characters in the same row to the bitmap font.
	 *
	 * @param ch the first character to add.
	 * @param x the left coordinate of the first character.
	 * @param y the top coordinate of the characters.
	 * @param number the number of characters to add.
	 */
	void addCharacters( char ch, int x, int y, int number );

	/**
	 * Get the height of characters rendered by the font.
	 */
	int getCharacterHeight() const;

	/**
	 * Get the width of characters rendered by the font.
	 */
	int getCharacterWidth() const;

	/**
	 * Get the width of text rendered using this font.
	 */
	int getTextWidth( const std::string& text ) const;

	/**
	 * Render text using the font.
	 *
	 * @param text the text to render.
	 */
	void renderText( const std::string& text ) const;

	/**
	 * Render text scaled with one unit equal to one pixel.
	 *
	 * @param text the text to render.
	 */
	void renderTextScaled( const std::string& text ) const;

private:
	std::vector< Vector2<int> > charData;
	Image image;
	int charWidth;
	int charHeight;
};

#endif // BITMAPFONT_HPP
