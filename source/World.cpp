#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>

#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "Background.hpp"
#include "Entity.hpp"
#include "Episode.hpp"
#include "Globals.hpp"
#include "Level.hpp"
#include "LevelGenerator.hpp"
#include "Math.hpp"
#include "Music.hpp"
#include "Particle.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "Rendering.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "Texture.hpp"
#include "Tile.hpp"
#include "World.hpp"

static const double BOUNDARY_SIZE = 10.0; /**< The boundary size surrounding the world that kills sprites when they go out of bounds. */

/**
 * Check if two boxes intersect.
 *
 * @param p1 the lower left point of box1.
 * @param box1 bounding box 1.
 * @param p2 the lower left point of box2.
 * @param box2 bounding box 2.
 * @todo this needs epsilon support. Maybe?
 */
static bool intersects( const Vector2<double>& p1,
	const Vector2<double>& box1,
	const Vector2<double>& p2,
	const Vector2<double>& box2 )
{
	Vector2<double> max1 = p1 + Vector2<double>(box1.x, box1.y);
	Vector2<double> max2 = p2 + Vector2<double>(box2.x, box2.y);
	if( max1.x <= p2.x || p1.x >= max2.x ||
		max1.y <= p2.y || p1.y >= max2.y )
	{
		return false;
	}

	return true;
}

World::Cell::Cell() :
	tile(nullptr),
	spawn(nullptr),
	underwater(false)
{
}

World::Cell::~Cell()
{
	delete spawn;
}

World::World() :
	background(nullptr),
	delta(GAME_DELTA),
	frameNumber(0),
	player(nullptr)
{
	random = new Random;
	random->seedTime();
}

World::~World()
{
	unloadLevel();

	delete random;
}

void World::addSprite(Sprite* sprite)
{
	sprite->world = this;
	sprite->firstFrame = frameNumber;
	sprites.push_back(sprite);
	insertSprite(sprite);
}

void World::addSprite(double x, double y, Sprite* sprite)
{
	sprite->position.x = x;
	sprite->position.y = y;
	addSprite(sprite);
}

void World::destroyDeadTiles()
{
	for( auto t : deadTiles )
	{
		delete t;
	}
	deadTiles.clear();
}

void World::destroySprite(Sprite* sprite)
{
	removeSprite(sprite);
	delete sprite;
}

void World::destroyTile( Tile* tile )
{
	eraseTile(tile);
	deadTiles.insert(tile);
}

bool World::doSpriteCollisionXAxisTest(Sprite* sprite, const Vector2<double>& oldPosition, bool tileCollisionsEnabled, bool spriteCollisionsEnabled)
{
	Vector2<double>& velocity = sprite->velocity;
	Vector2<double>& position = sprite->position;
	Vector2<double>& size = sprite->size;
	bool stop = false;

	// Lists of tiles and sprites that the sprite collides with
	std::set<Tile*> collisionTiles;
	std::set<Sprite*> collisionSprites;

	// Do regular axis checking
	for( double y = std::floor(oldPosition.y); y < oldPosition.y + size.y - std::fabs(oldPosition.y) * DOUBLE_EPSILON; ++y )
	{
		for( double x = std::floor(position.x); x < position.x + size.x - std::fabs(position.x) * DOUBLE_EPSILON; ++x )
		{
			Cell* cell = getCell(x, y);
			if( cell == nullptr )
			{
				continue;
			}

			if( tileCollisionsEnabled )
			{
				// Check collisions with tiles
				if( cell->tile != nullptr )
				{
					Tile* tile = cell->tile;
					if( velocity.x > 0 &&
						position.x + size.x > tile->x - std::fabs(tile->x) * DOUBLE_EPSILON &&
						oldPosition.x + size.x <= tile->x + EPSILON * EPSILON )
					{
						if( getCellEdgeState(x, y, EDGE_LEFT) == true )
						{
							position.x = tile->x - size.x;
							stop = true;
						}
						collisionTiles.insert(tile);
					}
					else if( velocity.x < 0 &&
						position.x - std::fabs(position.x) * DOUBLE_EPSILON < tile->x + tile->width &&
						oldPosition.x + EPSILON * EPSILON >= tile->x + tile->width )
					{
						if( getCellEdgeState(x, y, EDGE_RIGHT) == true )
						{
							position.x = tile->x + tile->width;
							stop = true;
						}
						collisionTiles.insert(tile);
					}
				}
			}

			// Check collisions with sprites
			if( spriteCollisionsEnabled )
			{
				for( auto sprite2 : cell->sprites )
				{
					if( sprite2 == sprite )
					{
						continue;
					}

					if( intersects(Vector2<double>(position.x, oldPosition.y), size, sprite2->position, sprite2->size) )
					{
						if( velocity.x > 0 )
						{
							collisionSprites.insert(sprite2);
						}
						else if( velocity.x < 0 )
						{
							collisionSprites.insert(sprite2);
						}
					}
				}
			}
		}
	}

	double centerX = position.x + size.x / 2.0;
	double oldCenterX = oldPosition.x + size.x / 2.0;
	bool onSlope = false;
	double newY = -1.0;
	double oldVelocity = velocity.x;
	if( tileCollisionsEnabled )
	{
		// Check for slopes (ugly, please comment more)
		if( /*velocity.y < 0*/ true ) ///@todo is this needed? not having it fixes issues when jumping up through slopes when we don't check y velocity.
		{
			for( double y = std::floor(oldPosition.y - 1); y < oldPosition.y + size.y; ++y )
			{
				Cell* cell = getCell(std::floor(centerX), y);
				if( cell != nullptr && cell->tile != nullptr &&
					(cell->tile->collisionBehavior == TILE_SLOPE_DOWN || cell->tile->collisionBehavior == TILE_SLOPE_UP) )
				{
					Tile* tile = cell->tile;
					double height, oldHeight;
					if( tile->collisionBehavior == TILE_SLOPE_DOWN )
					{
						height = tile->y + tile->height - (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
						oldHeight = tile->y + tile->height - (double)tile->height * (oldCenterX - (double)tile->x) / (double)tile->width;
					}
					else if( tile->collisionBehavior == TILE_SLOPE_UP )
					{
						height = tile->y + (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
						oldHeight = tile->y + (double)tile->height * (oldCenterX - (double)tile->x) / (double)tile->width;
					}

					if( position.y < height && oldPosition.y >= oldHeight )
					{
						newY = height;
						sprite->slope = tile;
						onSlope = true;
					}
				}
				///@todo determine if the code should really be in the middle of this loop. It was moved out when the slope flying bug was fixed.
	/*
				else if( sprite->slope != nullptr && !onSlope )
				{
					// We left a slope...
					Tile* tile = sprite->slope;
					double height;
					if( tile->collisionBehavior == TILE_SLOPE_DOWN )
					{
						height = tile->y + tile->height - (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
					}
					else if( tile->collisionBehavior == TILE_SLOPE_UP )
					{
						height = tile->y + (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
					}

					if( height < tile->y )
					{
						height = tile->y;
					}
					else if( height > tile->y + tile->height )
					{
						height = tile->y + tile->height;
					}

					newY = height;
				}
	*/
			}
		}

		// Check if the sprite just left a slope tile
		if( sprite->slope != nullptr && !onSlope )
		{
			// We left a slope...
			Tile* tile = sprite->slope;
			double height = 0.0;
			if( tile->collisionBehavior == TILE_SLOPE_DOWN )
			{
				height = tile->y + tile->height - (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
			}
			else if( tile->collisionBehavior == TILE_SLOPE_UP )
			{
				height = tile->y + (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
			}

			if( height < tile->y )
			{
				height = tile->y;
			}
			else if( height > tile->y + tile->height )
			{
				height = tile->y + tile->height;
			}

			newY = height;

			sprite->exitedSlopeThisFrame = true;
		}
		else
		{
			sprite->exitedSlopeThisFrame = false;
		}

		// Handle any collisions that occurred
		for( auto t : collisionTiles )
		{
			if( oldVelocity > 0 )
			{
				handleCollision(sprite, t, EDGE_RIGHT);
			}
			else if( oldVelocity < 0 )
			{
				handleCollision(sprite, t, EDGE_LEFT);
			}
		}
	}

	if( spriteCollisionsEnabled )
	{
		for( auto s : collisionSprites )
		{
			if( oldVelocity > 0 )
			{
				handleCollision(sprite, s, EDGE_RIGHT);
			}
			else if( oldVelocity < 0 )
			{
				handleCollision(sprite, s, EDGE_LEFT);
			}
		}
	}

	if( !onSlope )
	{
		sprite->slope = nullptr;
	}
	if( newY != -1.0 )
	{
		position.y = newY;
	}

	if( oldVelocity == velocity.x )
	{
		return stop;
	}
	else
	{
		return false;
	}
}

bool World::doSpriteCollisionYAxisTest(Sprite* sprite, const Vector2<double>& oldPosition, bool tileCollisionsEnabled, bool spriteCollisionsEnabled)
{
	Vector2<double>& velocity = sprite->velocity;
	Vector2<double>& position = sprite->position;
	Vector2<double>& size = sprite->size;
	bool stop = false;

	double newY = -1.0; // Delay the change of the y coordinate to allow priority for slopes
	bool slope = false; // Whether a slope collision occurred

	// Check for slopes (ugly, please comment more)
	double centerX = position.x + size.x / 2.0;
	double oldCenterX = oldPosition.x + size.x / 2.0;
	if( tileCollisionsEnabled )
	{
		if( velocity.y < 0 )
		{
			for( double y = std::floor(position.y - 1); y < position.y + size.y; ++y )
			{
				Cell* cell = getCell(std::floor(centerX), y);
				if( cell != nullptr && cell->tile != nullptr &&
					(cell->tile->collisionBehavior == TILE_SLOPE_DOWN || cell->tile->collisionBehavior == TILE_SLOPE_UP) )
				{
					Tile* tile = cell->tile;
					double height, oldHeight;
					if( tile->collisionBehavior == TILE_SLOPE_DOWN )
					{
						height = tile->y + tile->height - (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
						oldHeight = tile->y + tile->height - (double)tile->height * (oldCenterX - (double)tile->x) / (double)tile->width;
					}
					else if( tile->collisionBehavior == TILE_SLOPE_UP )
					{
						height = tile->y + (double)tile->height * (centerX - (double)tile->x) / (double)tile->width;
						oldHeight = tile->y + (double)tile->height * (oldCenterX - (double)tile->x) / (double)tile->width;
					}

					// Hack - this works fine in most cases. Try changing epsilon if needed
					//static const double EPSILON = 0.2; // now in World.hpp
					/// @todo this was originially height - EPSILON. Determine if oldHeight is appropriate
					if( std::fabs(position.y - height) < EPSILON && oldPosition.y >= oldHeight - EPSILON )
					{
						newY = height;
						sprite->slope = tile;
						slope = true;
						stop = true;
					}
				}
			}
		}
	}

	// Lists of tiles and sprites that the sprite collides with
	std::set<Tile*> collisionTiles;
	std::set<Sprite*> collisionSprites;

	// Do regular axis checking
	for( double x = std::floor(position.x); x < position.x + size.x - std::fabs(position.x) * DOUBLE_EPSILON; ++x )
	{
		for( double y = std::floor(position.y); y < position.y + size.y - std::fabs(position.y) * DOUBLE_EPSILON; ++y )
		{
			Cell* cell = getCell(x, y);
			if( cell == nullptr )
			{
				continue;
			}

			if( tileCollisionsEnabled )
			{
				// Check collisions with tiles
				if( cell->tile != nullptr )
				{
					Tile* tile = cell->tile;
					if( velocity.y > 0 &&
						position.y + size.y > tile->y - std::fabs(tile->y) * DOUBLE_EPSILON &&
						oldPosition.y + size.y <= tile->y + std::fabs(tile->y) * DOUBLE_EPSILON )
					{
						if( getCellEdgeState(x, y, EDGE_BOTTOM) == true )
						{
							//position.y = tile->y - size.y;
							newY = tile->y - size.y;
							stop = true;
						}
						collisionTiles.insert(tile);
					}
					else if( velocity.y < 0 &&
						position.y - std::fabs(position.y) * DOUBLE_EPSILON < tile->y + tile->height &&
						oldPosition.y + std::fabs(oldPosition.y) * DOUBLE_EPSILON >= tile->y + tile->height )
					{
						if( getCellEdgeState(x, y, EDGE_TOP) == true )
						{
							// Override - this allows "sinking" into slopes from adjacent tiles
							if( !slope )
							{
								newY = tile->y + tile->height;
								stop = true;
							}
						}
						collisionTiles.insert(tile);
					}
				}
			}

			if( spriteCollisionsEnabled )
			{
				// Check collisions with sprites
				for( auto sprite2 : cell->sprites )
				{
					if( sprite2 == sprite )
					{
						continue;
					}

					if( intersects(position, size, sprite2->position, sprite2->size) )
					{
						if( velocity.y > 0 )
						{
							collisionSprites.insert(sprite2);
						}
						else if( velocity.y < 0 )
						{
							collisionSprites.insert(sprite2);
						}
					}
				}
			}
		}
	}

	// Update the flying flag
	bool oldFlyingState = sprite->flying;
	if( (stop && sprite->velocity.y <= 0.0) || sprite->exitedSlopeThisFrame )
	{
		sprite->flying = false;
		if( oldFlyingState )
		{
			sprite->onGround();
		}
	}
	else
	{
		sprite->flying = true;
	}

	// Handle any collisions that occurred
	double oldVelocity = velocity.y;
	if( tileCollisionsEnabled )
	{
		for( auto t : collisionTiles )
		{
			if( oldVelocity > 0 )
			{
				handleCollision(sprite, t, EDGE_TOP);
			}
			else if( oldVelocity < 0 )
			{
				handleCollision(sprite, t, EDGE_BOTTOM);
			}
		}
	}
	if( spriteCollisionsEnabled )
	{
		for( auto s : collisionSprites )
		{
			if( oldVelocity > 0 )
			{
				handleCollision(sprite, s, EDGE_TOP);
			}
			else if( oldVelocity < 0 )
			{
				handleCollision(sprite, s, EDGE_BOTTOM);
			}
		}
	}

	// Support for slope sinking
	if( newY != -1.0 && !slope )
	{
		position.y = newY;
	}

	if( oldVelocity == velocity.y )
	{
		return stop;
	}
	else
	{
		return false;
	}
}

void World::endLevel( int exit )
{
	status.statusType = WORLD_LEVEL_ENDED;
	status.exit = exit;
}

void World::eraseSprite(Sprite* sprite)
{
	// Remove the sprite from all cells it is (was) on
	for( auto v : sprite->occupiedCells )
	{
		Cell* cell = getCell(v.x, v.y);
		if( cell != nullptr )
		{
			cell->sprites.erase(sprite);
		}
	}
	sprite->occupiedCells.clear();
}

void World::eraseTile(Tile* tile)
{
	// Remove the tile from all cells it is on
	for( int x = tile->x; x < tile->x + tile->width; ++x )
	{
		for( int y = tile->y; y < tile->y + tile->height; ++y )
		{
			Cell* cell = getCell(x, y);
			if( cell != nullptr )
			{
				cell->tile = 0;
			}
		}
	}
}

void World::freezeTime()
{
	timeFrozen = true;
}

World::Cell* World::getCell(int x, int y)
{
	if( x < 0 || x >= width || y < 0 || y >= height )
	{
		return nullptr;
	}

	return &cells[y * width + x];
}

const World::Cell* World::getCell( int x, int y ) const
{
	if( x < 0 || x >= width || y < 0 || y >= height )
	{
		return nullptr;
	}

	return &cells[y * width + x];
}

bool World::getCellEdgeState(int x, int y, Edge edge)
{
	int dx = 0;
	int dy = 0;
	Edge otherEdge = getOppositeEdge(edge);
	switch( edge )
	{
		case EDGE_BOTTOM:
			dy--;
			break;

		case EDGE_LEFT:
			dx--;
			break;

		case EDGE_RIGHT:
			dx++;
			break;

		case EDGE_TOP:
			dy++;
			break;
	}

	bool edge1 = false;
	bool edge2 = false;

	Cell* cell = getCell(x, y);
	Cell* cell2 = getCell(x + dx, y + dy);
	Tile* tile = nullptr;
	Tile* tile2 = nullptr;
	if( cell != nullptr )
	{
		tile = cell->tile;
	}
	if( cell2 != nullptr )
	{
		tile2 = cell2->tile;
	}

	bool slope = false; // Slopes are a special case
	bool tileIsSlope = false;
	if( tile != tile2 ) // Edges within the same tile are never solid
	{
		if( tile != nullptr )
		{
			edge1 = tile->getEdgeState(edge);
			if( tile->collisionBehavior == TILE_SLOPE_DOWN || tile->collisionBehavior == TILE_SLOPE_UP )
			{
				if( edge1 )
				{
					slope = true;
				}
				tileIsSlope = true;
			}
		}
		if( tile2 != nullptr )
		{
			edge2 = tile2->getEdgeState(otherEdge);
			if( tile2->collisionBehavior == TILE_SLOPE_DOWN || tile2->collisionBehavior == TILE_SLOPE_UP )
			{
				if( edge2 )
				{
					slope = true;
				}
			}
		}
	}

	return (edge1 != edge2 &&
		!slope &&
		!(tileIsSlope && edge == EDGE_BOTTOM) ); // Exemption for top edge below slopes
}

double World::getDelta() const
{
	return delta;
}

int World::getFrameNumber() const
{
	return frameNumber;
}

int World::getHeight() const
{
	return height;
}

Player* World::getPlayer()
{
	return player;
}

Random& World::getRandom()
{
	return *random;
}

const std::set<Sprite*>* World::getSprites( int x, int y ) const
{
	const Cell* cell = getCell(x, y);
	if( cell != nullptr )
	{
		return &cell->sprites;
	}
	else
	{
		return nullptr;
	}
}

std::set<Sprite*> World::getSpritesInBox( double left, double bottom, double width, double height )
{
	std::set<Sprite*> spriteSet;
	Vector2<double> position(left, bottom);
	Vector2<double> box(width, height);
	for( int x = static_cast<int>(std::floor(left)); x <= static_cast<int>(std::floor(left + width)); x++ )
	{
		for( int y = static_cast<int>(std::floor(bottom)); y <= static_cast<int>(std::floor(bottom + height)); y++ )
		{
			auto s = getSprites(x, y);
			if( s != nullptr )
			{
				for( auto t : *s )
				{
					if( intersects(position, box, t->position, t->size) )
					{
						spriteSet.insert(t);
					}
				}
			}
		}
	}
	return spriteSet;
}

WorldStatus World::getStatus() const
{
	return status;
}

const Tile* World::getTile( int x, int y ) const
{
	const Tile* tile = nullptr;
	const Cell* cell = getCell(x, y);
	if( cell != nullptr )
	{
		tile = cell->tile;
	}

	return tile;
}

std::set<Tile*> World::getTilesInBox( double left, double bottom, double width, double height )
{
	std::set<Tile*> tileSet;
	for( int x = static_cast<int>(std::floor(left)); x <= static_cast<int>(std::floor(left + width)); x++ )
	{
		for( int y = static_cast<int>(std::floor(bottom)); y <= static_cast<int>(std::floor(bottom + height)); y++ )
		{
			auto t = const_cast<Tile*>(getTile(x, y));
			if( t != nullptr )
			{
				tileSet.insert(t);
			}
		}
	}
	return tileSet;
}

int World::getTime() const
{
	int t = time / GAME_FPS;
	if( time % (int)GAME_FPS != 0 )
	{
		t++;
	}
	return t;
}

int World::getWidth() const
{
	return width;
}

void World::handleCollision(Sprite* sprite1, Sprite* sprite2, Edge edge)
{
	/// @todo move the dead check to the calling function for optimization
	if( !sprite1->dead && sprite2->spriteCollisionsEnabled && !sprite2->dead )
	{
		// Activate event handlers
		sprite1->onCollision(*sprite2, edge);
		sprite2->onCollision(*sprite1, getOppositeEdge(edge));
	}
}

void World::handleCollision(Sprite* sprite, Tile* tile, Edge edge)
{
	// Activate event handlers
	sprite->onCollision(*tile, edge);
	tile->onCollision(*sprite, getOppositeEdge(edge));
}

void World::insertSprite(Sprite* sprite)
{
	// Add the sprite to all cells it is on
	sprite->occupiedCells.clear();
	for( double x = std::floor(sprite->position.x); x < sprite->position.x + sprite->size.x; x++ )
	{
		for( double y = std::floor(sprite->position.y); y < sprite->position.y + sprite->size.y; y++ )
		{
			int a = std::floor(x);
			int b = std::floor(y);
			Cell* cell = getCell(a, b);
			if( cell != nullptr )
			{
				cell->sprites.insert(sprite);
				sprite->occupiedCells.push_back(Vector2<int>(a, b));
			}
		}
	}
}

bool World::isUnderwater( double x, double y ) const
{
	const Cell* cell = getCell( static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)) );
	if( cell == nullptr )
	{
		return false;
	}

	return cell->underwater;
}

void World::loadLevel( const Level* level )
{
	// Set the background
	background = level->background;

	// Set the music
	backgroundMusic = level->music;

	// Allocate cells
	width = level->width;
	height = level->height;
	cells.clear();
	cells.resize(level->width * level->height);

	// Load the entities in the Level
	int id = 0;
	for( auto tile : level->tiles )
	{
		setTile(tile.x, tile.y, level->createTile(id++) );
	}
	id = 0;
	for( auto sprite : level->sprites )
	{
		Cell* cell = getCell(std::floor(sprite.x), std::floor(sprite.y));
		if( cell != nullptr )
		{
			cell->spawn = level->createSprite(id++);
			cell->spawn->position.x = sprite.x;
			cell->spawn->position.y = sprite.y;
		}
	}

	// Copy the water array
	for( int x = 0; x < width; x++ )
	{
		for( int y = 0; y < height; y++ )
		{
			getCell(x, y)->underwater = level->water[y * width + x];
		}
	}

	status.statusType = WORLD_RUNNING;
}

void World::playBackgroundMusic() const
{
	if( backgroundMusic != nullptr )
	{
		backgroundMusic->play(true);
	}
}

void World::removeSprite(Sprite* sprite)
{
	eraseSprite(sprite);
	sprites.remove(sprite);
	sprite->world = nullptr;
	sprite->slope = nullptr;
}

void World::render(double viewX, double viewY, double viewWidth, double viewHeight)
{
	// Clamp view coordinates
	renderClampView(viewX, viewY, viewWidth, viewHeight, width, height);

	// Render the background
	if( background != nullptr )
	{
		background->getTexture().bind();
		const Animation::Frame& frame = background->getAnimation().getFrame(frameNumber);

		int frameWidth = (int)(frame.width * UNIT_SIZE);
		int frameHeight = (int)(frame.height * UNIT_SIZE);

		// Determine the background tilings that the view encompasses
		int xMin = std::floor((viewX - viewWidth / 2.0) * UNIT_SIZE);
		int xMax = std::ceil((viewX + viewWidth / 2.0) * UNIT_SIZE) + frameWidth;
		int yMin = std::floor((viewY - viewHeight / 2.0) * UNIT_SIZE);
		int yMax = std::ceil((viewY + viewHeight / 2.0) * UNIT_SIZE) + frameHeight;

		for( int x = xMin; x <= xMax; x += frameWidth )
		{
			for( int y = yMin; y <= yMax; y += frameHeight )
			{
				glPushMatrix();

				double translationX = x / (double)UNIT_SIZE - xMin / (double)UNIT_SIZE - (xMin / UNIT_SIZE) % frameWidth / (double)UNIT_SIZE;
				double translationY = y / (double)UNIT_SIZE - yMin / (double)UNIT_SIZE - (yMin / UNIT_SIZE) % frameHeight / (double)UNIT_SIZE;
				glTranslatef( translationX, translationY, -10.0 );

				if( SETTINGS.debugMode )
				{
					glEnable(GL_TEXTURE_2D);
				}

				glColor4ub(0xff, 0xff, 0xff, 0xff);
				glBegin(GL_QUADS);
					glTexCoord2d(frame.left, frame.bottom);
					glVertex2d(0.0, 0.0);
					glTexCoord2d(frame.right, frame.bottom);
					glVertex2d(frame.width, 0.0);
					glTexCoord2d(frame.right, frame.top);
					glVertex2d(frame.width, frame.height);
					glTexCoord2d(frame.left, frame.top);
					glVertex2d(0.0, frame.height);
				glEnd();

				if( SETTINGS.debugMode )
				{
					glColor4ub(0, 0, 0, 0x7f);
					glDisable(GL_TEXTURE_2D);
					glBegin(GL_LINE_LOOP);
						glVertex2d(0.0, 0.0);
						glVertex2d(frame.width, 0.0);
						glVertex2d(frame.width, frame.height);
						glVertex2d(0.0, frame.height);
					glEnd();
				}

				glPopMatrix();
			}
		}
	}

	// Bind the texture atlas
	RESOURCE_MANAGER.bindTextureAtlas();

	// Build a set of all entities to render
	std::set<Entity*> entitiesToRender;
	for( int x = std::floor(viewX - viewWidth / 2.0); x <= std::ceil(viewX + viewWidth / 2.0); ++x )
	{
		for( int y = std::floor(viewY - viewHeight / 2.0); y <= std::ceil(viewY + viewHeight / 2.0); ++y )
		{
			Cell* cell = getCell(x, y);
			if( cell == nullptr )
			{
				continue;
			}

			// If the cell has a sprite to spawn on it, add it to the world
			if( cell->spawn != nullptr )
			{
				addSprite(cell->spawn);
				cell->spawn = nullptr;
			}

			if( cell->tile != nullptr )
			{
				entitiesToRender.insert(cell->tile);
			}

			// Add all sprites to be rendered
			entitiesToRender.insert( cell->sprites.begin(), cell->sprites.end() );

			if( SETTINGS.debugMode )
			{
				// Show water
				if( cell->underwater )
				{
					double transX = x - (viewX - viewWidth / 2.0);
					double transY = y - (viewY - viewHeight / 2.0);
					glPushMatrix();
					glTranslatef(transX, transY, -5.0);
					glDisable(GL_TEXTURE_2D);
					glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
					glBegin(GL_QUADS);
						glVertex2i(0, 0);
						glVertex2i(1, 0);
						glVertex2i(1, 1);
						glVertex2i(0, 1);
					glEnd();
					glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
					glBegin(GL_LINE_LOOP);
						glVertex2i(0, 0);
						glVertex2i(1, 0);
						glVertex2i(1, 1);
						glVertex2i(0, 1);
					glEnd();
					glPopMatrix();
				}
			}
		}
	}

	// Sort entities by layer
	std::vector<Entity*> entities;
	entities.insert(entities.end(), entitiesToRender.begin(), entitiesToRender.end());
	std::sort( entities.begin(), entities.end(), [](Entity* a, Entity* b){return ((a->layer < b->layer) || (a->layer == b->layer && a < b));});

	// Render all entities
	std::stringstream str;
	for( auto entity : entities )
	{
		renderEntity(entity, viewX, viewY, viewWidth, viewHeight);
	}
}

void World::renderEntity(Entity* entity, double viewX, double viewY, double viewWidth, double viewHeight)
{
	glPushMatrix();
	double transX = entity->getX() - (viewX - viewWidth / 2.0);
	double transY = entity->getY() - (viewY - viewHeight / 2.0);

	// Render to the nearest pixel
	transX = std::floor( (double)UNIT_SIZE * transX ) / (double)UNIT_SIZE;
	transY = std::floor( (double)UNIT_SIZE * transY ) / (double)UNIT_SIZE;

	glTranslatef( transX, transY, entity->layer);

	// Draw the frame for the entity
	const Animation::Frame* frame = entity->getActiveAnimationFrame(frameNumber);
	renderFrame( entity, entity->activeAnimation, frame, false );

	// If it is a tile, draw its tileset
	Tile* tile = dynamic_cast<Tile*>(entity);
	if( tile != nullptr && tile->tilesetEnabled )
	{
		for( int x = 0; x < tile->width; x++ )
		{
			for( int y = 0; y < tile->height; y++ )
			{
				glPushMatrix();
				glTranslatef( x, y, 0 );
				const Animation* animation = tile->getTilesetAnimation( x, y );
				if( animation != nullptr )
				{
					renderFrame( entity, animation, &animation->getFrame(frameNumber), true );
				}
				glPopMatrix();
			}
		}
	}

	if( SETTINGS.debugMode )
	{
		glDisable(GL_TEXTURE_2D);

		glColor4ub(0xff, 0xff, 0xff, 0x7f);
		glBegin(GL_LINE_LOOP);
			glVertex2d(0.0, 0.0);
			glVertex2d(entity->getWidth(), 0.0);
			glVertex2d(entity->getWidth(), entity->getHeight());
			glVertex2d(0.0, entity->getHeight());
		glEnd();

		Tile* tile = dynamic_cast<Tile*>(entity);
		if( tile != nullptr )
		{
			glBegin(GL_LINES);
			for( int x = 0; x < tile->width; ++x )
			{
				for( int y = 0; y < tile->height; ++y )
				{
					if( getCellEdgeState(tile->x + x, tile->y + y, EDGE_BOTTOM) )
					{
						glColor4ub(0xff, 0x00, 0x00, 0xff);
						glVertex2i(x, y);
						glVertex2i(x + 1, y);
					}
					if( getCellEdgeState(tile->x + x, tile->y + y, EDGE_LEFT) )
					{
						glColor4ub(0x00, 0x00, 0xff, 0xff);
						glVertex2i(x, y);
						glVertex2i(x, y + 1);
					}
					if( getCellEdgeState(tile->x + x, tile->y + y, EDGE_RIGHT) )
					{
						glColor4ub(0x00, 0x00, 0xff, 0xff);
						glVertex2i(x + 1, y);
						glVertex2i(x + 1, y + 1);
					}
					if( getCellEdgeState(tile->x + x, tile->y + y, EDGE_TOP) )
					{
						glColor4ub(0x00, 0xff, 0x00, 0xff);
						glVertex2i(x, y + 1);
						glVertex2i(x + 1, y + 1);
					}
				}
			}
			glColor4ub(0x00, 0x00, 0xff, 0xff);
			if( tile->collisionBehavior == TILE_SLOPE_DOWN )
			{
				glVertex2i(0, tile->height);
				glVertex2i(tile->width, 0);
			}
			else if( tile->collisionBehavior == TILE_SLOPE_UP )
			{
				glVertex2i(0, 0);
				glVertex2i(tile->width, tile->height);
			}
			glEnd();
		}

		Sprite* sprite = dynamic_cast<Sprite*>(entity);
		if( sprite != nullptr && (sprite->spriteCollisionsEnabled || sprite->tileCollisionsEnabled) )
		{
			double w = sprite->size.x;
			double h = sprite->size.y;
			glColor4ub(0xff, 0xff, 0xff, 0xff);
			glBegin(GL_LINE_LOOP);
			glVertex2d(0, 0);
			glVertex2d(w, 0);
			glVertex2d(w, h);
			glVertex2d(0, h);
			glEnd();
		}
	} // debugMode

	entity->onRender();

	glPopMatrix();
}

void World::renderFrame( Entity* entity, const Animation* animation, const Animation::Frame* frame, bool tileset )
{
	if( frame != nullptr )
	{
		bool mirrorX = animation->getHorizontalOrientation() != entity->horizontalOrientation;
		bool mirrorY = animation->getVerticalOrientation() != entity->verticalOrientation;

		double left = frame->left;
		double right = frame->right;
		double bottom = frame->bottom;
		double top = frame->top;
		double xOffset = frame->xOffset + entity->offset.x;
		double yOffset = frame->yOffset + entity->offset.y;
		if( mirrorX )
		{
			std::swap(left, right);
			xOffset *= -1.0;
		}
		if( mirrorY )
		{
			std::swap(bottom, top);
		}

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		if( tileset )
		{
			glTranslatef( xOffset, yOffset, 0.0 );
		}
		else
		{
			double transX = entity->getWidth() / 2.0 - frame->width / 2.0 + xOffset;
			double transY = yOffset;
			transX = std::floor( (double)UNIT_SIZE * transX ) / (double)UNIT_SIZE;
			transY = std::floor( (double)UNIT_SIZE * transY ) / (double)UNIT_SIZE;
			glTranslatef(transX, transY, 0.0);
		}
		glColor4f(entity->redMask, entity->greenMask, entity->blueMask, entity->alpha);
		glBegin(GL_QUADS);
			glTexCoord2d(left, bottom);
			glVertex2d(0.0, 0.0);
			glTexCoord2d(right, bottom);
			glVertex2d(frame->width, 0.0);
			glTexCoord2d(right, top);
			glVertex2d(frame->width, frame->height);
			glTexCoord2d(left, top);
			glVertex2d(0.0, frame->height);
		glEnd();
		glPopMatrix();
	}
}

void World::setBackground( const std::string& name )
{
	background = GET_BACKGROUND(name);
}

void World::setLevel( int levelId )
{
	unloadLevel();
	const Level* level = GAME_SESSION.episode->getLevel(levelId);
	time = level->time * GAME_FPS;
	timeFrozen = false;
	loadLevel(level);
}

void World::setPlayer( Player* player )
{
	this->player = player;
}

void World::setTile(int x, int y, Tile* tile)
{
	tile->world = this;
	tile->x = x;
	tile->y = y;
	tile->firstFrame = frameNumber;
	tile->onInit();
	bool tileInserted = false;
	for( int a = x; a < x + tile->width; a++ )
	{
		for( int b = y; b < y + tile->height; b++ )
		{
			Cell* cell = getCell(a, b);
			if( cell != nullptr )
			{
				tileInserted = true;
				///@todo find a way to delete tiles that are completely obscured by new tiles
#if 0
				if( cell->tile != nullptr )
				{
					eraseTile(cell->tile);
					delete cell->tile;
				}
#endif
				cell->tile = tile;
			}
		}
	}

	if( !tileInserted )
	{
		eraseTile(tile);
	}
}

void World::setTimeEnabled( bool enabled )
{
	timeFrozen = !enabled;
}

void World::unloadLevel()
{
	// Free all entities
	std::set<Tile*> tiles;
	for( Cell& cell : cells )
	{
		if( cell.tile != nullptr )
		{
			tiles.insert(cell.tile);
		}
	}
	for( auto tile : tiles )
	{
		delete tile;
	}
	for( auto sprite : sprites )
	{
		///@todo audit onDestroy() methods for anything that should be done in the destructor instead. not sure why this call was here in the first place.
		//sprite->onDestroy();
		delete sprite;
	}

	// Clear the cell array
	cells.resize(0);
	destroyDeadTiles();

	// Clear sprites
	sprites.clear();
}

void World::update(double dt)
{
	delta = dt;
	frameNumber++;
	if( time > 0 && !timeFrozen )
	{
		time--;
	}

	// Do movement and collision checking for all sprites
	std::list<Sprite*> deadSprites;
	for( auto sprite : sprites )
	{
		updateSprite(sprite);

		if( sprite->dead )
		{
			deadSprites.push_back(sprite);
		}
	}
	for( auto sprite : deadSprites )
	{
		sprite->onDestroy();
		destroySprite(sprite);
	}
	destroyDeadTiles();
}

void World::updateSprite(Sprite* sprite)
{
	// Save old kinematic quantities to compare them if the Sprite changes them
	Vector2<double> oldPosition = sprite->position;
	bool tileCollisionsEnabled = sprite->tileCollisionsEnabled;
	bool spriteCollisionsEnabled = sprite->spriteCollisionsEnabled;

	// Motion update
	if( sprite->motionEnabled )
	{
		// Have the sprite update its motion (acceleration, forces, etc.)
		sprite->onPreUpdate();
		Vector2<double> acceleration = sprite->acceleration;
		if( sprite->gravityEnabled )
		{
			acceleration += Vector2<double>(0, Sprite::DEFAULT_GRAVITY);
		}

		// Update velocity
		sprite->velocity += (acceleration * delta);

		// Cap velocity
		if( sprite->maximumSpeed.x != 0 && std::fabs(sprite->velocity.x) > sprite->maximumSpeed.x )
		{
			sprite->velocity.x = sign(sprite->velocity.x) * sprite->maximumSpeed.x;
		}
		else if( std::fabs(sprite->velocity.x) > 1.0 / delta )
		{
			sprite->velocity.x = sign(sprite->velocity.x) / delta;
		}
		if( sprite->maximumSpeed.y != 0 && std::fabs(sprite->velocity.y) > sprite->maximumSpeed.y )
		{
			sprite->velocity.y = sign(sprite->velocity.y) * sprite->maximumSpeed.y;
		}
		else if( std::fabs(sprite->velocity.y) > 1.0 / delta )
		{
			sprite->velocity.y = sign(sprite->velocity.y) / delta;
		}

		// Update position
		sprite->position += sprite->velocity * delta;
	}

	// Now, do collision testing
	if( !sprite->held && (spriteCollisionsEnabled || tileCollisionsEnabled) )
	{
		// Do x axis testing
		bool stopX = doSpriteCollisionXAxisTest(sprite, oldPosition, tileCollisionsEnabled, spriteCollisionsEnabled);

		// Do y axis testing
		bool stopY = doSpriteCollisionYAxisTest(sprite, oldPosition, tileCollisionsEnabled, spriteCollisionsEnabled);

		// Stop if we need to
		if( stopX )
		{
			sprite->velocity.x = 0;
		}
		if( stopY )
		{
			sprite->velocity.y = 0;
		}
	}

	// Check if the sprite is out of bounds
	if( sprite->deathBoundaryEnabled &&
		(sprite->position.x < -1 * BOUNDARY_SIZE || sprite->position.x > width + BOUNDARY_SIZE ||
		sprite->position.y < -1 * BOUNDARY_SIZE || sprite->position.y > height + BOUNDARY_SIZE) )
	{
		// Kill it
		sprite->kill();
		return;
	}
	// Invisible side boundaries for the player
	else if( dynamic_cast<Player*>(sprite) != nullptr )
	{
		if( sprite->getLeft() < 0 )
		{
			sprite->position.x = 0;
			sprite->velocity.x = 0;
		}
		else if( sprite->getRight() > width )
		{
			sprite->position.x = width - sprite->size.x;
			sprite->velocity.x = 0;
		}
	}

	// Remove the sprite from the grid
	eraseSprite(sprite);

	// Re-add the sprite to the grid
	insertSprite(sprite);

	// Post update (after movement)
	sprite->onPostUpdate();
}
