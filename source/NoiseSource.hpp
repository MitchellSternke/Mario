#ifndef NOISESOURCE_HPP
#define NOISESOURCE_HPP

/**
 * A source of pseudorandom noise.
 *
 * @tparam T the type to use for noise that is returned.
 */
template <typename T>
class NoiseSource
{
public:
	virtual ~NoiseSource() {}

	/**
	 * Fractal brownian motion.
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param z the z coordinate.
	 * @param octaves the number of iterations to perform.
	 * @param persistance the amount to modify amplitude by each octave.
	 * @param lacunarity the amount to modify frequency by each octave.
	 * @return the value of FBM with the given parameters.
	 */
	T fbm(T x, T y = 0, T z = 0, unsigned int octaves = 4, T persistance = 0.5, T lacunarity = 2) const
	{
		T total = 0;
		T amplitude = persistance;
		T frequency = 1;
		T max = 0;

		for( unsigned int i = 0; i < octaves; i++ )
		{
			total += noise((T)x * frequency, (T)y * frequency, (T)z * frequency) * amplitude;
			max += amplitude;
			frequency *= lacunarity;
			amplitude *= persistance;
		}

		return total / max;
	}

	/**
	 * Get noise at (x, y, z).
	 *
	 * @param x the x coordinate.
	 * @param y the y coordinate.
	 * @param z the z coordinate.
	 * @return the noise value at (x, y, z).
	 */
	virtual T noise(T x, T y = 0, T z = 0) const =0;
};

#endif // NOISESOURCE_HPP
