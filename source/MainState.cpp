#include <boost/algorithm/string.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "BitmapFont.hpp"
#include "Block.hpp"
#include "Coin.hpp"
#include "Flower.hpp"
#include "Font.hpp"
#include "Game.hpp"
#include "Globals.hpp"
#include "Goomba.hpp"
#include "Item.hpp"
#include "Koopa.hpp"
#include "Level.hpp"
#include "MainState.hpp"
#include "Mushroom.hpp"
#include "Music.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "Rendering.hpp"
#include "Shell.hpp"
#include "Star.hpp"
#include "Text.hpp"
#include "Tile.hpp"
#include "World.hpp"

static const double DEAD_PLAYER_INITIAL_VELOCITY = 16.0;
static const double DEAD_PLAYER_GRAVITY = -32.0;

#define VIEW_WIDTH (SETTINGS.screenWidth / (SETTINGS.scale * (double)UNIT_SIZE))
#define VIEW_HEIGHT (SETTINGS.screenHeight / (SETTINGS.scale * (double)UNIT_SIZE))

class DeadPlayer : public Particle
{
public:
	DeadPlayer() :
		Particle( GET_ANIMATION("player_dead") )
	{
		setDeathBoundaryEnabled(false);
		setGravityEnabled(false);
		setYVelocity(DEAD_PLAYER_INITIAL_VELOCITY);
		setYAcceleration(DEAD_PLAYER_GRAVITY);
	}
};

MainState::MainState(int level) :
	GameState(true),
	commandMode(false),
	paused(false),
	deadPlayer(nullptr),
	playerDeathHandled(false),
	endTimer(0),
	initialEndTimer(0)
{
	world = GAME_SESSION.world;
	world->setLevel( level );
	player = GAME_SESSION.player;
	player->setLayer(1);
	player->setCenterX(1.0);
	player->setY(9.0);
	player->reset();
	world->addSprite(player);
	world->setPlayer(player);
	world->playBackgroundMusic();
	INPUT_MANAGER.getController(0)->addListener(*player);
}

MainState::~MainState()
{
	Mix_HaltMusic();
	world->removeSprite(player);
	INPUT_MANAGER.getController(0)->removeListener(*player);
}

void MainState::executeCommand()
{
	// Split into arguments
	std::vector<std::string> args;
	boost::split(args, commandString, [](char c){ return std::isspace(c); });

	for( std::size_t i = 0; i < args.size(); ++i )
	{
		if( args[i].compare("spawn") == 0 )
		{
			if( ++i < args.size() && ++i < args.size() )
			{
				Sprite* sprite = nullptr;
				int number = std::atoi(args[i - 1].c_str());

				for( int j = 0; j < number; ++j )
				{
					if( args[i].compare("goomba") == 0 )
					{
						sprite = new Goomba;
					}
					else if( args[i].compare("coin") == 0 )
					{
						sprite = new Coin;
					}
					else if( args[i].compare("flower") == 0 )
					{
						sprite = new Flower;
					}
					else if( args[i].compare("shell") == 0 )
					{
						sprite = new Shell;
					}
					else if( args[i].compare("star") == 0 )
					{
						sprite = new Star;
					}
					else if( args[i].compare("koopa") == 0 )
					{
						sprite = new Koopa;
					}
					else
					{
						return;
					}

					while( true )
					{
						Random& random = world->getRandom();
						double x = random.nextReal() * 14.0 - 7.0;
						double y = random.nextReal() * 14.0 - 7.0;
						if( std::abs(x) < 2 && std::abs(y) < 2 )
						{
							x = 2;
						}
						x += player->getX();
						y += player->getY();

						if( world->getTile( floor(x), floor(y) ) != nullptr )
						{
							continue;
						}

						world->addSprite(x, y, sprite);
						break;
					}
				}
				LOG << "Spawned " << number << ' ' << args[i] << "(s).\n";
				return;
			}
		}
		else if( args[i].compare("debug-disable") == 0 )
		{
			SETTINGS.debugMode = false;
			LOG << "Debug mode disabled.\n";
			commandMode = false;
			return;
		}
	}
}

void MainState::input()
{
	SDL_Event event;
	while( SDL_PollEvent(&event) )
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				if( commandMode )
				{
					if( event.key.keysym.sym == SDLK_BACKQUOTE )
					{
						commandMode = false;
					}
					else if( event.key.keysym.sym == SDLK_BACKSPACE && !commandString.empty() )
					{
						commandString = commandString.substr(0, commandString.size() - 1);
					}
					else if( std::isprint(event.key.keysym.sym) )
					{
						commandString += event.key.keysym.sym;
					}
					else if( event.key.keysym.sym == SDLK_RETURN )
					{
						executeCommand();
						commandString.clear();
					}
				}
				else
				{
					switch( event.key.keysym.sym )
					{
						case SDLK_e:
							if( SETTINGS.debugMode )
							{
								world->endLevel();
							}
							break;

						case SDLK_p:
							paused = !paused;
							break;

						case SDLK_BACKQUOTE:
							if( SETTINGS.debugMode )
							{
								commandMode = true;
							}
							break;

						case SDLK_ESCAPE:
							getGame().quit();
							break;

						default:
							break;
					}
				}
				break;

			case SDL_QUIT:
				getGame().quit();
				break;

			default:
				break;
		}
	} // while(SDL_PollEvent(&event))

	// Update the player's input state
	if( !commandMode && !player->isDead() && endTimer == 0 )
	{
		INPUT_MANAGER.update();
	}
}

void MainState::render()
{
	static int frames = 0;

	// Render the world
	renderClearScreen();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const Camera& c = player->getCamera();
	double renderX = c.getPosition().x;
	double renderY = c.getPosition().y;

	world->render(
		renderX,
		renderY,
		VIEW_WIDTH,
		VIEW_HEIGHT);

	// Set projection units to pixels
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSetUnitsToPixels();

	// Render the HUD
	const Animation* marioIndicator = GET_ANIMATION("mario_indicator");
	const Animation* times = GET_ANIMATION("times");
	const Animation* timeIndicator = GET_ANIMATION("time_indicator");
	const Animation* coinIndicator = GET_ANIMATION("coin_indicator");
	const Animation* pMeterBarEmpty = GET_ANIMATION("p_meter_bar_empty");
	const Animation* pMeterBarFull = GET_ANIMATION("p_meter_bar_full");
	const Animation* pMeterEmpty = GET_ANIMATION("p_meter_empty");
	const Animation* pMeterFull = GET_ANIMATION("p_meter_full");
	const Animation* reserveItemBox = GET_ANIMATION("reserve_item_box");
	const BitmapFont* font = GET_FONT("font_bold");

	glEnable(GL_TEXTURE_2D);
	RESOURCE_MANAGER.bindTextureAtlas();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Lives indicator
	glTranslatef(font->getCharacterWidth(), SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 2.0, 0.0f);
	Animation::Frame frame = marioIndicator->getFrame(world->getFrameNumber());
	marioIndicator->renderFrame(frame);
	glTranslatef(frame.width * UNIT_SIZE + font->getCharacterWidth(), 0.0f, 0.0f);
	frame = times->getFrame(world->getFrameNumber());
	times->renderFrame(frame);
	glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);
	char tempString[128];
	sprintf(tempString, " %d", player->getLives());
	font->renderText(tempString);

	// P meter
	glLoadIdentity();
	glTranslatef(font->getCharacterWidth(), SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 3.0 - 1.0, 0.0f);
	for( int i = 0; i < Player::P_METER_FULL_BARS; i++ )
	{
		if( i == Player::P_METER_FULL_BARS - 1 )
		{
			if( player->isPMeterFilled() )
			{
				frame = pMeterFull->getFrame(world->getFrameNumber());
				pMeterFull->renderFrame(frame);
			}
			else
			{
				frame = pMeterEmpty->getFrame(world->getFrameNumber());
				pMeterEmpty->renderFrame(frame);			}
		}
		else if( i >= player->getPMeterBars() )
		{
			frame = pMeterBarEmpty->getFrame(world->getFrameNumber());
			pMeterBarEmpty->renderFrame(frame);
		}
		else
		{
			frame = pMeterBarFull->getFrame(world->getFrameNumber());
			pMeterBarFull->renderFrame(frame);
		}

		glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);
	}

	// Reserve item box
	glLoadIdentity();
	frame = reserveItemBox->getFrame(world->getFrameNumber());
	glTranslatef(
		SETTINGS.getRenderedScreenWidth() / 2.0 - frame.width * UNIT_SIZE / 2.0,
		SETTINGS.getRenderedScreenHeight() - frame.height * UNIT_SIZE - 4.0,
		0.0f
	);
	reserveItemBox->renderFrame(frame);
	if( player->getReserveItem() != nullptr )
	{
		const Animation* a = player->getReserveItem()->getActiveAnimation();
		Animation::Frame f = a->getFrame(world->getFrameNumber());
		glTranslatef( (frame.width / 2.0 - f.width / 2.0) * UNIT_SIZE, (frame.height / 2.0 - f.height / 2.0) * UNIT_SIZE, 1.0f );
		a->renderFrame(f);
	}

	// Time
	if( world->getTime() != INFINITE_LEVEL_TIME )
	{
		glLoadIdentity();
		glTranslatef(
			SETTINGS.getRenderedScreenWidth() - font->getCharacterWidth() * 13.0,
			SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 2.0,
			0.0f
		);
		frame = timeIndicator->getFrame(world->getFrameNumber());
		timeIndicator->renderFrame(frame);
		glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);
		sprintf(tempString, "%d", world->getTime());
		font->renderText(tempString);
	}

	// Coins and score
	glLoadIdentity();
	glTranslatef(
		SETTINGS.getRenderedScreenWidth() - font->getCharacterWidth() * 6.0,
		SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 2.0,
		0.0f);
	frame = coinIndicator->getFrame(world->getFrameNumber());
	coinIndicator->renderFrame(frame);
	glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);
	frame = times->getFrame(world->getFrameNumber());
	times->renderFrame(frame);
	glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);
	sprintf(tempString, " %2d", player->getCoins());
	font->renderText(tempString);
	glLoadIdentity();
	glTranslatef(
		SETTINGS.getRenderedScreenWidth() - font->getCharacterWidth() * 11.0,
		SETTINGS.getRenderedScreenHeight() - font->getCharacterHeight() * 3.0 - 1.0,
		0.0f);
	sprintf(tempString, "%10d", player->getScore());
	font->renderText(tempString);

	// Stuff for if we finish a level
	if( world->getStatus().statusType == WORLD_LEVEL_ENDED )
	{
		glLoadIdentity();
		glTranslatef(
			SETTINGS.getRenderedScreenWidth() / 2.0 - font->getCharacterWidth() * 6.0,
			SETTINGS.getRenderedScreenHeight() / 2.0 + font->getCharacterHeight() * 3.0,
			1.0f
		);
		font->renderText("Level Cleared");
		glTranslatef( 0.0f, -2.0 * font->getCharacterHeight(), 0.0f);

		if( world->getTime() != INFINITE_LEVEL_TIME && initialEndTimer - endTimer > 60 )
		{
			frame = timeIndicator->getFrame(world->getFrameNumber());
			timeIndicator->renderFrame(frame);
			glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);


			sprintf(tempString, "%d ", remainingTime);
			font->renderText(tempString);
			glTranslatef( strlen(tempString) * font->getCharacterWidth(), 0.0f, 0.0f);

			frame = times->getFrame(world->getFrameNumber());
			times->renderFrame(frame);
			glTranslatef(frame.width * UNIT_SIZE, 0.0f, 0.0f);

			font->renderText(" 50");
		}
	}

	// Fade out effect
	if( endTimer != 0 && endTimer < 60 )
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.0f, 0.0f, 0.0f, (60 - endTimer) / 60.0f);
		glBegin(GL_QUADS);
			glVertex2i(0, 0);
			glVertex2i(1, 0);
			glVertex2i(1, 1);
			glVertex2i(0, 1);
		glEnd();
	}

	// Render debug stuff
	glDisable(GL_TEXTURE_2D);
	if( SETTINGS.debugMode )
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -10.0, 10.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 10.0f);
		if( commandMode )
		{
			glColor4ub(0x0, 0x0, 0x0, 0x80);
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.7f * VIEW_HEIGHT);
			glVertex2f(VIEW_WIDTH, 0.7f * VIEW_HEIGHT);
			glVertex2f(VIEW_WIDTH, VIEW_HEIGHT);
			glVertex2f(0.0f, VIEW_HEIGHT);
			glEnd();
			glTranslatef( 0.0f, VIEW_HEIGHT - (FONT_HEIGHT + 2) / (double)UNIT_SIZE, 0.0f);
			drawTextScaled(std::string("Debug Command:\n") + commandString + (frames % 30 < 15 ? '_' : ' '));
		}
		else
		{
			glTranslatef(0.0f, VIEW_HEIGHT - (FONT_HEIGHT + 2) / (double)UNIT_SIZE, 0.0f);
			char debugText[256];
			sprintf( debugText, "Debug Mode\n----------\nX: %.2f\nY: %.2f\nV_X: %.2f\nV_Y: %.2f\nA_X: %.2f\nA_Y: %.2f\nFrame: %d",
				player->getX(),
				player->getY(),
				player->getXVelocity(),
				player->getYVelocity(),
				player->getXAcceleration(),
				player->getYAcceleration(),
				world->getFrameNumber() );
			drawBorderedTextScaled(debugText);
		}
		glPopMatrix();
	}
	glEnable(GL_TEXTURE_2D);

	SDL_GL_SwapWindow((SDL_Window*)window);
	frames++;
}

void MainState::update()
{
	// Handle input
	input();

	// Check if time is up
	if( world->getTime() == 0 )
	{
		player->takeDamage(true);
	}

	// Handle player deaths
	if( player->isDead() && !playerDeathHandled )
	{
		playerDeathHandled = true;
		world->removeSprite(player);
		PLAY_MUSIC("death", false);
		world->freezeTime();

		if( player->getBottom() >= 0.0 )
		{
			deadPlayer = new DeadPlayer();
			deadPlayer->setCenterX(player->getCenterX());
			deadPlayer->setBottom(player->getBottom());
			deadPlayer->setCollisionsEnabled(false);
			world->addSprite(deadPlayer);
		}

		endTimer = 240; ///@todo constant
	}
	if( endTimer > 0 )
	{
		if( --endTimer == 0 )
		{
			// Head back to the map now...
			getGame().popState();
			return;
		}
	}

	// Handle the level ending
	if( world->getStatus().statusType == WORLD_LEVEL_ENDED )
	{
		if( endTimer == 0 )
		{
			remainingTime = world->getTime();
			endTimer = 180 + world->getTime() / 5;
			initialEndTimer = endTimer;
			PLAY_MUSIC("course_clear", false);
		}
		else if( initialEndTimer - endTimer > 90 )
		{
			int dt = 5;
			remainingTime -= dt;
			if( remainingTime < 0 )
			{
				dt += remainingTime;
				remainingTime = 0;
			}
			if( dt > 0 )
			{
				player->scorePointsWithoutEffect(50 * dt);
				if( endTimer % 2 == 0 )
				{
					PLAY_SOUND_CHANNEL("count", 0);
				}
			}
		}
	}
	// Processing
	else if( !paused )
	{
		world->update(GAME_DELTA);
	}

	// Rendering
	if( player->isDead() && deadPlayer != nullptr )
	{
		render(/*deadPlayer*/);
	}
	else
	{
		render(/*player*/);
	}
}
