#ifndef PLANT_HPP
#define PLANT_HPP

#include "Enemy.hpp"

/**
 * Plants that come out of pipes.
 */
class Plant : public Enemy
{
public:
	/**
	 * Create a new plant.
	 *
	 * @param orientation the direction of the pipe that the plant will be in.
	 */
	Plant( Direction orientation = UP );

	static constexpr int PHASE_DURATION = 30; /**< The time that the plant spends in each phase. */
	static constexpr double WAIT_DISTANCE = 4.0; /**< The distance that the player needs to be within for a plant to wait in a pipe. */

private:
	/**
	 * The phases that the plant goes through.
	 */
	enum Phase
	{
		PHASE_WAIT,
		PHASE_EXIT,
		PHASE_ATTACK,
		PHASE_ENTER
	};

	Direction orientation;
	Phase phase;
	int phaseTimer;

	void onPreUpdate();
};

#endif // PLANT_HPP
