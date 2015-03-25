#include "Block.hpp"
#include "Coin.hpp"
#include "Flower.hpp"
#include "Globals.hpp"
#include "Item.hpp"
#include "Leaf.hpp"
#include "Mushroom.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "Shell.hpp"
#include "Sound.hpp"
#include "World.hpp"

/**
 * For spawning multiple coins that scatter.
 *
 * @todo clean this up.
 */
class CoinSpawner : public Particle
{
public:
	CoinSpawner(int coins) :
		Particle(GET_ANIMATION("coin"), 0.0),
		coins(coins)
	{
	}

private:
	int coins;

	void onDestroy()
	{
		for( int i = 0; i < coins; i++ )
		{
			Coin* coin = new Coin;
			coin->setGravityEnabled(true);
			coin->setCollisionsEnabled(true);
			coin->setWidth(0.75);
			coin->setXVelocity((getWorld().getRandom().nextReal() * 2.0 - 1.0) * 4.0);
			coin->setCenterX(getCenterX());
			coin->setBottom(getBottom());
			getWorld().addSprite(coin);
		}
	}
};

Block::CoinParticle::CoinParticle( Player* player ) :
	Particle( GET_ANIMATION("coin_particle"), COIN_PARTICLE_DURATION ),
	player(player)
{
	disableGravity();
	disableCollisions();
	setYAcceleration(COIN_PARTICLE_GRAVITY);
	setYVelocity(COIN_PARTICLE_SPEED);
	setMaximumYSpeed(COIN_PARTICLE_SPEED);
	setAnimation("coin_particle");
}

void Block::CoinParticle::onDestroy()
{
	player->scorePoints( BLOCK_COIN_POINTS, getX(), getY() );
	player->gainCoins(1);

	Particle* p = new Particle( getAnimation("coin_explosion"), true );
	p->disableCollisions();
	p->disableGravity();
	p->setLayer(-1);
	getWorld().addSprite( getX(), getY(), p );
}

Block::Spawner::Spawner(Sprite* sprite, int blockY) :
	sprite(sprite),
	blockY(blockY)
{
	disableCollisions();
	disableGravity();
}

Block::Spawner::~Spawner()
{
	delete sprite;
}

void Block::Spawner::onPreUpdate()
{
	Sprite::onPreUpdate();

	// We only live for 1 second (plus the initial delay)
	if( getAge() > BLOCK_SPAWN_TIME + (BLOCK_BUMP_FRAMES / GAME_FPS))
	{
		// Time to spawn our sprite
		getWorld().addSprite(getCenterX() - sprite->getWidth() / 2.0, blockY + 1, sprite);
		sprite = nullptr;

		// Self destruct
		kill();
	}
	else if( getAge() > (BLOCK_BUMP_FRAMES / GAME_FPS) )
	{
		// Use the same animation as what is being spawned
		setAnimation(sprite->getActiveAnimation());

		setYVelocity(1.0);
	}
}

Block::Block(BlockType type, Sprite* contents) :
	Tile(TILE_SOLID),
	bumpProgress(0),
	coins(0),
	contents(contents),
	specialContents(BLOCK_CONTENTS_NONE),
	dead(false),
	type(type)
{
	init();
}

Block::Block(BlockType type, BlockContents contents) :
	Tile(TILE_SOLID),
	bumpProgress(0),
	coins(0),
	contents(nullptr),
	specialContents(contents),
	dead(false),
	type(type)
{
	init();
}

Block::Block(BlockType type, int coins, bool scatterCoins) :
	Tile(TILE_SOLID),
	bumpProgress(0),
	coins(coins),
	contents(nullptr),
	specialContents(BLOCK_CONTENTS_NONE),
	dead(false),
	type(type)
{
	if( scatterCoins )
	{
		contents = new CoinSpawner(coins);
		this->coins = 0;
	}

	init();
}

Block::~Block()
{
	delete contents;
}

void Block::bump( bool strongBump, bool fromPlayer, Player* bumper )
{
	// Blocks can only be bumped when alive
	if( dead )
	{
		return;
	}

	// Bump sprites that are above
	const std::set<Sprite*>* sprites = getWorld().getSprites(getXInt(), getYInt() + getHeightInt());
	if( sprites != nullptr )
	{
		for( auto sprite : *sprites )
		{
			// Only bump sprites that are on top of the block
			if( sprite->getBottom() != getTop() )
			{
				continue;
			}

			Enemy* enemy = dynamic_cast<Enemy*>(sprite);
			if( enemy != nullptr )
			{
				enemy->bump();
				if( bumper != nullptr )
				{
					bumper->scorePoints(MULTIPLE_KILL_POINT_SEQUENCE[0]);
				}
				playSound("kick");
				continue;
			}

			Item* item = dynamic_cast<Item*>(sprite);
			if( item != nullptr )
			{
				Coin* coin = dynamic_cast<Coin*>(sprite);
				if( coin != nullptr && fromPlayer )
				{
					coin->sparkle();
					coin->kill();
					bumper->gainCoins(1);
				}
				else
				{
					sprite->setYVelocity(BLOCK_BUMP_VELOCITY);
				}
				continue;
			}

			Player* player = dynamic_cast<Player*>(sprite);
			if( player != nullptr )
			{
				player->setYVelocity(BLOCK_BUMP_VELOCITY);
			}
		}
	}

	if( coins > 0 )
	{
		coins--;
		if( coins == 0 )
		{
			dead = true;
			setAnimation(deadAnimation);
		}
		bumpProgress = getWorld().getFrameNumber();

		Particle* coinParticle = new CoinParticle(bumper);
		coinParticle->setLayer(-1);
		getWorld().addSprite(getX(), getY(), coinParticle);

		if( fromPlayer )
		{
			bumper->setYVelocity( -0.5 * std::fabs(bumper->getYVelocity()) );
		}

		playSound("bump");
	}
	else if( contents != nullptr )
	{
		Spawner* spawner = new Spawner(contents, getY());
		spawner->setLayer(-1);
		getWorld().addSprite(getX(), getY(), spawner);
		contents = nullptr;
		dead = true;
		setAnimation(deadAnimation);
		bumpProgress = getWorld().getFrameNumber();
		playSound("sprout");

		if( fromPlayer )
		{
			bumper->setYVelocity( -0.5 * std::fabs(bumper->getYVelocity()) ); ///@todo this should be an acceleration
		}
	}
	else if( specialContents != BLOCK_CONTENTS_NONE )
	{
		Sprite* spriteToSpawn = nullptr;
		switch( bumper->getState() )
		{
		case Player::SMALL:
			spriteToSpawn = new Mushroom;
			break;
		case Player::SUPER:
		default:
			switch( specialContents )
			{
			case BLOCK_FLOWER:
				spriteToSpawn = new Flower;
				break;
			case BLOCK_LEAF:
				spriteToSpawn = new Leaf;
				break;
			default:
				break;
			}
			break;
		}
		specialContents = BLOCK_CONTENTS_NONE;

		// The following was copied from the above case
		///@todo make this a function
		Spawner* spawner = new Spawner(spriteToSpawn, getY());
		spawner->setLayer(-1);
		getWorld().addSprite(getX(), getY(), spawner);
		contents = nullptr;
		dead = true;
		setAnimation(deadAnimation);
		bumpProgress = getWorld().getFrameNumber();
		playSound("sprout");

		if( fromPlayer )
		{
			bumper->setYVelocity( -0.5 * std::fabs(bumper->getYVelocity()) ); ///@todo this should be an acceleration
		}
	}
	else if( !strongBump || type != BLOCK_BRICK )
	{
		//playAnimation(bumpAnimation, mainAnimation);
		bumpProgress = getWorld().getFrameNumber();
		playSound("bump");
	}
	else
	{
		// Give the player points
		bumper->scorePoints( BRICK_DESTROY_POINTS );

		// Create brick particles
		Particle* p = new Particle( getAnimation("brick_particle")/*, BRICK_PARTICLE_DURATION*/ );
		p->setXVelocity(BRICK_PARTICLE_X_SPEED);
		p->setYVelocity(BRICK_PARTICLE_Y_SPEED);
		p->setYAcceleration(BRICK_PARTICLE_GRAVITY);
		p->disableCollisions();
		p->disableGravity();
		getWorld().addSprite(getX(), getY(), p);
		p = new Particle( getAnimation("brick_particle")/*, BRICK_PARTICLE_DURATION*/ );
		p->setXVelocity(-1.0 * BRICK_PARTICLE_X_SPEED);
		p->setYVelocity(BRICK_PARTICLE_Y_SPEED);
		p->setYAcceleration(BRICK_PARTICLE_GRAVITY);
		p->disableCollisions();
		p->disableGravity();
		getWorld().addSprite(getX(), getY(), p);
		p = new Particle( getAnimation("brick_particle")/*, BRICK_PARTICLE_DURATION*/ );
		p->setXVelocity(BRICK_PARTICLE_X_SPEED);
		p->setYVelocity(BRICK_PARTICLE_Y_SPEED / 2.0);
		p->setYAcceleration(BRICK_PARTICLE_GRAVITY);
		p->disableCollisions();
		p->disableGravity();
		getWorld().addSprite(getX(), getY(), p);
		p = new Particle( getAnimation("brick_particle")/*, BRICK_PARTICLE_DURATION*/ );
		p->setXVelocity(-1.0 * BRICK_PARTICLE_X_SPEED);
		p->setYVelocity(BRICK_PARTICLE_Y_SPEED / 2.0);
		p->setYAcceleration(BRICK_PARTICLE_GRAVITY);
		p->disableCollisions();
		p->disableGravity();
		getWorld().addSprite(getX(), getY(), p);

		// Self-destruct
		playSound("brick_break");
		getWorld().destroyTile(this);
	}
}

BlockType Block::getBlockType() const
{
	return type;
}

void Block::init()
{
	switch( type )
	{
		case BLOCK_BRICK:
			mainAnimation = getAnimation("brick");
			break;

		case BLOCK_QUESTION:
			mainAnimation = getAnimation("question_block");
			break;
	}

	deadAnimation = getAnimation("block_dead");

	setAnimation(mainAnimation);
}

void Block::onCollision(Sprite& sprite, Edge edge)
{
	// Handle collisions with shells
	Shell* shell = dynamic_cast<Shell*>(&sprite);
	if( shell != nullptr )
	{
		if( (edge == EDGE_LEFT || edge == EDGE_RIGHT) && shell->getXVelocity() != 0.0 )
		{
			bump(true, false, shell->getPlayer());
		}

		return;
	}

	// Handle collisions with the player
	Player* player = dynamic_cast<Player*>(&sprite);
	if( player != nullptr )
	{
		if( edge == EDGE_BOTTOM && !player->isClimbingLadder() )
		{
			if( player->getState() != Player::SMALL )
			{
				bump(true, true, player);
			}
			else
			{
				bump(false, true, player);
			}
		}

		return;
	}
}

void Block::onRender()
{
	// Render the bumping effect by modifying the y offset
	if( bumpProgress > 0 )
	{
		int progress = getWorld().getFrameNumber() - bumpProgress;

		if( progress + 1 >= BLOCK_BUMP_FRAMES )
		{
			bumpProgress = 0;
		}
		else
		{
			double offset;
			if( progress < BLOCK_BUMP_FRAMES / 2 )
			{
				offset = (double)progress / (BLOCK_BUMP_FRAMES / 2);
			}
			else
			{
				offset = (double)(BLOCK_BUMP_FRAMES - progress) / (BLOCK_BUMP_FRAMES / 2);
			}
			setYOffset(offset / 2.0);
		}
	}
	else
	{
		setYOffset(0.0);
	}
}
