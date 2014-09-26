#ifndef PERLINNOISESOURCE_HPP
#define PERLINNOISESOURCE_HPP

#include <cmath>

#include <boost/multi_array.hpp>

#include "Math.hpp"
#include "NoiseSource.hpp"
#include "Random.hpp"
#include "Vector3.hpp"

/**
 * A noise source that uses the perlin noise algorithm.
 */
template <typename T>
class PerlinNoiseSource : public NoiseSource<T>
{
public:
	/**
	 * Create a perlin noise source.
	 *
	 * @param size the range in each dimension that will be unique.
	 * @param rng the random number generator to use.
	 */
	PerlinNoiseSource(unsigned int size, Random& rng) :
		gradientArray(boost::extents[size][size][size]),
		size(size)
	{
		//Generate random gradients
		for( unsigned int x = 0; x < size; x++ )
		{
			for( unsigned int y = 0; y < size; y++ )
			{
				for( unsigned int z = 0; z < size; z++ )
				{
					Vector3<T> gradient( normalizeReal(rng.nextReal()), normalizeReal(rng.nextReal()), normalizeReal(rng.nextReal()) );
					gradient.normalize();
					this->gradientArray[x][y][z] = gradient;
				}
			}
		}
	}

	T noise(T x, T y = 0, T z = 0) const
	{
		clampDomain<T>(x, size);
		clampDomain<T>(y, size);
		clampDomain<T>(z, size);

		//Determine the cell that we are in
		T xi, yi, zi;
		T xf = std::modf(x, &xi);
		T yf = std::modf(y, &yi);
		T zf = std::modf(z, &zi);

		int a[] = {static_cast<int>(xi) % static_cast<int>(size),
			static_cast<int>(xi + 1) % static_cast<int>(size)};
		int b[] = {static_cast<int>(yi) % static_cast<int>(size),
			static_cast<int>(yi + 1) % static_cast<int>(size)};
		int c[] = {static_cast<int>(zi) % static_cast<int>(size),
			static_cast<int>(zi + 1) % static_cast<int>(size)};

		//Compute dot products
		T dots[2][2][2];
		for( int i = 0; i < 2; i++ )
		{
			for( int j = 0; j < 2; j++ )
			{
				for( int k = 0; k < 2; k++ )
				{
					dots[i][j][k] = dot(this->gradientArray[a[i]][b[j]][c[k]], Vector3<T>(xf - (T)i, yf - (T)j, zf - (T)k));
				}
			}
		}

		//Compute fade in each dimension
		T fadeX = fade<T>(xf);
		T fadeY = fade<T>(yf);
		T fadeZ = fade<T>(zf);

		//Perform trilinear interpolation
		T ix1 = lerp<T>(dots[0][0][0], dots[1][0][0], fadeX);
		T ix2 = lerp<T>(dots[0][1][0], dots[1][1][0], fadeX);
		T ix3 = lerp<T>(dots[0][0][1], dots[1][0][1], fadeX);
		T ix4 = lerp<T>(dots[0][1][1], dots[1][1][1], fadeX);

		T iy1 = lerp<T>(ix1, ix2, fadeY);
		T iy2 = lerp<T>(ix3, ix4, fadeY);

		return lerp<T>(iy1, iy2, fadeZ);
	}

private:
	boost::multi_array<Vector3<T>, 3> gradientArray;
	unsigned int size;
};

#endif // PERLINNOISESOURCE_HPP
