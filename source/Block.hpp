#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Particle.hpp"
#include "Sprite.hpp"
#include "Tile.hpp"

class Player;
class Sound;

/**
 * Special block contents.
 */
enum BlockContents
{
	BLOCK_CONTENTS_NONE,
	BLOCK_FLOWER,
	BLOCK_LEAF
};

/**
 * The type of a Block.
 */
enum BlockType
{
	BLOCK_BRICK,   /**< A block made of bricks. */
	BLOCK_QUESTION /**< A question block. */
};

/**
 * A solid block.
 */
class Block : public Tile
{
public:
	static constexpr double BRICK_PARTICLE_GRAVITY = -50.0; /**< The gravity of a brick particle. */
	static constexpr double BRICK_PARTICLE_X_SPEED = 4.0;   /**< The speed that brick particles move at. */
	static constexpr double BRICK_PARTICLE_Y_SPEED = 16.0;  /**< The speed that brick particles move at. */

	static constexpr int BLOCK_COIN_POINTS = 100;   /**< The points that the Player gains for getting a coin out of a block. */
	static constexpr int BRICK_DESTROY_POINTS = 10; /**< The points that the Player gets for destroying a brick. */

	static constexpr double COIN_PARTICLE_DURATION = 0.5;   /**< The duration of a coin particle. */
	static constexpr double COIN_PARTICLE_GRAVITY = -60.0;  /**< The gravity of a coin particle. */
	static constexpr double COIN_PARTICLE_SPEED   = 18.5;  /**< The speed of a coin particle. */

	static constexpr double BLOCK_SPAWN_TIME = 1.0; /**< The time it takes for a block to spawn a sprite. */

	static constexpr int BLOCK_BUMP_FRAMES = 10; /**< The number of frames it takes for a block to be bumped. */

	static constexpr double BLOCK_BUMP_VELOCITY = 32.0; /**< The initial Y velocity applied to sprites on top of blocks when they are bumped. */

	/**
	 * Create a new block with a Sprite as contents.
	 *
	 * @param type the type of block.
	 * @param contents a sprite that will spawn when the block is hit from below.
	 */
	Block( BlockType type, Sprite* contents = nullptr );

	/**
	 * Create a new block with an item as contents.
	 *
	 * @param type the type of block.
	 * @param contents the best item type to spawn, based on the player hitting the block.
	 */
	Block( BlockType type, BlockContents contents );

	/**
	 * Create a new block with coins as contents.
	 *
	 * @param type the type of Block.
	 * @param coins the number of coins to contain.
	 * @param scatterCoins whether the coins should all spawn at once and
	 * scatter instead of coming out one at a time.
	 */
	Block( BlockType type, int coins, bool scatterCoins = false );

	~Block();

	/**
	 * Bump the block.
	 *
	 * @param strongBump whether the bump is strong enough to destroy a brick.
	 * @param fromPlayer whether the player bumped from underneith.
	 * @param bumper the player that bumped it, if any.
	 */
	void bump(bool strongBump, bool fromPlayer, Player* bumper);

	/**
	 * Get the type of the block.
	 */
	BlockType getBlockType() const;

private:
	class CoinParticle : public Particle
	{
	public:
		CoinParticle( Player* player );

	private:
		Player* player;

		void onDestroy();
	};

	class Spawner : public Sprite
	{
	public:
		Spawner( Sprite* sprite, int blockY );
		~Spawner();

	private:
		Sprite* sprite;
		int blockY;

		void onPreUpdate();
	};

	int bumpProgress;
	int coins;
	Sprite* contents;
	BlockContents specialContents;
	bool dead;
	int spawnTimer;
	BlockType type;

	const Animation* deadAnimation;
	const Animation* mainAnimation;

	void init();
	void onCollision(Sprite& sprite, Edge edge);
	void onRender();
};

#endif // BLOCK_HPP
