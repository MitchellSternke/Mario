#ifndef BLASTER_HPP
#define BLASTER_HPP

#include "Tile.hpp"

/**
 * A launcher for Bullet objects.
 */
class Blaster : public Tile
{
public:
	Blaster();

private:
	int lastBlastFrame;

	void onInit();
	void onRender();
};

#endif // BLASTER_HPP
