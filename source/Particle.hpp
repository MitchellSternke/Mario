#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Sprite.hpp"

/**
 * An animated effect sprite.
 */
class Particle : public Sprite
{
public:
	/**
	 * Create a new particle with infinite duration.
	 *
	 * @param animation the Animation to use for rendering.
	 * @param destroyOnAnimationEnd whether the particle should be destroyed when its animation is complete.
	 */
	Particle( const Animation* animation, bool destroyOnAnimationEnd = false );

	/**
	 * Create a new particle with an initial delay before it plays its animation.
	 *
	 * @param animation the Animation to use for rendering.
	 * @param initialDelay the initial delay, in seconds, to wait before playing the animation after spawning.
	 * @param destroyOnAnimationEnd whether the particle should be destroyed when its animation is complete.
	 */
	Particle( const Animation* animation, double initialDelay, bool destroyOnAnimationEnd );

	/**
	 * Create a new particle.
	 *
	 * @param animation the Animation to use for rendering.
	 * @param duration the length of time that the particle will live for.
	 */
	Particle( const Animation* animation, double duration );

private:
	const Animation* animationToPlay;
	bool destroyOnAnimationEnd;
	double duration;
	double initialDelay;

	void initialize( const Animation* animation );
	void onPlayAnimationEnd();
	void onPreUpdate();
};

#endif // PARTICLE_HPP
