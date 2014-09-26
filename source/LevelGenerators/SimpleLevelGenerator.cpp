#include "SimpleLevelGenerator.hpp"

static const int LEVEL_WIDTH = 256;
static const int LEVEL_HEIGHT = 24;

Level* SimpleLevelGenerator::generateLevel( int seed ) const
{
	// Setup
	Random random;
	random.seed( seed );
	Level* level = new Level( LEVEL_WIDTH, LEVEL_HEIGHT );

#if 0
	// Generate ground
	int groundWidth = random.nextInt() % 5 + 5;
	int groundRemaining = groundWidth;
	for( int x = 0; x < LEVEL_WIDTH - 16; x++ )
	{
		// Done creating the ground? Make a gap.
		if( groundRemaining == 0 || x == LEVEL_WIDTH - 17 )
		{
			level->addTile(x - groundWidth, 0, TYPE_GROUND, groundWidth, 1);

			if( x == LEVEL_WIDTH - 17 )
			{
				break;
			}

			int gapWidth = random.nextInt() % 3 + 3;

			// Generate coins over a gap
			if( random.nextInt() % 3 == 0 )
			{
				int coinWidth = random.nextInt() % (gapWidth + 1);
				for( int a = 0; a < coinWidth; a++ )
				{
					level->addSprite(x + a, 4, TYPE_COIN);
				}
			}

			x += gapWidth;

			groundWidth = random.nextInt() % 5 + 5;
			groundRemaining = groundWidth;
		}
		else
		{
			// Add pipes on the edge of ground ledges
			if( (groundRemaining == groundWidth || groundRemaining == 2) && random.nextInt() % 3 == 0 )
			{
				level->addTile(x, 1, TYPE_PIPE_UP, 2, 3);
				x++;
				groundRemaining--;
			}
			// Generate enemies
			else if( random.nextInt() % 3 == 0 )
			{
				level->addSprite(x, 1, TYPE_GOOMBA);
			}
			// Generate blocks
			else if( random.nextInt() % 10 == 0 )
			{
				int width = random.nextInt() % groundRemaining + 1;
				for( int a = 0; a < width; a++ )
				{
					TileType blockType;
					if( random.nextInt() % 3 )
					{
						blockType = TYPE_BRICK;
					}
					else
					{
						blockType = TYPE_QUESTION_BLOCK;
					}
					int block = level->addTile(x + a, 4, blockType );
					int coins = (random.nextInt() % 2) * 10;
					if( coins == 0 && blockType == TYPE_QUESTION_BLOCK )
					{
						std::string contents;
						switch( random.nextInt() % 2 )
						{
						case 0:
							contents = "mushroom";
							break;
						case 1:
							contents = "flower";
							break;
						}
						level->setTileData(block, "contents", contents);
					}
					else
					{
						level->setTileData(block, "coins", coins );
					}
				}
				x += width - 1;
				groundRemaining -= (width - 1);
			}

			groundRemaining--;
		}
	}

	// Generate the end of the level
	level->addTile(LEVEL_WIDTH - 14, 0, TYPE_GROUND, 14, 1);
	level->addSprite(LEVEL_WIDTH - 2, 5, TYPE_LEVEL_END);
#else
	int height = 0;
	int l = 0;
	bool blocksPlaced = false;
	bool coinsPlaced = false;
	bool sceneryOn = false;
	for( int x = 0; x < LEVEL_WIDTH; x++ )
	{
		// Build platforms
		if( l <= 0 )
		{
			int newH = height;
			while( newH > height + 4 || newH == height )
			{
				newH = random.nextInt() % 24 - 8;
				if( newH < 0 )
				{
					newH = 0;
				}
			}
			height = newH;
			if( newH == 0 )
			{
				l = random.nextInt() % 5 + 2;
			}
			else
			{
				l = random.nextInt() % 3 + 4;
			}

			if( l + x > LEVEL_WIDTH )
			{
				l = LEVEL_WIDTH - x;
			}

			blocksPlaced = false;
			coinsPlaced = false;

			if( height > 0 )
			{
				if( random.nextInt() % 5 > 0 )
				{
					level->addTile(x, 0, TYPE_LEDGE, l, height);
					sceneryOn = true;
				}
				else
				{
					for( int a = 0; a < l; a++ )
					{
						for( int b = 0; b < height; b++ )
						{
							if( a == 0 || a == l - 1 )
							{
								int coins = 1;
								if( b == 1 )
								{
									coins = 10;
								}

								int tile = level->addTile(x + a, b, TYPE_BRICK);
								level->setTileData(tile, "coins", coins);
							}
							else if( (b % 2 == 0 && a != l - 2) || (b % 2 == 1 && a != 1) || b == 0 )
							{
								level->addTile(x + a, b, TYPE_BRICK);
							}
						}
					}
					sceneryOn = false;
				}
			}
		}
		l--;

		// Spawn enemies
		if( random.nextInt() % 5 == 0 && height != 0 )
		{
			SpriteType enemy;
			switch( random.nextInt() % 2 )
			{
				case 0:
					enemy = TYPE_GOOMBA;
					break;

				case 1:
					enemy = TYPE_KOOPA;
					break;
			}
			level->addSprite(x, height, enemy);
		}

		// Create blocks
		if( !blocksPlaced && height != 0 && l > 0 && random.nextInt() % 7 == 0 )
		{
			int blocks = random.nextInt() % 3 + 4;
			if( blocks >= l )
			{
				blocks = l - 1;
			}
			int blockHeight = height + random.nextInt() % 2 + 3;
			for( int b = 0; b < blocks; b++ )
			{
				TileType blockType;
				if( random.nextInt() % 3 )
				{
					blockType = TYPE_BRICK;
				}
				else
				{
					blockType = TYPE_QUESTION_BLOCK;
				}
				int block = level->addTile(x + b, blockHeight, blockType);
				int coins = (random.nextInt() % 2) * 10;
				if( coins == 0 && blockType == TYPE_QUESTION_BLOCK )
				{
					std::string contents;
					switch( random.nextInt() % 3 )
					{
					case 0:
						contents = "mushroom";
						break;
					case 1:
						contents = "flower";
						break;
					case 2:
						contents = "leaf";
						break;
					}
					level->setTileData(block, "contents", contents);
				}
				else
				{
					level->setTileData(block, "coins", coins );
				}
			}
			blocksPlaced = true;
		}

		// Create coins
		if( !coinsPlaced && random.nextInt() % 3 == 0 )
		{
			int coins = random.nextInt() % 6 + 2;
			if( coins >= l )
			{
				coins = l - 1;
			}

			int coinY = height + random.nextInt() % 2 + 1;
			int coinHeight = 1;
			if( coinY < height + 2 )
			{
				coinHeight = random.nextInt() % 2 + 1;
			}

			if( height == 0 )
			{
				coinY = random.nextInt() % 3 + 4;
				coinHeight = random.nextInt() % 2 + 1;
			}

			for( int coinX = x; coinX < x + coins; coinX++ )
			{
				for( int y = 0; y < coinHeight; y++ )
				{
					level->addSprite(coinX, coinY + y, TYPE_COIN);
				}
			}

			coinsPlaced = true;
		}

		// Create scenery
		if( sceneryOn && height != 0 && random.nextInt() % 5 == 0 )
		{
			//level->addTile(x, height, new Scenery("bush"));
		}
	}

	// Add the end of the level
	level->addSprite(LEVEL_WIDTH - 2, height + 3, TYPE_LEVEL_END);
#endif

	return level;
}
