/**
 * @file
 * @brief defines math stuff.
 */
#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

#include "Vector2.hpp"
#include "Vector3.hpp"

//Constants
static const double PI = 4.0 * std::atan(1.0); /** Pi. */

//Non-Templated funcitons

/**
* This performs a modulus that has the appearance of a sawtooth wave for all numbers.
* @todo this needs a better name.
*/
int saw(int a, int b);

//Templated functions

/**
 * Clamp a value to a range.
 *
 * @param min the minimum value.
 * @param max the maxmimum value.
 */
template <typename T>
T clamp(T value, T min, T max)
{
	if( value < min )
	{
		return min;
	}
	if( value > max )
	{
		return max;
	}
	return value;
}

/**
 * Clamp a value to a domain, wrapping around when the range is exceeded.
 *
 * @param value the value to clamp.
 * @param domain the maximum value that the value can have before being
 * modified.
 */
template <typename T>
void clampDomain(T& value, unsigned domain)
{
	//Adjust for negative values by sticking them in a sawtooth function
	//and by adding the fractional part back in
	value = (T)saw((int)std::floor(value), domain) + (value - std::floor(value));
}

/**
 * Calculate the cross product of two vectors.
 *
 * @param u the first vector.
 * @param v the second vector.
 * @return the cross product.
 */
template <typename T>
Vector3<T> cross( const Vector3<T>& u, const Vector3<T>& v )
{
	Vector3<T> t;
	t.x = (u.y * v.z) - (u.z * v.y);
	t.y = (u.z * v.x) - (u.x * v.z);
	t.z = (u.x * v.y) - (u.y * v.x);
	return t;
}

/**
 * Calculate the dot product of two vectors.
 *
 * @param u the first vector.
 * @param v the second vector.
 * @return the dot product.
 */
template <typename T>
T dot( const Vector2<T>& u, const Vector2<T>& v )
{
	return (u.x * v.x) + (u.y * v.y);
}

/**
 * Calculate the dot product of two vectors.
 *
 * @param u the first vector.
 * @param v the second vector.
 * @return the dot product.
 */
template <typename T>
T dot( const Vector3<T>& u, const Vector3<T>& v )
{
	return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

/**
* This smooths out a value from 0 to 1 in the same range as a curve.
*
* @param x the value.
* @return the faded value.
*/
template <typename T>
T fade(T x)
{
	// 6x^15 - 15x^4 + 10x^3
	return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

/**
 * Fast floor to integer.
 */
template <typename T>
int fastFloor( T x )
{
	int i = (int)x;     // truncate
	return i - (i > x); // convert to floor for negatives
}

/**
 * Get the fractional part of a floating-point number.
 *
 * @param value the value.
 * @return the fractional part of the value.
 */
template <typename T>
T fraction( T value )
{
	double dummy;
	return modf(value, &dummy);
}

/**
 * Perform linear interpolation between two values.
 *
 * @param a the lower value.
 * @param b the upper value.
 * @param x the weight between the two values in the range [0, 1].
 * @return the interpolated value.
 */
template <typename T>
T lerp(T a, T b, T x)
{
	return a * (1.0 - x) + b * x;
}

/**
 * Convert a value in the range [-1, 1] to one in the range [0, 1].
 *
 * @param n the value.
 * @return the converted value.
 */
template <typename T>
T normalizePositive(T n)
{
	return (n + 1.0) / 2.0;
}

/**
 * Convert a value in the range [0, 1] to one in the range [-1, 1].
 *
 * @param n the value.
 * @return the converted value.
 */
template <typename T>
T normalizeReal(T n)
{
	return (n * 2.0) - 1.0;
}

/**
 * Get the sign of a number.
 *
 * @param n the value.
 * @return the sign of the number as -1 or 1.
 */
template <typename T>
T sign(T n)
{
	if( n < 0 )
	{
		return -1;
	}
	return 1;
}

#endif // MATH_HPP
