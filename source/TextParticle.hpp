#ifndef TEXTPARTICLE_HPP
#define TEXTPARTICLE_HPP

#include "Particle.hpp"

/**
 * A Particle that renders text. By default, text particles have collisions
 * and gravity disabled.
 */
class TextParticle : public Particle
{
public:
	/**
	 * Create a new text particle.
	 *
	 * @param text the string of text to render.
	 * @param duration how long the text particle should live for.
	 */
	TextParticle( const std::string& text, double duration );

private:
	std::string text;

	void onRender();
};

#endif // TEXTPARTICLE_HPP
