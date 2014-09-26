#include "Camera.hpp"
#include "Globals.hpp"
#include "Math.hpp"

const Vector2<double>& Camera::getPosition() const
{
	return position;
}

void Camera::setPosition( double x, double y )
{
	position.x = x;
	position.y = y;
}

void Camera::setSpeed( double xSpeed, double ySpeed )
{
	speed.x = xSpeed;
	speed.y = ySpeed;
}

void Camera::setTarget( double x, double y )
{
	target.x = x;
	target.y = y;
}

void Camera::setXTarget( double x )
{
	target.x = x;
}

void Camera::setYTarget( double y )
{
	target.y = y;
}

void Camera::update()
{
	if( std::fabs(position.x - target.x) < speed.x * GAME_DELTA )
	{
		position.x = target.x;
	}
	else
	{
		position.x += speed.x * sign(target.x - position.x) * GAME_DELTA;
	}

	if( std::fabs(position.y - target.y) < speed.y * GAME_DELTA )
	{
		position.y = target.y;
	}
	else
	{
		position.y += speed.y * sign(target.y - position.y) * GAME_DELTA;
	}
}
