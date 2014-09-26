#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#define SETTINGS_FILE_NAME "settings.ini"

/**
 * Settings loaded from the configuration file.
 */
struct Settings
{
	int screenWidth;  /**< The width of the screen, in pixels. */
	int screenHeight; /**< The height of the screen, in pixels. */
	int scale;        /**< The zoom level for rendering. */
	bool fullscreen;  /**< Full screen on/off. */
	bool sound;       /**< Sound effects on/off. */
	bool music;       /**< Music on/off. */
	bool debugMode;   /**< Debug mode on/off. */

	/**
	 * Initializes with default settings.
	 */
	Settings();

	/**
	 * Get the height of the screen in pixels with scale taken into consideration.
	 */
	int getRenderedScreenHeight() const;

	/**
	 * Get the width of the screen in pixels with scale taken into consideration.
	 */
	int getRenderedScreenWidth() const;
};

#endif // SETTINGS_HPP
