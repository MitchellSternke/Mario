#include <algorithm>

#include <GL/gl.h>

#include "Animation.hpp"
#include "Globals.hpp"

Animation::Animation() :
	horizontalOrientation(false),
	verticalOrientation(true)
{
}

Animation::Animation( bool horizontalOrientation, bool verticalOrientation ) :
	horizontalOrientation(horizontalOrientation),
	verticalOrientation(verticalOrientation)
{
}

void Animation::addFrame(
	double left,
	double right,
	double bottom,
	double top,
	double xOffset,
	double yOffset,
	double width,
	double height,
	int duration
)
{
	// Duplicate frames that have a duration > 1
	for( int i = 0; i < duration; ++i )
	{
		Frame f;
		f.left = left;
		f.right = right;
		f.bottom = bottom;
		f.top = top;
		f.xOffset = xOffset;
		f.yOffset = yOffset;
		f.width = width;
		f.height = height;
		frames.push_back(f);
	}
}

void Animation::addFrame( const Frame& frame, int duration )
{
	addFrame( frame.left, frame.right, frame.bottom, frame.top, frame.xOffset, frame.yOffset, frame.width, frame.height, duration );
}

void Animation::getFrame(
	int frameNumber,
	double& left,
	double& right,
	double& bottom,
	double& top,
	double& xOffset,
	double& yOffset,
	double& width,
	double& height
) const
{
	const Frame& f = getFrame(frameNumber);
	left = f.left;
	right = f.right;
	bottom = f.bottom;
	top = f.top;
	xOffset = f.xOffset;
	yOffset = f.yOffset;
	width = f.width;
	height = f.height;
}

double Animation::getDuration() const
{
	return getLength() * GAME_DELTA;
}

const Animation::Frame& Animation::getFrame(int frameNumber) const
{
	unsigned int index = frameNumber % frames.size();
	return frames[index];
}

const Animation::Frame& Animation::getFrameBySequence(int sequence) const
{
	return frames[sequence];
}

bool Animation::getHorizontalOrientation() const
{
	return horizontalOrientation;
}

int Animation::getLength() const
{
	return static_cast<int>(frames.size());
}

bool Animation::getVerticalOrientation() const
{
	return verticalOrientation;
}

void Animation::renderFrame( const Frame& frame, bool mirrorX, bool mirrorY ) const
{
	double left = frame.left;
	double right = frame.right;
	double bottom = frame.bottom;
	double top = frame.top;

	// Swap texture coordinates if necessary
	if( mirrorX )
	{
		std::swap(left, right);
	}
	if( mirrorY )
	{
		std::swap(bottom, top);
	}

	// Render a quad
	glBegin(GL_QUADS);
		glTexCoord2d(left, bottom);
		glVertex2d(0.0, 0.0);
		glTexCoord2d(right, bottom);
		glVertex2d(frame.width * UNIT_SIZE, 0.0);
		glTexCoord2d(right, top);
		glVertex2d(frame.width * UNIT_SIZE, frame.height * UNIT_SIZE);
		glTexCoord2d(left, top);
		glVertex2d(0.0, frame.height * UNIT_SIZE);
	glEnd();
}

void Animation::renderFrame( int frameNumber, bool mirrorX, bool mirrorY ) const
{
	// Get the frame and render it
	const Frame& frame = getFrame(frameNumber);
	renderFrame(frame, mirrorX, mirrorY);
}
