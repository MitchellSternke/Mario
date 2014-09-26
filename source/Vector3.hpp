#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>
#include <sstream>
#include <string>

#include "Vector2.hpp"

/**
 * A 3-dimensional vector.
 *
 * @tparam T the type to use for components.
 */
template <typename T>
struct Vector3
{
	T x; /**< The x component. */
	T y; /**< The y component. */
	T z; /**< The z component. */

	/**
	 * Construct a vector with components [0, 0, 0].
	 */
	Vector3()
	{
	}

	/**
	 * Construct a vector with components [x, y, z].
	 *
	 * @param x the x component.
	 * @param y the y component.
	 * @param z the z component.
	 */
	Vector3(T x, T y, T z) :
		x(x),
		y(y),
		z(z)
	{
	}

	Vector3( const Vector2<T>& v, T z = 0 ) :
		x(v.x),
		y(v.y),
		z(z)
	{
	}

	/**
	 * Get the magnitude of the vector.
	 */
	T getMagnitude() const
	{
		return static_cast<T>(std::sqrt( getMagnitudeSquared() ));
	}

	/**
	 * Get the magnitude of the vector squared.
	 */
	T getMagnitudeSquared() const
	{
		return (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
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
		str << "[" << this->x << " " << this->y << " " << this->z << "]";
		return str.str();
	}

	bool operator == (const Vector3<T>& v) const
	{
		return (this->x == v.x && this->y == v.y && this->z == v.z);
	}

	bool operator != (const Vector3<T>& v) const
	{
		return !((*this) == v);
	}

	bool operator < (const Vector3<T>& v) const
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
			else if( this->y == v.y )
			{
				if( this->z < v.z )
				{
					return true;
				}
			}
		}

		return false;
	}

	void operator += (const Vector3<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
	}

	void operator -= (const Vector3<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
	}

	void operator *= (T value)
	{
		this->x *= value;
		this->y *= value;
		this->z *= value;
	}

	void operator /= (T value)
	{
		this->x /= value;
		this->y /= value;
		this->z /= value;
	}

	Vector3<T> operator + (const Vector3<T>& v) const
	{
		Vector3<T> u = *this;
		u += v;
		return u;
	}

	Vector3<T> operator - (const Vector3<T>& v) const
	{
		Vector3<T> u = *this;
		u -= v;
		return u;
	}

	Vector3<T> operator * (T value) const
	{
		Vector3<T> u = *this;
		u *= value;
		return u;
	}

	Vector3<T> operator / (T value) const
	{
		Vector3<T> u = *this;
		u /= value;
		return u;
	}
};

#endif // VECTOR3_HPP
