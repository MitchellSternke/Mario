#include "Entity.hpp"
#include "Globals.hpp"
#include "World.hpp"

Entity::Entity() :
	layer(0),
	activeAnimation(nullptr),
	queuedAnimation(nullptr),
	playingAnimation(false),
	startedPlayingAnimation(false),
	horizontalOrientation(false),
	verticalOrientation(true),
	animationPaused(false),
	alpha(1.0f),
	redMask(1.0f),
	greenMask(1.0f),
	blueMask(1.0f),
	world(nullptr)
{
	resourceManager = DEFAULT_RESOURCE_GROUP;
}

const Animation* Entity::getActiveAnimation() const
{
	return activeAnimation;
}

const Animation::Frame* Entity::getActiveAnimationFrame(int frameNumber)
{
	// If there is an active animation
	if( activeAnimation != nullptr && activeAnimation->getLength() > 0 )
	{
		// Is the animation paused?
		if( animationPaused )
		{
			return &activeAnimation->getFrame(pauseFrame);
		}

		// Start playing an animation if it was set to play before this frame
		if( playingAnimation && !startedPlayingAnimation )
		{
			animationStartFrame = frameNumber;
			animationEndFrame = frameNumber + activeAnimation->getLength();
			startedPlayingAnimation = true;
		}

		if( playingAnimation )
		{
			if( frameNumber >= animationEndFrame )
			{
				// Stop playing the animation
				activeAnimation = queuedAnimation;
				queuedAnimation = nullptr;
				playingAnimation = false;
				startedPlayingAnimation = false;

				onPlayAnimationEnd();
			}
			else
			{
				// Return the frame from the currently playing animation
				return &activeAnimation->getFrameBySequence( frameNumber - (animationEndFrame - activeAnimation->getLength()) );
			}
		}

		// Return the frame of the currently active animation
		if( activeAnimation != nullptr )
		{
			return &activeAnimation->getFrame(frameNumber);
		}
	}
	// If we can switch to a non-null queued animation, do it
	else if( queuedAnimation != nullptr )
	{
		activeAnimation = queuedAnimation;
		queuedAnimation = nullptr;
		playingAnimation = false;
		startedPlayingAnimation = false;
	}

	return nullptr;
}

const Animation* Entity::getAnimation( const std::string& name ) const
{
	return resourceManager->getAnimation(name);
}

double Entity::getAge() const
{
	if( world == nullptr )
	{
		return 0.0;
	}
	return static_cast<double>(world->getFrameNumber() - firstFrame) * world->getDelta();
}

double Entity::getBottom() const
{
	return getY();
}

double Entity::getCenterX() const
{
	return getX() + getWidth() / 2.0;
}

double Entity::getCenterY() const
{
	return getY() + getHeight() / 2.0;
}

double Entity::getLeft() const
{
	return getX();
}

const ResourceManager* Entity::getResourceManager() const
{
	return resourceManager;
}

double Entity::getRight() const
{
	return getX() + getWidth();
}

double Entity::getTop() const
{
	return getY() + getHeight();
}

const World& Entity::getWorld() const
{
	return *world;
}

World& Entity::getWorld()
{
	return *world;
}

void Entity::playAnimation( const Animation* animation, const Animation* nextAnimation )
{
	// Queue the current animation to play after this animation plays
	if( nextAnimation != nullptr )
	{
		queuedAnimation = nextAnimation;
	}
	else
	{
		queuedAnimation = activeAnimation;
	}
	playingAnimation = true;
	activeAnimation = animation;
}

void Entity::playAnimation( const std::string& name )
{
	playAnimation(getAnimation(name));
}

void Entity::playAnimation( const std::string& name, const std::string& nextAnimation )
{
	playAnimation(getAnimation(name), getAnimation(nextAnimation));
}

void Entity::playMusic( const std::string& name, bool loop ) const
{
	resourceManager->playMusic(name, loop);
}

void Entity::playSound( const std::string& name, int channel ) const
{
	resourceManager->playSound(name, channel);
}

void Entity::setAlpha( float alpha )
{
	this->alpha = alpha;
}

void Entity::setAnimation(const Animation* animation)
{
	// Queue the animation until the current one is done playing
	if( playingAnimation )
	{
		queuedAnimation = animation;
	}
	else
	{
		activeAnimation = animation;
	}

	// Also unpause the animation if we are setting a new one
	setAnimationPaused(false);
}

void Entity::setAnimation( const std::string& name )
{
	setAnimation( getAnimation(name) );
}

void Entity::setAnimationPaused( bool paused, int pauseFrame )
{
	animationPaused = paused;
	this->pauseFrame = pauseFrame;
}

void Entity::setBlueMask( float mask )
{
	blueMask = mask;
}

void Entity::setGreenMask( float mask )
{
	greenMask = mask;
}

void Entity::setOrientation( Direction orientation )
{
	switch( orientation )
	{
		case DOWN:
			verticalOrientation = false;
			break;

		case LEFT:
			horizontalOrientation = false;
			break;

		case RIGHT:
			horizontalOrientation = true;
			break;

		case UP:
			verticalOrientation = true;
			break;
	}
}

void Entity::setRedMask( float mask )
{
	redMask = mask;
}

void Entity::setResourceManager( const ResourceManager* resourceManager )
{
	this->resourceManager = resourceManager;
}

void Entity::setXOffset( double xOffset )
{
	offset.x = xOffset;
}

void Entity::setYOffset( double yOffset )
{
	offset.y = yOffset;
}
