#include "Animation.hpp"
#include "Background.hpp"

Background::Background( const Texture& texture, Animation* animation, BackgroundTiling tiling ) :
	texture(texture),
	backgroundAnimation(animation),
	tiling(tiling)
{
}

Background::~Background()
{
	delete backgroundAnimation;
}

const Animation& Background::getAnimation() const
{
	return *backgroundAnimation;
}

const Texture& Background::getTexture() const
{
	return texture;
}

BackgroundTiling Background::getTiling() const
{
	return tiling;
}
