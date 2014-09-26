#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>

#include "Animation.hpp"
#include "Enums.hpp"
#include "Vector2.hpp"

class ResourceManager;
class World;

/**
 * Base class for any object in the Game.
 */
class Entity
{
	friend class World;
public:
	Entity();
	virtual ~Entity() {}

	/**
	 * Get the current animation being rendered for the Entity.
	 */
	const Animation* getActiveAnimation() const;

	/**
	 * Get the current frame of the animation being rendered for the Entity.
	 */
	const Animation::Frame* getActiveAnimationFrame(int frameNumber);

	/**
	 * Get the age of the Entity since it has spawned, in seconds.
	 */
	double getAge() const;

	/**
	 * Get the bottom coordinate of the bounding box.
	 */
	double getBottom() const;

	/**
	 * Get the x coordinate of the center of the bounding box.
	 */
	double getCenterX() const;

	/**
	 * Get the y coordinate of the center of the bounding box.
	 */
	double getCenterY() const;

	/**
	 * Get the height of the bounding box.
	 */
	virtual double getHeight() const =0;

	/**
	 * Get the left coordinate of the bounding box.
	 */
	double getLeft() const;

	/**
	 * Get the ResourceManager used by the Entity to load its resources.
	 */
	const ResourceManager* getResourceManager() const;

	/**
	 * Get the right coordinate of the bounding box.
	 */
	double getRight() const;

	/**
	 * Get the top coordinate of the bounding box.
	 */
	double getTop() const;

	/**
	 * Get the width of the bounding box.
	 */
	virtual double getWidth() const =0;

	/**
	 * Get the World that the Entity is a part of.
	 */
	const World& getWorld() const;

	/**
	 * Get the x coordinate of the entity.
	 * @todo make this protected.
	 */
	virtual double getX() const =0;

	/**
	 * Get the y coordinate of the entity.
	 * @todo make this protected.
	 */
	virtual double getY() const =0;

	/**
	 * Set the alpha (opacity) of the Entity used when rendering.
	 */
	void setAlpha( float alpha );

	/**
	 * Set the blue color mask used for rendering.
	 */
	void setBlueMask( float mask );

	/**
	 * Set the green color mask used for rendering.
	 */
	void setGreenMask( float mask );

	/**
	 * Set the layer that the Entity is rendered on. Lower layers are rendered
	 * first.
	 */
	virtual void setLayer( int layer )=0;

	/**
	 * Set the orientation of the Entity for rendering. This only affects one
	 * axis per call.
	 */
	void setOrientation( Direction direction );

	/**
	 * Set the red color mask used for rendering.
	 */
	void setRedMask( float mask );

	/**
	 * Set the resource manager used by the entity to load its resources.
	 */
	void setResourceManager( const ResourceManager* resourceManager );

	/**
	 * Set the x offset used for rendering.
	 */
	void setXOffset( double xOffset );

	/**
	 * Set the y offset used for rendering.
	 */
	void setYOffset( double yOffset );

protected:
	int layer;

	const Animation* getAnimation( const std::string& name ) const;
	World& getWorld();
	virtual void onPlayAnimationEnd() {}
	virtual void onRender() {}
	void playAnimation( const Animation* animation, const Animation* nextAnimation = nullptr );
	void playAnimation( const std::string& name );
	void playAnimation( const std::string& name, const std::string& nextAnimation );
	void playMusic( const std::string& name, bool loop = true ) const;
	void playSound( const std::string& name, int channel = -1 ) const;
	void setAnimation( const Animation* animation );
	void setAnimation( const std::string& name );

	/**
	 * Pause (or unpause) the current animation.
	 *
	 * @param paused whether the animation should be paused.
	 * @param pauseFrame the frame that it should be paused at.
	 */
	void setAnimationPaused( bool paused, int pauseFrame = 0 );

private:
	const Animation* activeAnimation;
	const Animation* queuedAnimation;
	int animationStartFrame;
	int animationEndFrame;
	bool playingAnimation;
	bool startedPlayingAnimation;
	int firstFrame;
	bool horizontalOrientation; /**< False for left, true for right. */
	bool verticalOrientation;   /**< False for down, true for up. */
	bool animationPaused; /**< Whether the current animation is paused. */
	int pauseFrame; /**< The frame that the animation was paused on. */

	float alpha;
	float redMask;
	float greenMask;
	float blueMask;
	Vector2<double> offset; /**< Offset used for rendering. */

	const ResourceManager* resourceManager;

	World* world;
};

#endif // ENTITY_HPP
