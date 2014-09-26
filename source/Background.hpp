#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

class Animation;
class Texture;

/**
 * Background tiling modes.
 */
enum BackgroundTiling
{
	BACKGROUND_HORIZONTAL, /**< Horizontal tiling. */
	BACKGROUND_VERTICAL,   /**< Vertical tiling. */
	BACKGROUND_ALL         /**< Horizontal and vertical tiling. */
};

/**
 * A background image resource for a level.
 */
class Background
{
public:
	/**
	 * Create a new background image.
	 *
	 * @param texture the Texture to use.
	 * @param animation the Animation to use.
	 * @param tiling the BackgroundTiling mode to use when rendering.
	 */
	Background( const Texture& texture, Animation* animation, BackgroundTiling tiling );

	~Background();

	/**
	 * Get the Animation used by the background.
	 */
	const Animation& getAnimation() const;

	/**
	 * Get the Texture used by the background.
	 */
	const Texture& getTexture() const;

	/**
	 * Get the tiling mode used when rendering.
	 */
	BackgroundTiling getTiling() const;

private:
	const Texture& texture;
	Animation* backgroundAnimation;
	BackgroundTiling tiling;
};

#endif // BACKGROUND_HPP
