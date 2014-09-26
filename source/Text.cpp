#include <GL/gl.h>

#include "Font.hpp"
#include "Globals.hpp"
#include "Text.hpp"

static unsigned char borderedFontData[FONT_GLYPHS][FONT_HEIGHT + 2][FONT_WIDTH + 2];
static bool borderedFontDataInitialized = false;

static void createBorderedFontData()
{
	for( int i = 0; i < FONT_GLYPHS; i++ )
	{
		// Initialize the array
		for( int y = 0; y < FONT_HEIGHT + 2; y++ )
		{
			for( int x = 0; x < FONT_HEIGHT + 2; x++ )
			{
				borderedFontData[i][x][y] = 0;
			}
		}

		// Copy each pixel of the bitmap font and add a border around each pixel
		for( int y = 0; y < FONT_HEIGHT; y++ )
		{
			for( int x = 0; x < FONT_WIDTH; x++ )
			{
				if( fontData[i][y][x] != 0 )
				{
					borderedFontData[i][y + 1][x + 1] = 1;
					for( int j = -1; j <= 1; j++ )
					{
						for( int k = -1; k <= 1; k++ )
						{
							if( borderedFontData[i][y + 1 + j][x + 1 + k] == 0 )
							{
								borderedFontData[i][y + 1 + j][x + 1 + k] = 2;
							}
						}
					}
				}
			}
		}
	}
}

static void drawText(const std::string& text, int charWidth, int charHeight, const unsigned char* data)
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

		// Draw the right character
		for( int cx = 0; cx < charWidth; ++cx )
		{
			for( int cy = 0; cy < charHeight; ++cy )
			{
				unsigned char p = data[ch * charWidth * charHeight + (charHeight - 1 - cy) * charWidth + cx];
				if( p != 0 )
				{
					if( p == 1 )
					{
						glColor4ub(0xff, 0xff, 0xff, 0xff);
					}
					else
					{
						glColor4ub(0, 0, 0, 0xff);
					}
					glVertex2i(x * (charWidth + 1) + cx, y * -1 * (charHeight + 1) + cy);
					glVertex2i(x * (charWidth + 1) + cx + 1, y * -1 * (charHeight + 1) + cy);
					glVertex2i(x * (charWidth + 1) + cx + 1, y * -1 * (charHeight + 1) + cy + 1);
					glVertex2i(x * (charWidth + 1) + cx, y * -1 * (charHeight + 1) + cy + 1);
				}
			}
		}
		x++;
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void drawBorderedText(const std::string& text)
{
	if( !borderedFontDataInitialized )
	{
		createBorderedFontData();
		borderedFontDataInitialized = true;
	}

	drawText(text, FONT_WIDTH + 2, FONT_HEIGHT + 2, &borderedFontData[0][0][0]);
}

void drawBorderedTextScaled(const std::string& text)
{
	glPushMatrix();
	glScalef(1.0 / UNIT_SIZE, 1.0 / UNIT_SIZE, 1.0);
	drawBorderedText(text);
	glPopMatrix();
}

void drawText(const std::string& text)
{
	drawText(text, FONT_WIDTH, FONT_HEIGHT, &fontData[0][0][0]);
}

void drawTextScaled( const std::string& text )
{
	glPushMatrix();
	glScalef(1.0 / UNIT_SIZE, 1.0 / UNIT_SIZE, 1.0);
	drawText(text);
	glPopMatrix();
}
