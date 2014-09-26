#include <algorithm>
#include <cmath>

#include <boost/format.hpp>

#include "Color.hpp"
#include "Math.hpp"

using boost::format;
using std::floor;
using std::sort;
using std::string;
using std::vector;

Color::Color(Color::Name name) :
	a(0xFF)
{
	r = (static_cast<unsigned int>(name) & 0xFF0000) >> 16;
	g = (static_cast<unsigned int>(name) & 0x00FF00) >> 8;
	b = static_cast<unsigned int>(name) & 0x0000FF;
}

Color::Color(ubyte r, ubyte g, ubyte b, ubyte a)
{
	init(r, g, b);
	this->a = a;
}

Color Color::gradient(Color color1, Color color2, float t)
{
	Color c;
	c.r = static_cast<ubyte>(lerp<float>(color1.r, color2.r, t));
	c.g = static_cast<ubyte>(lerp<float>(color1.g, color2.g, t));
	c.b = static_cast<ubyte>(lerp<float>(color1.b, color2.b, t));
	return c;
}

Color Color::hsv(float h, float s, float v)
{
	Color color;

	if( s == 0.0f )
		color = Color::rgb(v, v, v);
	else
	{
		h *= 5; // Sector 0 to 5
		int i = floor(h);
		float f = h - i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch( i )
		{
			case 0:
				color = Color::rgb(v, t, p);
				break;

			case 1:
				color = Color::rgb(q, v, p);
				break;

			case 2:
				color = Color::rgb(p, v, t);
				break;

			case 3:
				color = Color::rgb(p, q, v);
				break;

			case 4:
				color = Color::rgb(t, p, v);
				break;

			default:
			case 5:
				color = Color::rgb(v, p, q);
				break;
		}
	}

	return color;
}

void Color::init(ubyte r, ubyte g, ubyte b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

float Color::normalizedA() const
{
	return static_cast<float>(this->a) / 255.0f;
}

float Color::normalizedB() const
{
	return static_cast<float>(this->b) / 255.0f;
}

float Color::normalizedG() const
{
	return static_cast<float>(this->g) / 255.0f;
}

float Color::normalizedR() const
{
	return static_cast<float>(this->r) / 255.0f;
}

Color Color::rgb(float r, float g, float b)
{
	Color color;
	color.r = floor(r * 255.0f);
	color.g = floor(g * 255.0f);
	color.b = floor(b * 255.0f);
	return color;
}

unsigned int Color::toInteger() const
{
	return (a & 0xFF000000) + (r & 0x00FF0000) + (g & 0x0000FF00) + (b & 0x000000FF);
}

string Color::toString() const
{
	return (format("#%02x%02x%02x%02x") %
		static_cast<unsigned short>(a) %
		static_cast<unsigned short>(r) %
		static_cast<unsigned short>(g) %
		static_cast<unsigned short>(b)).str();
}

bool Gradient::ColorPoint::operator < (const Gradient::ColorPoint& p) const
{
	return (this->location < p.location);
}

Gradient::Gradient(Color minColor, Color maxColor)
{
	addColor(minColor, 0.0f);
	addColor(maxColor, 1.0f);
}

void Gradient::addColor(Color color, float location)
{
	ColorPoint p;
	p.color = color;
	p.location = location;
	this->colorArray.push_back(p);
	sort(this->colorArray.begin(), this->colorArray.end());
}

Color Gradient::getColor(float t)
{
	if( t <= 0.0 )
		return this->colorArray.front().color;
	if( t >= 1.0 )
		return this->colorArray.back().color;

	for( vector<ColorPoint>::iterator it = this->colorArray.begin(); it != this->colorArray.end(); ++it )
	{
		if( (*it).location > t )
		{
			vector<ColorPoint>::iterator j = it;
			j--;
			ColorPoint& p1 = (*j);
			ColorPoint& p2 = (*it);
			return Color::gradient(p1.color, p2.color, (t - p1.location) / (p2.location - p1.location));
		}
	}

	//Default
	return Color::BLACK;
}
