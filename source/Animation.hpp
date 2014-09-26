#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>

/**
 * A renderable graphic made up of one or more Frames.
 */
class Animation
{
public:
	/**
	 * Represents an individual Frame of an Animation.
	 */
	struct Frame
	{
		// Texture atlas position:
		double left;
		double right;
		double bottom;
		double top;

		// Rendering parameters:
		double xOffset;
		double yOffset;
		double width;
		double height;
	};

	/**
	 * Default constructor.
	 */
	Animation();

	/**
	 * Create an animation with set orientations.
	 */
	Animation( bool horizontalOrientation, bool verticalOrientation );

	/**
	 * Add a new frame to the animation.
	 *
	 * @param left the left texture coordinate.
	 * @param right the right texture coordinate.
	 * @param bottom the bottom texture coordinate.
	 * @param top the top texture coordinate.
	 * @param xOffset how far left/right to render off-center.
	 * @param yOffset how far up/down to render off-center.
	 * @param width the width of the frame, in world units.
	 * @param height the height of the frame, in world units.
	 * @param the duration of the frame, in game frames (60 fps).
	 */
	void addFrame(
		double left,
		double right,
		double bottom,
		double top,
		double xOffset,
		double yOffset,
		double width,
		double height,
		int duration = 1
	);

	/**
	 * Add a new frame to the animation.
	 *
	 * @param frame the Frame to add.
	 * @param duration the duration of the frame.
	 */
	void addFrame( const Frame& frame, int duration = 1 );

	/**
	 * Get a frame from the animation.
	 *
	 * @param frameNumber the sequence number of the frame, starting at zero.
	 * This loops/wraps around by default if the number is larger than the
	 * total number of frames.
	 * @see addFrame for documentation on other parameters.
	 */
	void getFrame(
		int frameNumber,
		double& left,
		double& right,
		double& bottom,
		double& top,
		double& xOffset,
		double& yOffset,
		double& width,
		double& height
	) const;

	/**
	 * Get the duration of the animation, in seconds.
	 */
	double getDuration() const;

	/**
	 * Get a frame from the animation.
	 *
	 * @see the other version of getFrame for documentation.
	 */
	const Frame& getFrame(int frameNumber) const;

	/**
	 * Get a frame by its absolute sequence in the Animation.
	 */
	const Frame& getFrameBySequence( int sequence ) const;

	/**
	 * Get the horizontal orientation of the animation.
	 *
	 * @retval false for left (default).
	 * @retval true for right.
	 */
	bool getHorizontalOrientation() const;

	/**
	 * Get the length of the Animation, in frames.
	 */
	int getLength() const;

	/**
	 * Get the vertical orientation of the animation.
	 *
	 * @retval false for down.
	 * @retval true for up (default).
	 */
	bool getVerticalOrientation() const;

	/**
	 * Render a frame of the animation.
	 *
	 * @param frame the Frame of the animation to render.
	 * @param mirrorX whether mirroring should be performed in the x direction.
	 * @param mirrorY whether mirroring should be performed in the y direction.
	 * @note this assumes that the current projection has positive y facing up,
	 * and that one unit = one pixel.
	 */
	void renderFrame( const Frame& frame, bool mirrorX = false, bool mirrorY = false ) const;

	/**
	 * Render a frame of the animation.
	 *
	 * @see getFrame() for frameNumber parameter documentation.
	 * @see renderFrame() for mirrorX and mirrorY parameter documentation.
	 */
	void renderFrame( int frameNumber = 0, bool mirrorX = false, bool mirrorY = false ) const;

private:
	std::vector<Frame> frames;
	bool horizontalOrientation; /**< False for left, true for right. */
	bool verticalOrientation; /**< False for down, true for up. */
};

#endif // ANIMATION_HPP
