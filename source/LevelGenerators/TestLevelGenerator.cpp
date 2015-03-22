#include "TestLevelGenerator.hpp"

Level* TestLevelGenerator::generateLevel( int seed ) const
{
	Random random;
	random.seed(seed);

	Level* level = new Level( 256, 32 );
#if 0
	int start = 0;

	for( int x = 0; x < 240; x++ )
	{
		if( random.nextInt() % 3 == 0 )
		{
			int width = random.nextInt() % 4 + 3;
			int height = random.nextInt() % 5 + 2;
			level->addTile( x, 1, TYPE_LEDGE, width, height );
			x += width;
		}
		else if( random.nextInt() % 4 == 0 )
		{
			int width = random.nextInt() % 4 + 3;
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
					switch( random.nextInt() % 3 )
					{
					case 0:
						contents = "mushroom";
						break;
					case 1:
						contents = "flower";
						break;
					case 2:
						contents = "star";
						break;
					}
					level->setTileData(block, "contents", contents);
				}
				else
				{
					level->setTileData(block, "coins", coins );
				}
			}
			x += width;
		}
		else if( random.nextInt() % 7 == 0 )
		{
			for( int y = 1; y < 32; y++ )
			{
				level->addTile( x, y, TYPE_LADDER );
			}
		}
		else if( random.nextInt() % 10 == 0 )
		{
			int width = random.nextInt() % 4 + 4;
			level->addTile(x, 1, TYPE_BLOCK, 1, 4);
			level->addTile(x + 1, 1, TYPE_WATER, width - 2, 4);
			level->addTile(x + width - 1, 1, TYPE_BLOCK, 1, 4);
			x += width;
		}
		else if( random.nextInt() % 14 == 0 )
		{
			int pipe = level->addTile(x, 1, TYPE_PIPE_UP, 2, 4);
			level->setTileData( pipe, "warp_level_id", CURRENT_LEVEL );
			level->setTileData( pipe, "warp_x", x);
			level->setTileData( pipe, "warp_y", 8);
			pipe = level->addTile(x, 8, TYPE_PIPE_DOWN, 2, 24);
			level->setTileData( pipe, "warp_level_id", CURRENT_LEVEL);
			level->setTileData( pipe, "warp_x", x);
			level->setTileData( pipe, "warp_y", 1);
			x += 2;
		}
		else if( random.nextInt() % 7 == 0 )
		{
			int width = (random.nextInt() % 3 + 2) * 2;
			int pipe = level->addTile( x + 1, 1, TYPE_PIPE_LEFT, width / 2, 2);
			level->setTileData( pipe, "warp_level_id", CURRENT_LEVEL );
			level->setTileData( pipe, "warp_x", x + 1 + width / 2);
			level->setTileData( pipe, "warp_y", 1);
			pipe = level->addTile( x + 1 + width / 2, 1, TYPE_PIPE_RIGHT, width / 2, 2);
			level->setTileData(pipe, "warp_level_id", CURRENT_LEVEL);
			level->setTileData(pipe, "warp_x", x + 1);
			level->setTileData(pipe, "warp_y", 1);
			x += width + 2;
		}
		else if( random.nextInt() % 7 == 0 && x - start > 1 )
		{
			int width = random.nextInt() % 4 + 2;
			level->addTile( start, 0, TYPE_GROUND, x - start, 1);
			x += width;
			start = x;
		}
		else
		{
			int enemyCount = random.nextInt() % 5 + 1;
			for( int a = 0; a < enemyCount; a++ )
			{
				SpriteType enemy;
				switch( random.nextInt() % 3 )
				{
				case 0:
					enemy = TYPE_GOOMBA;
					break;
				case 1:
					enemy = TYPE_KOOPA;
					break;
				case 2:
					enemy = TYPE_SHELL;
					break;
				}
				level->addSprite( x + 2 * a, 1, enemy );
			}
			x += enemyCount * 2;
		}
	}

	if( 256 - start > 0 )
	{
		level->addTile( start, 0, TYPE_GROUND, 256 - start, 1 );
	}
#else
	level->addTile(0, 0, TYPE_GROUND, 256, 1);
#if 0
	int block = level->addTile(4, 4, TYPE_QUESTION_BLOCK);
	level->setTileData(block, "contents", "star");
	//level->addSprite(7, 1, TYPE_SHELL);

	//level->addTile(9, 1, TYPE_SLOPE_DOWN, 2, 1);

	//level->addTile( 12, 1, TYPE_WATER, 10, 10 );

	for( int x = 8; x < 12; x++ )
	{
		level->addTile(x, 4, TYPE_BRICK);
		level->addTile(x, 8, TYPE_BRICK);
	}
	level->addSprite( 11, 1, TYPE_HAMMER_BRO );

	level->addTile(12, 1, TYPE_PIPE_LEFT, 4, 2);
	level->addTile(16, 1, TYPE_PIPE_RIGHT, 4, 2);
	int sprite = level->addSprite(12.5, 1.5, TYPE_PLANT);
	level->setSpriteData(sprite, "orientation", "left");

	//for( int x = 0; x < 8; x += 2 )
	//{
		//level->addSprite(12 + x, 3, TYPE_PARATROOPA);
	//}

	level->addSprite( 20, 8, TYPE_LAKITU );

	int pipe = level->addTile(24, 1, TYPE_PIPE_UP, 2, 4);
	level->setTileData( pipe, "warp_level_id", CURRENT_LEVEL );
	level->setTileData( pipe, "warp_x", 26);
	level->setTileData( pipe, "warp_y", 1);
	level->addTile(26, 1, TYPE_PIPE_UP, 2, 3);
	level->addSprite( 26.5, 1.5, TYPE_PLANT );

	for( int y = 1; y < 32; y++ )
	{
		level->addTile( 30, y, TYPE_LADDER );
	}

	for( int x = 0; x < 10; x++ )
	{
		block = level->addTile( 33 + x, 4, TYPE_QUESTION_BLOCK);
		level->setTileData(block, "contents", "growing_ladder");
	}

	for( int x = 2; x < 9; x++ )
	{
		level->addTile( 33 + x, 10, TYPE_BRICK );
	}

	for( int x = 0; x < 10; x += 2 )
	{
		level->addTile( 40 + x, 1, TYPE_BLASTER, 1, (x / 2) % 2 + 1);
	}
	level->addSprite( 2, 5, TYPE_LEVEL_END );
#elif 0
	int qb = level->addTile(3, 4, TYPE_QUESTION_BLOCK);
	level->setTileData(qb, "contents", "growing_ladder");
	level->addTile(3, 20, TYPE_BRICK);
	for( int x = 0; x < 8; x++ )
	{
		level->addTile(4 + x, 16, TYPE_BRICK);
		level->addSprite(4 + x, 17, TYPE_COIN);
	}

	level->addTile( 12, 1, TYPE_BLOCK, 1, 11 );
	level->addTile( 13, 1, TYPE_WATER, 4, 11 );
	level->addTile( 17, 4, TYPE_BLOCK, 1, 8);
	level->addTile( 17, 1, TYPE_WATER, 5, 3);
	level->addTile( 22, 1, TYPE_BLOCK, 1, 3 );

	int pipe = level->addTile( 23, 1, TYPE_PIPE_UP, 2, 2 );
	level->setTileData(pipe, "warp_level_id", CURRENT_LEVEL);
	level->setTileData(pipe, "warp_x", 18);
	level->setTileData(pipe, "warp_y", 10);
	level->addTile(18, 10, TYPE_PIPE_RIGHT, 3, 2);

	level->addTile( 28, 1, TYPE_PIPE_LEFT, 2, 2);
	level->addTile( 30, 1, TYPE_BLOCK, 2, 2 );
	level->addTile( 30, 3, TYPE_PIPE_UP, 2, 2 );
	int plant = level->addSprite(28.5,1.5, TYPE_PLANT );
	level->setSpriteData(plant, "orientation", "left");
	level->addSprite( 30.5, 2.5, TYPE_PLANT );

	for( int x = 0; x < 10; x += 2 )
	{
		level->addTile(32 + x, 1, TYPE_PIPE_UP, 2, 4);
		level->addSprite(32.5 + x, 2.5, TYPE_PLANT);
	}

	qb = level->addTile( 42, 4, TYPE_QUESTION_BLOCK );
	level->setTileData(qb, "contents", "mushroom");
	qb = level->addTile( 44, 4, TYPE_QUESTION_BLOCK );
	level->setTileData(qb, "contents", "flower");

	for( int x = 0; x < 3; x++ )
	{
		level->addSprite(54 + x * 2, 1, TYPE_BEETLE);
	}

	level->addTile(62, 1, TYPE_BLASTER);
	level->addTile(63, 1, TYPE_BLASTER, 1, 2);
	level->addTile(65, 1, TYPE_BLASTER, 1, 2);
	level->addTile(69, 1, TYPE_BLASTER);
	level->addTile(69, 2, TYPE_BLASTER, 1, 2);

	for( int x = 0; x < 3; x++ )
	{
		level->addSprite(76 + x * 2, 1, TYPE_PARATROOPA);
	}

	for( int x = 0; x < 10; x++ )
	{
		level->addTile(86 + x, 4, TYPE_BRICK);
		level->addTile(86 + x, 8, TYPE_BRICK);
	}

	level->addSprite(88, 1, TYPE_HAMMER_BRO);
	level->addSprite(92, 5, TYPE_HAMMER_BRO);

	qb = level->addTile(99, 4, TYPE_QUESTION_BLOCK);
	level->setTileData(qb, "contents", "leaf");
	qb = level->addTile(100, 4, TYPE_QUESTION_BLOCK);
	level->setTileData(qb, "contents", "leaf");

	level->addSprite(120, 11, TYPE_LAKITU);

	for( int x = 0; x < 25; x++ )
	{
		level->addSprite(140 + x, x + 1, TYPE_COIN);
	}

	for(int x = 0; x < 10; x++ )
	{
		level->addTile(165 + x, 22, TYPE_BRICK);
	}

	level->addSprite( 175, 23, TYPE_LEVEL_END );
#else
	int block = level->addTile( 5, 5, TYPE_QUESTION_BLOCK );
	level->setTileData(block, "contents", "mushroom_1up" );
#endif
#endif

	return level;
}
