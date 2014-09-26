/**
 * @file
 * @brief Declares the external font data symbol.
 */
#ifndef FONT_HPP
#define FONT_HPP

// Font parameters:
#define FONT_GLYPHS 128
#define FONT_WIDTH  5
#define FONT_HEIGHT 5

/**
 * Font data for all ASCII characters in bitmap format. This is 5x5 pixels
 * in size. This is indexed as [character][y][x].
 */
extern const unsigned char fontData[FONT_GLYPHS][FONT_HEIGHT][FONT_WIDTH];

#endif // FONT_HPP
