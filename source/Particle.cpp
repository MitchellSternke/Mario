#include "Particle.hpp"

Particle::Particle( const Animation* animation, bool destroyOnAnimationEnd ) :
	destroyOnAnimationEnd(destroyOnAnimationEnd),
	duration(-1.0),
	initialDelay(-1.0)
{
	if( destroyOnAnimationEnd )
	{
		playAnimation(animation);
		duration = animation->getDuration();
	}
	else
	{
		playAnimation(animation, animation);
	}
	//initialize(animation);
}

Particle::Particle( const Animation* animation, double initialDelay, bool destroyOnAnimationEnd ) :
	animationToPlay(animation),
	destroyOnAnimationEnd(destroyOnAnimationEnd),
	duration(-1.0),
	initialDelay(initialDelay)
{
	initialize(animation); // for now, this is only used on coin sparkles.
}

Particle::Particle( const Animation* animation, double duration ) :
	destroyOnAnimationEnd(false),
	duration(duration),
	initialDelay(-1.0)
{
	playAnimation(animation, animation);
	//initialize(animation);
}

void Particle::initialize( const Animation* animation )
{
	// Set our size to be the size of the first frame of the animation
	if( animation != nullptr )
	{
		auto f = animation->getFrame(0);
		setWidth(f.width);
		setHeight(f.height);
	}
}

void Particle::onPlayAnimationEnd()
{
	if( destroyOnAnimationEnd )
	{
		kill();
	}
}

void Particle::onPreUpdate()
{
	// Kill the particle if it has been alive for the set duration
	if( duration != -1.0 && getAge() > duration )
	{
		kill();
	}

	// If we had an initial delay, play the animation once the delay is over
	if( initialDelay != -1.0 && getAge() > initialDelay )
	{
		if( destroyOnAnimationEnd )
		{
			playAnimation(animationToPlay);
			duration = animationToPlay->getDuration() + initialDelay;
		}
		else
		{
			playAnimation(animationToPlay, animationToPlay);
		}
		initialDelay = -1.0;
	}
}
