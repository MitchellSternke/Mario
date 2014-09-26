#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>
#include <vector>

#include "Types.hpp"

/**
* A 32-bit RGBA color.
*/
class Color
{
public:
	/**
	* A name that corresponds to a color.
	*/
	enum Name
	{
		AZURE        = 0x007FFF,
		BLACK        = 0x000000,
		BLUE         = 0x0000FF,
		BROWN        = 0x964B00,
		BUFF         = 0xF0DC82,
		COFFEE       = 0x6F4E37,
		CYAN         = 0x00FFFF,
		FOREST_GREEN = 0x228B22,
		GRAY         = 0x808080,
		GREEN        = 0x00FF00,
		MAGENTA      = 0xFF00FF,
		ORANGE       = 0xFF7F00,
		PINK         = 0xFFC0CB,
		RED          = 0xFF0000,
		VIOLET       = 0xEE82EE,
		WHITE        = 0xFFFFFF,
		YELLOW       = 0xFFFF00,
		YELLOW_GREEN = 0x9ACD32,
	};

	ubyte r; /**< The Red component. */
	ubyte g; /**< The Green component. */
	ubyte b; /**< The Blue component. */
	ubyte a; /**< The Alpha (opacity) component. */

	/**
	* Construct a Color by Name.
	*
	* @param name the Name of the Color.
	*/
	Color(Name name);

	/**
	* Construct a Color with RGBA values.
	*
	* @param r the Red component.
	* @param g the Green component.
	* @param b the Blue component.
	* @param a the Alpha component.
	*/
	Color(ubyte r = 0, ubyte g = 0, ubyte b = 0, ubyte a = 255);

	/**
	* Construct a Color from a Gradient shade.
	*
	* @param color1 the starting color at 0.
	* @param color2 the ending color at 1.
	* @param t the Color between color1 and color2 mapped from 0 to 1.
	* @return the new Color.
	*/
	static Color gradient(Color color1, Color color2, float t);

	/**
	* Construct a Color from an HSV value.
	*
	* @param h the hue component.
	* @param s the saturation component.
	* @param v the value component.
	* @return the new Color.
	*/
	static Color hsv(float h, float s, float v);

	/**
	* Get the normalized Alpha value of the Color from 0 to 1.
	*
	* @return the normalized Alpha value.
	*/
	float normalizedA() const;

	/**
	* Get the normalized Blue value of the Color from 0 to 1.
	*
	* @return the normalized Blue value.
	*/
	float normalizedB() const;

	/**
	* Get the normalized Green value of the Color from 0 to 1.
	*
	* @return the normalized Green value.
	*/
	float normalizedG() const;

	/**
	* Get the normalized Red value of the Color from 0 to 1.
	*
	* @return the normalized Red value.
	*/
	float normalizedR() const;

	/**
	* Construct a Color from a normalized RGB value.
	*
	* @param r the red component.
	* @param g the green component.
	* @param b the blue component.
	* @return the new Color.
	*/
	static Color rgb(float r, float g, float b);

	/**
	* Convert the Color to an ARGB integer.
	*
	* @return the Color as an ARGB integer.
	*/
	unsigned int toInteger() const;

	/**
	* Convert the Color to a readable string.
	*
	* @return the Color in the format "#AARRGGBB".
	*/
	std::string toString() const;

private:
	void init(ubyte r, ubyte g, ubyte b);
};

/**
* Represents a Gradient shade.
*
* @todo this should be moved to a separate file.
*/
class Gradient
{
public:
	/**
	* Construct the Gradient with an initial color at 0 and 1.
	*
	* @param minColor the minimum Color value at 0.
	* @param maxColor the maximum Color value at 1.
	*/
	Gradient(Color minColor, Color maxColor);

	/**
	* Add a Color to the Gradient.
	*
	* @param color the Color to add.
	* @param location the position between 0 and 1 to add the color for shading.
	*/
	void addColor(Color color, float location);

	/**
	* Get the Color of the Gradient at a specific position.
	*
	* @param t the position in the Gradient between 0 and 1.
	* @return the Color at the position.
	*/
	Color getColor(float t);

private:
	struct ColorPoint
	{
		Color color;
		float location;

		bool operator < (const ColorPoint& p) const;
	};

	std::vector<ColorPoint> colorArray;
};

#endif // COLOR_HPP
