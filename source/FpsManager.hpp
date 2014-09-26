#ifndef FPSMANAGER_HPP
#define FPSMANAGER_HPP

/**
* FPS (frames per second) manager.
*/
class FpsManager
{
public:
	/**
	* Constructor.
	*
	* @param fps the desired frames per second to achieve.
	*/
	FpsManager(float fps);

	/**
	 * Get the desired frame rate set when the manager was constructed.
	 */
	float getDesiredFrameRate() const;

	/**
	* Get the current frame rate in frames per second.
	*
	* @return the current frame rate.
	*/
	float getFrameRate() const;

	/**
	* Manage frames per second. This will sleep the calling thread
	* automatically if there is time until the next frame.
	*
	* @return if a frame had to be skipped without delay.
	*/
	bool update();

private:
	float currentFps;
	float desiredFps;
	unsigned fpsTicks;
	unsigned frameCount;
	unsigned ticks;
};

#endif // FPSMANAGER_HPP
