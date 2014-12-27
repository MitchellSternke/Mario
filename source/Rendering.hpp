/**
 * @file
 * @brief defines utility functions used during rendering.
 */
#ifndef RENDERING_HPP
#define RENDERING_HPP

/**
 * Clamp view coordinates so that nothing is rendered out of bounds.
 *
 * @param viewX the center x coordinate.
 * @param viewY the center y coordinate.
 * @param viewWidth the width of the viewport.
 * @param viewHeight the height of the viewport.
 * @param width the width of the area being rendered, in units.
 * @param height the height of the area being rendered, in units.
 */
void renderClampView( double& viewX, double& viewY, double viewWidth, double viewHeight, int width, int height );

/**
 * Clear the screen and set appropriate OpenGL settings.
 *
 * @param r the red component of the clear color.
 * @param g the green component of the clear color.
 * @param b the blue component of the clear color.
 * @param a the alpha component of the clear color.
 */
void renderClearScreen( float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.5f);

/**
 * Get the height of the screen in pixels.
 */
int renderGetScreenHeight();

/**
 * Get the width of the screen in pixels.
 */
int renderGetScreenWidth();

/**
 * Set the matrix units to represent individual pixels on the screen.
 */
void renderSetUnitsToPixels();

/**
 * Swap screen buffers.
 */
void renderSwapBuffers();

#endif // RENDERING_HPP
