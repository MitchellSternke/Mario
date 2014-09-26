#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>
#include <sstream>

/**
 * A 2-dimensional vector.
 *
 * @tparam T the type to use for components.
 */
template <typename T>
struct Vector2
{
	T x; /**< The x component. */
	T y; /**< The y component. */

	/**
	 * Construct a vector with components [0, 0].
	 */
	Vector2()
	{
		this->x = 0;
		this->y = 0;
	}

	/**
	 * Construct a vector with components [x, y].
	 * @param x the x component.
	 * @param y the y component.
	 */
	Vector2(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	/**
	 * Get the magnitude of the vector.
	 *
	 * @return the magnitude of the vector.
	 */
	T getMagnitude() const
	{
		return static_cast<T>(std::sqrt( getMagnitudeSquared() ));
	}

	/**
	 * Get the magnitude of the vector squared.
	 *
	 * @return the squared magnitude of the vector.
	 */
	T getMagnitudeSquared() const
	{
		return x * x + y * y;
	}

	/**
	 * Normalize the vector.
	 */
	void normalize()
	{
		(*this) /= getMagnitude();
	}

	/**
	 * Convert the vector to a printable string.
	 */
	std::string toString() const
	{
		std::stringstream str;
		str << "[" << this->x << " " << this->y << "]";
		return str.str();
	}

	bool operator == (const Vector2<T>& v) const
	{
		return (this->x == v.x && this->y == v.y);
	}

	bool operator != (const Vector2<T>& v) const
	{
		return !((*this) == v);
	}

	bool operator < (const Vector2<T>& v) const
	{
		if( this->x < v.x )
		{
			return true;
		}
		else if( this->x == v.x )
		{
			if( this->y < v.y )
			{
				return true;
			}
		}

		return false;
	}

	void operator += (const Vector2<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
	}

	void operator -= (const Vector2<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
	}

	void operator *= (T value)
	{
		this->x *= value;
		this->y *= value;
	}

	void operator /= (T value)
	{
		this->x /= value;
		this->y /= value;
	}

	Vector2<T> operator + (const Vector2<T>& v) const
	{
		Vector2<T> u = *this;
		u += v;
		return u;
	}

	Vector2<T> operator - (const Vector2<T>& v) const
	{
		Vector2<T> u = *this;
		u -= v;
		return u;
	}

	Vector2<T> operator * (T value) const
	{
		Vector2<T> u = *this;
		u *= value;
		return u;
	}

	Vector2<T> operator / (T value) const
	{
		Vector2<T> u = *this;
		u /= value;
		return u;
	}
};

#endif // VECTOR2_HPP
