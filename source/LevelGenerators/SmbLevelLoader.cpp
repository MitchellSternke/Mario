#include <fstream>

#include <boost/algorithm/string.hpp>

#include "../Globals.hpp"
#include "SmbLevelLoader.hpp"

Level* SmbLevelLoader::generateLevel( int seed ) const
{
	Random random;
	random.seed(seed);

	// Load the data file
	std::string filename;
	filename += "smb/";
	filename += std::to_string((random.nextInt() % 8) + 1);
	filename += "-";
	filename += std::to_string((random.nextInt() % 4) + 1);
	filename += ".txt";
	std::ifstream file( filename );
	if( !file.good() )
	{
		return nullptr;
	}

	// Read all of the text to a string
	std::string line;
	getline( file, line );
	file.close();

	// Split the data by ';'
	std::vector<std::string> dataStrings;
	boost::split(dataStrings, line, boost::is_any_of(";"));

	// Split the first data string by ',' to discover tiles
	std::vector<std::string> levelData;
	boost::split(levelData, dataStrings[0], boost::is_any_of(","));

	const int levelWidth = levelData.size() / 15;
	const int levelHeight = 15;

	// Parse the tile data into the level
	Level* level = new Level(levelWidth, levelHeight);
	int index = 0;
	for( int y = levelHeight - 1; y >= 0; y-- )
	{
		for( int x = 0; x < levelWidth; x++ )
		{
			// Finally, split by '-'
			std::vector<std::string> tileData;
			boost::split(tileData, levelData[index++], boost::is_any_of("-"));

			// Determine tile information
			int type = std::atoi(tileData[0].c_str());
			int coins = 0;
			TileType tileType = TYPE_TILE_NULL;
			int width = 1;
			int height = 1;
			switch( type )
			{
			case 1:
				break;
			case 2:
			case 50:
				tileType = TYPE_GROUND;
				break;
			case 7:
			case 49:
				tileType = TYPE_BRICK;
				break;
			case 8:
				tileType = TYPE_QUESTION_BLOCK;
				coins = 1;
				break;
			case 16:
				tileType = TYPE_PIPE_UP;
				height = y + 1;
				width = 2;
				break;
			case 78:
				tileType = TYPE_BLOCK;
				break;
			case 103:
			case 104:
			case 105:
			case 129:
				if( y == 3 )
				{
					level->addSprite(x, y, TYPE_LEVEL_END);
				}
				break;
			case 116:
				level->addSprite(x, y, TYPE_COIN);
				break;
			default:
				//LOG << "Unrecognized type: " << levelData[index - 1] << std::endl;
				break;
			}

			// Add the tile to the level
			if( tileData.size() > 1 )
			{
				type = std::atoi(tileData[1].c_str());
			}
			if( tileType != TYPE_TILE_NULL )
			{
				int tile = level->addTile(x, y - height + 1, tileType, width, height);
				if( tileData.size() > 1 || coins > 0 )
				{
					// Get the entity for this position
					std::string sprite;
					switch( type )
					{
					case 2:
						sprite = "flower";
						break;
					case 4:
						sprite = "star";
						break;
					case 5:
						coins = 10;
						break;
					default:
						break;
					}

					if( sprite != "" )
					{
						level->setTileData( tile, "contents", sprite );
					}
					else if( coins != 0 )
					{
						level->setTileData( tile, "coins", coins );
					}
				}
			}
			else if( tileData.size() > 1 ) // Entities on an empty position?
			{
				SpriteType sprite = TYPE_SPRITE_NULL;
				switch( type )
				{
				case 6:
					sprite = TYPE_GOOMBA;
					break;
				case 7:
					sprite = TYPE_KOOPA;
					break;
				case 15:
					sprite = TYPE_HAMMER_BRO;
					break;
				case 22:
					sprite = TYPE_LAKITU;
					break;
				case 78:
					sprite = TYPE_PARATROOPA;
					break;
				default:
					break;
				}

				if( sprite != TYPE_SPRITE_NULL )
				{
					level->addSprite(x, y, sprite);
				}
			}
		}
	}

	return level;
}
