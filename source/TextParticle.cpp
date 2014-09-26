#include "Text.hpp"
#include "TextParticle.hpp"

TextParticle::TextParticle( const std::string& text, double duration ) :
	Particle( nullptr, duration ),
	text(text)
{
	// These are defaults
	disableCollisions();
	disableGravity();
}

void TextParticle::onRender()
{
	drawTextScaled(text);
}
