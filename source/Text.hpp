/**
 * @file
 * @brief declares text rendering functions.
 */
#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>

#include "Font.hpp"

/**
 * Draw bordered text.
 */
void drawBorderedText( const std::string& text );

/**
 * Draw bordered text scaled to the appropriate unit size defined by the game.
 */
void drawBorderedTextScaled( const std::string& text );

/**
 * Draw text.
 */
void drawText(const std::string& text);

/**
 * Draw text scaled to the appropriate unit size defined by the game.
 */
void drawTextScaled( const std::string& text );

#endif // TEXT_HPP
