#include <GL/gl.h>

#include "BitmapFont.hpp"
#include "Globals.hpp"

BitmapFont::BitmapFont( const std::string& imageFileName, int charWidth, int charHeight ) :
	image(imageFileName),
	charWidth(charWidth),
	charHeight(charHeight)
{
	charData.resize(94, Vector2<int>(-1, -1));
}

void BitmapFont::addCharacter( char ch, int x, int y )
{
	charData[ch - ' ' - 1] = Vector2<int>(x, y);
}

void BitmapFont::addCharacters( char ch, int x, int y, int number )
{
	for( int i = 0; i < number; i++ )
	{
		addCharacter( ch + (char)i, x + charWidth * i, y );
	}
}

int BitmapFont::getCharacterHeight() const
{
	return charHeight;
}

int BitmapFont::getCharacterWidth() const
{
	return charWidth;
}

int BitmapFont::getTextWidth( const std::string& text ) const
{
	return getCharacterWidth() * text.length();
}

///@todo texture quads
void BitmapFont::renderText( const std::string& text ) const
{
	glDisable(GL_TEXTURE_2D);
	int x = 0;
	int y = 0;
	glBegin(GL_QUADS);
	for( std::size_t i = 0; i < text.size(); ++i )
	{
		unsigned char ch = text[i];
		if( ch > 0x7f )
		{
			ch = 0x7f;
		}

		if( ch == '\n' ) // newline
		{
			y++;
			x = 0;
			continue;
		}
		else if( ch <= ' ' )
		{
			x++;
			continue;
		}

		// Draw the right character
		for( int cx = 0; cx < charWidth; ++cx )
		{
			for( int cy = 0; cy < charHeight; ++cy )
			{
				if( charData[ch - ' ' - 1].x == -1 )
				{
					continue;
				}

				Color c = image.getPixel( charData[ch - ' ' - 1].x + cx, charData[ch - ' ' - 1].y + charHeight - 1 - cy );
				if( c.a != 0 )
				{
					glColor4ub(c.r, c.g, c.b, c.a);
					glVertex2i(x * (charWidth) + cx, y * -1 * (charHeight) + cy);
					glVertex2i(x * (charWidth) + cx + 1, y * -1 * (charHeight) + cy);
					glVertex2i(x * (charWidth) + cx + 1, y * -1 * (charHeight) + cy + 1);
					glVertex2i(x * (charWidth) + cx, y * -1 * (charHeight) + cy + 1);
				}
			}
		}
		x++;
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

///@todo deprecate this
void BitmapFont::renderTextScaled( const std::string& text ) const
{
	glPushMatrix();
	glScalef(1.0 / UNIT_SIZE, 1.0 / UNIT_SIZE, 1.0);
	renderText(text);
	glPopMatrix();
}
