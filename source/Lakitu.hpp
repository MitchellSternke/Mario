#ifndef LAKITU_HPP
#define LAKITU_HPP

#include "Enemy.hpp"

/**
 * A cloud-riding enemy that throws spinies at the player.
 */
class Lakitu : public Enemy
{
public:
	static constexpr int THROW_DURATION = 30; /**< Number of frames that Lakitu spends throwing spinies. */

	Lakitu();

private:
	int throwTimer;

	void onPreUpdate();
};

#endif // LAKITU_HPP
