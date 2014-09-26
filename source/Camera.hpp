#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vector2.hpp"

/**
 * Handles player camera movement.
 */
class Camera
{
public:
	const Vector2<double>& getPosition() const;
	void setPosition( double x, double y );
	void setSpeed( double xSpeed, double ySpeed );
	void setTarget( double x, double y );
	void setXTarget( double x );
	void setYTarget( double y );
	void update();

private:
	Vector2<double> position;
	Vector2<double> speed;
	Vector2<double> target;
};

#endif // CAMERA_HPP
