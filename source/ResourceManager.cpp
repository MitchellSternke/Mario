#include <cstdlib>

#include <rapidxml_utils.hpp>

#include "Animation.hpp"
#include "Background.hpp"
#include "BitmapFont.hpp"
#include "Exception.hpp"
#include "Globals.hpp"
#include "Image.hpp"
#include "LevelTheme.hpp"
#include "Music.hpp"
#include "ResourceManager.hpp"
#include "Sound.hpp"
#include "Texture.hpp"

using namespace rapidxml;

static std::string getResourceFileName( const std::string& relativeFileName )
{
	return std::string("resources/") + relativeFileName;
}

ResourceManager::ResourceManager() :
	isMainResourceManager(true),
	textureAtlas(nullptr),
	atlasImage(nullptr),
	atlasX(0),
	atlasY(0),
	atlasMaxHeight(0),
	parent(nullptr)
{
}

ResourceManager::ResourceManager( const ResourceManager& parent ) :
	isMainResourceManager(false),
	textureAtlas(nullptr),
	atlasImage(nullptr),
	atlasX(0),
	atlasY(0),
	atlasMaxHeight(0),
	parent(&parent)
{
}

ResourceManager::~ResourceManager()
{
	// No need to free anything unless we are the main resource manager
	if( !isMainResourceManager )
	{
		return;
	}

	// Free the texture atlas
	delete textureAtlas;

	// Free the texture atlas image if it is still loaded in memory
	delete atlasImage;

	// Free all other textures
	for( auto texture : textures )
	{
		delete texture;
	}

	// Free all resources
	LOG << "Freeing all resources... ";
	for( auto resource : resources )
	{
		switch( resource.second.type )
		{
			case RESOURCE_ANIMATION:
				delete resource.second.animation;
				break;
			case RESOURCE_BACKGROUND:
				delete resource.second.background;
				break;
			case RESOURCE_FONT:
				delete resource.second.font;
				break;
			case RESOURCE_LEVELTHEME:
				delete resource.second.levelTheme;
				break;
			case RESOURCE_MUSIC:
				delete resource.second.music;
				break;
			case RESOURCE_SOUND:
				delete resource.second.sound;
				break;
		}
	}

	// Free all resource groups
	for( auto group : groups )
	{
		delete group.second;
	}

	LOG << "Done.\n";
}

void ResourceManager::bindTextureAtlas() const
{
	if( textureAtlas != nullptr )
	{
		textureAtlas->bind();
	}
}

const Animation* ResourceManager::getAnimation( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_ANIMATION )
	{
		return nullptr;
	}
	return resource->animation;
}

const Background* ResourceManager::getBackground( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_BACKGROUND )
	{
		LOG << "Warning: requested background resource \"" << name << "\" not found!\n";
		return nullptr;
	}

	return resource->background;
}

const BitmapFont* ResourceManager::getFont( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_FONT )
	{
		LOG << "Warning: requested font resource \"" << name << "\" not found!\n";
		return nullptr;
	}

	return resource->font;
}

const LevelTheme* ResourceManager::getLevelTheme( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_LEVELTHEME )
	{
		return nullptr;
	}

	return resource->levelTheme;
}

const std::vector<LevelTheme*>& ResourceManager::getLevelThemes() const
{
	return levelThemes;
}

int ResourceManager::getLoadingProgress() const
{
	return resources.size();
}

const Music* ResourceManager::getMusic( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_MUSIC )
	{
		return nullptr;
	}

	return resource->music;
}

const ResourceManager::Resource* ResourceManager::getResource( const std::string& name ) const
{
	auto it = resources.find(name);
	if( it == resources.end() && parent == nullptr )
	{
		return nullptr;
	}
	else if( parent != nullptr )
	{
		return parent->getResource( name );
	}
	else
	{
		return &((*it).second);
	}
}

const ResourceManager* ResourceManager::getResourceGroup( const std::string& name ) const
{
	auto it = groups.find(name);
	if( it == groups.end() )
	{
		LOG << "Warning: resource group \"" << name << "\" not found!\n";
		return nullptr;
	}

	return (*it).second;
}

const Sound* ResourceManager::getSound( const std::string& name ) const
{
	const Resource* resource = getResource(name);
	if( resource == nullptr || resource->type != RESOURCE_SOUND )
	{
		return nullptr;
	}

	return resource->sound;
}

void ResourceManager::loadAnimations( xml_node<>* root )
{
	// Create an image to serve as a texture atlas if one hasn't been created already
	///@todo move this constant elsewhere
	static const int ATLAS_SIZE = 2048; // Storage size for 16k 16x16 tiles. Change if this ever gets too small
	if( atlasImage == nullptr )
	{
		atlasImage = new Image(ATLAS_SIZE, ATLAS_SIZE);

		atlasX = 0;
		atlasY = 0;
		atlasMaxHeight = 0;
	}

	// Create a cache for images
	std::map< std::string, Image* > images;

	// Structure for frames with raw information (not atlas coordinates)
	struct Frame
	{
		int x, y, w, h, xo, yo, duration;
		Image* image;
		std::string imageFile;
		bool mirrorX, mirrorY;
	};

	// Cache for frame templates
	std::map< std::string, Frame > frames;

	// Cache for duplicate frames
	std::map< std::string, Animation::Frame > atlasFrames;

	// Enumerate animations
	LOG << "Loading animations...\n";
	for( xml_node<>* node = root->first_node("animation"); node != nullptr; node = node->next_sibling("animation") )
	{
		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring an animation that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Check for x/y orientation of the animation
		bool xOrientation = false;
		bool yOrientation = true;
		xml_attribute<>* orientationAttr = node->first_attribute("x_orientation");
		if( orientationAttr != nullptr && strcmp(orientationAttr->value(), "right") == 0 )
		{
			xOrientation = true;
		}
		orientationAttr = node->first_attribute("y_orientation");
		if( orientationAttr != nullptr && strcmp(orientationAttr->value(), "down") == 0 )
		{
			yOrientation = false;
		}

		// Create an Animation Resource
		Animation* animation = new Animation(xOrientation, yOrientation);
		Resource resource;
		resource.type = RESOURCE_ANIMATION;
		resource.animation = animation;

		// These are preserved between frames to allow quasi-inheritance of frame properties
		int x = 0;
		int y = 0;
		int w = 1;
		int h = 1;
		int xo = 0;
		int yo = 0;
		int duration = 1;
		Image* image = nullptr;
		std::string imageFile;
		bool mirrorX = false;
		bool mirrorY = false;

		// Load all frames for the animation
		for( xml_node<>* frame = node->first_node("frame"); frame != nullptr; frame = frame->next_sibling("frame") )
		{
			// Whether the frame with the given ID should be saved for other frames to inherit
			bool createTemplate = false;

			// Check if the frame has an id
			xml_attribute<>* attr = frame->first_attribute("id");
			if( attr != nullptr )
			{
				// Check if the ID already exists
				std::map< std::string, Frame >::iterator it = frames.find( attr->value() );
				if( it == frames.end() )
				{
					// Copy this frame when it is done being parsed later...
					///@todo this is unnecessary, remove this functionality
					createTemplate = true;
				}
				else
				{
					Frame& f = (*it).second;
					x = f.x;
					y = f.y;
					w = f.w;
					h = f.h;
					xo = f.xo;
					yo = f.yo;
					duration = f.duration;
					image = f.image;
					imageFile = f.imageFile;
					mirrorX = f.mirrorX;
					mirrorY = f.mirrorY;
				}
			}

			// Check if it references a frame that we already loaded from another animation
			attr = frame->first_attribute("animation");
			if( attr != nullptr )
			{
				std::string animationName = attr->value();
				const Animation* a = getAnimation(animationName);
				if( a == nullptr )
				{
					LOG << "Warning : animation frame for animation \"" << idAttr->value() << "\" had an invalid animation specified.\n";
					continue;
				}

				attr = frame->first_attribute("index");
				if( attr == nullptr )
				{
					LOG << "Warning: animation frame for animation \"" << idAttr->value() << "\" had an animation specified without an index.\n";
					continue;
				}

				// Read the index as hex
				int index = strtol( attr->value(), NULL, 16 );
				if( index < 0 || index >= a->getLength() )
				{
					LOG << "Warning: animation frame for animation \"" << idAttr->value() << "\" had a bad index specified.\n";
					continue;
				}

				// Allow the duration of the frame to be specified
				attr = frame->first_attribute("time");
				if( attr != nullptr )
				{
					duration = std::atoi(attr->value());
				}

				// Also allow the mirroring of the frame to be specified
				attr = frame->first_attribute("mirror_x");
				if( attr != nullptr )
				{
					mirrorX = (std::atoi(attr->value()) == 1);
				}
				attr = frame->first_attribute("mirror_y");
				if( attr != nullptr )
				{
					mirrorY = (std::atoi(attr->value()) == 1);
				}

				Animation::Frame f =  a->getFrameBySequence(index);
				if( mirrorX )
				{
					std::swap(f.left, f.right);
				}
				if( mirrorY )
				{
					std::swap(f.bottom, f.top);
				}
				animation->addFrame( f, duration );
				continue;
			}

			// Get the image for the frame
			attr = frame->first_attribute("image");
			if( attr != nullptr )
			{
				imageFile = getResourceFileName(attr->value());
				std::map< std::string, Image* >::iterator it = images.find(imageFile);
				if( it == images.end() )
				{
					// Load the image
					image = new Image(imageFile);
					images[imageFile] = image;
				}
				else
				{
					// We've already loaded it
					image = (*it).second;
				}
			}
			else if( image == nullptr )
			{
				// No image found for this frame
				LOG << "Warning: Frame without a specified image was detected for animation \""
					<< idAttr->value() << "\". The frame will be ignored.\n";
				continue;
			}

			// Get other attributes for the frame
			attr = frame->first_attribute("x");
			if( attr != nullptr )
			{
				x = std::atoi(attr->value());
			}
			attr = frame->first_attribute("y");
			if( attr != nullptr )
			{
				y = std::atoi(attr->value());
			}
			attr = frame->first_attribute("w");
			if( attr != nullptr )
			{
				w = std::atoi(attr->value());
			}
			attr = frame->first_attribute("h");
			if( attr != nullptr )
			{
				h = std::atoi(attr->value());
			}
			attr = frame->first_attribute("xo");
			if( attr != nullptr )
			{
				xo = std::atoi(attr->value());
			}
			attr = frame->first_attribute("yo");
			if( attr != nullptr )
			{
				yo = std::atoi(attr->value());
			}
			attr = frame->first_attribute("time");
			if( attr != nullptr )
			{
				duration = std::atoi(attr->value());
			}
			attr = frame->first_attribute("mirror_x");
			if( attr != nullptr )
			{
				mirrorX = (std::atoi(attr->value()) == 1);
			}
			attr = frame->first_attribute("mirror_y");
			if( attr != nullptr )
			{
				mirrorY = (std::atoi(attr->value()) == 1);
			}

			// Also check if we are loading a sheet of tiles to create the animation
			int xStride = -1;
			int yStride = -1;
			attr = frame->first_attribute("x_stride");
			if( attr != nullptr && !createTemplate )
			{
				xStride = std::atoi(attr->value());
			}
			attr = frame->first_attribute("y_stride");
			if( attr != nullptr && !createTemplate )
			{
				yStride = std::atoi(attr->value());
			}

			// Create a copy of this frame for later use as a template for other frames
			if( createTemplate )
			{
				Frame f;
				f.x = x;
				f.y = y;
				f.w = w;
				f.h = h;
				f.xo = xo;
				f.yo = yo;
				f.duration = duration;
				f.image = image;
				f.imageFile = imageFile;
				f.mirrorX = mirrorX;
				f.mirrorY = mirrorY;
				frames[ frame->first_attribute("id")->value() ] = f;
			}

			// Now, add the frame(s) specified by the tag to the atlas
			int xMin = x;
			int xMax = x + 1;
			int xInc = 1;
			int yMin = y;
			int yMax = y + 1;
			int yInc = 1;
			Image* indexImage = nullptr;
			if( xStride != -1 && yStride != -1 )
			{
				xMax = image->getWidth();
				xInc = w + xStride;
				yMax = image->getHeight();
				yInc = h + yStride;

				// Create an index image depicting the indices for each frame
				indexImage = new Image(image->getWidth() * 2, image->getHeight() * 2);
				for( int a = 0; a < image->getWidth(); a++ )
				{
					for( int b = 0; b < image->getHeight(); b++ )
					{
						Color pixel = image->getPixel(a, b);
						indexImage->setPixel(a * 2, b * 2, pixel);
						indexImage->setPixel(a * 2 + 1, b * 2, pixel);
						indexImage->setPixel(a * 2, b * 2 + 1, pixel);
						indexImage->setPixel(a * 2 + 1, b * 2 + 1, pixel);
					}
				}
			}
			int frameNumber = 0;
			std::string frameString;
			bool addToAtlas = true;
			for( y = yMin; y < yMax; y += yInc )
			{
				for( x = xMin; x < xMax; x += xInc )
				{
					Animation::Frame f;
					if( xStride == -1 || yStride == -1 )
					{
						// Only copy to the atlas if the frame isn't a duplicate of one that has already been loaded
						// Construct a unique string that identifies the file and pixels for the frame, and hash it
						std::stringstream frameId;
						frameId << imageFile << ':' << x << ':' << y << ':' << w << ':' << h;
						frameString = frameId.str();
						std::map< std::string, Animation::Frame >::iterator it = atlasFrames.find( frameString );
						if( it != atlasFrames.end() )
						{
							// We don't need to add it to the atlas; it's already been copied
							f = (*it).second;
							f.xOffset = (double)xo / (double)UNIT_SIZE;
							f.yOffset = (double)yo / (double)UNIT_SIZE * -1.0;
							addToAtlas = false;
						}
					}

					if( addToAtlas )
					{
						// Check if the frame is all blank pixels
						// If so, skip it since it is a waste to store it
						bool skipFrame = true;
						for( int a = x; a < x + w; a++ )
						{
							for( int b = y; b < y + h; b++ )
							{
								if( image->getPixel(a, b).a != 0 )
								{
									skipFrame = false;
								}
							}
						}
						if( skipFrame )
						{
							continue;
						}

						// Wrap around the atlas when a row is full
						if( atlasX + w >= ATLAS_SIZE )
						{
							atlasX = 0;
							atlasY += atlasMaxHeight;
							atlasMaxHeight = 0;
						}

						// Copy pixels from the image to the atlas
						for( int a = 0; a < w; ++a )
						{
							for( int b = 0; b < h; ++b )
							{
								///@todo add blit function for Image
								atlasImage->setPixel(atlasX + a, atlasY + b, image->getPixel(x + a, y + b));
							}
						}

						f.left = (double)atlasX / (double)ATLAS_SIZE + 1e-6;
						f.right = (double)(atlasX + w) / (double)ATLAS_SIZE - 1e-6;
						f.bottom = (double)(atlasY + h) / (double)ATLAS_SIZE - 1e-6;
						f.top = (double)atlasY / (double)ATLAS_SIZE + 1e-6;
						f.xOffset = (double)xo / (double)UNIT_SIZE;
						f.yOffset = (double)yo / (double)UNIT_SIZE * -1.0;
						f.width = (double)w / (double)UNIT_SIZE;
						f.height = (double)h / (double)UNIT_SIZE;
					}

					// Add the frame to the animation
					if( mirrorX )
					{
						std::swap(f.left, f.right);
					}
					if( mirrorY )
					{
						std::swap(f.bottom, f.top);
					}
					animation->addFrame(
						f.left,
						f.right,
						f.bottom,
						f.top,
						f.xOffset,
						f.yOffset,
						f.width,
						f.height,
						duration
					);

					if( xStride == -1 || yStride == -1 )
					{
						// Save the frame in case there are duplicate uses of it later
						atlasFrames[frameString] = f;
					}
					else
					{
						// Save the frame number in the index image
						char tempString[8];
						sprintf(tempString, "%x", frameNumber);
						indexImage->writeText(x * 2, y * 2, Color::WHITE, tempString);
					}

					// Advance along the atlas row
					if( addToAtlas )
					{
						atlasX += w;
						if( h > atlasMaxHeight )
						{
							atlasMaxHeight = h;
						}
					}

					frameNumber++;
				} // Loop through all frames to be added (x direction)
			} // Loop through all frames to be added (y direction)

			if( indexImage != nullptr )
			{
				if( SETTINGS.debugMode )
				{
					indexImage->save( std::string(idAttr->value()) + "_index.png" );
				}
				delete indexImage;
			}

			// Restore values for x and y for frames that will reuse them
			x = xMin;
			y = yMin;
		} // Loop through <frame> tags

		// Save the resource
		resources[idAttr->value()] = resource;

		LOG << "Loaded animation \"" << idAttr->value() << "\".\n";
	} // Enumerate animations

	// Free loaded images
	for( auto image : images )
	{
		delete image.second;
	}
}

void ResourceManager::loadBackgrounds( xml_node<>* root )
{
	// Enumerate backgrounds
	LOG << "Loading backgrounds...\n";
	for( xml_node<>* node = root->first_node("background"); node != nullptr; node = node->next_sibling("background") )
	{
		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a background that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Check if it has a tiling specified
		xml_attribute<>* tilingAttr = node->first_attribute("tiling");
		BackgroundTiling tiling = BACKGROUND_HORIZONTAL;
		if( tilingAttr != nullptr )
		{
			if( strcmp( tilingAttr->value(), "horizontal" ) == 0 )
			{
				tiling = BACKGROUND_HORIZONTAL;
			}
			else if( strcmp( tilingAttr->value(), "vertical" ) == 0 )
			{
				tiling = BACKGROUND_VERTICAL;
			}
			else if( strcmp( tilingAttr->value(), "all" ) == 0 )
			{
				tiling = BACKGROUND_ALL;
			}
			else
			{
				LOG << "Warning: Background resource \"" << idAttr->value() << "\" has invalid tiling attribute value \"" <<
					tilingAttr->value() << "\". Ignoring and using default value of horizontal.\n";
			}
		}

		// Check if it has a frame time specified
		xml_attribute<>* timeAttr = node->first_attribute("time");
		int time = 1;
		if( timeAttr != nullptr )
		{
			time = std::atoi( timeAttr->value() );
		}

		// Load all frames in the background
		std::vector< Image* > images;
		std::vector< int > indices;
		for( xml_node<>* frameNode = node->first_node("frame"); frameNode != nullptr; frameNode = frameNode->next_sibling("frame") )
		{
			xml_attribute<>* imageAttr = frameNode->first_attribute("image");
			xml_attribute<>* indexAttr = frameNode->first_attribute("index");

			if( imageAttr != nullptr )
			{
				std::string imageName = getResourceFileName(imageAttr->value());
				Image* image = new Image(imageName);
				images.push_back(image);
				indices.push_back(images.size() - 1);
			}
			else if( indexAttr != nullptr )
			{
				int index = std::atoi( indexAttr->value() );
				if( index < 0 || index >= (int)images.size() )
				{
					LOG << "Warning: Ignoring a frame of background resource \"" << idAttr->value() << "\" that had an invalid index specified.\n";
					continue;
				}

				indices.push_back(index);
			}
			else
			{
				LOG << "Warning: Ignoring a frame of background resource \"" << idAttr->value() << "\" that did not have an image or index specified.\n";
				continue;
			}
		}

		// Decide on a texture size to use...
		// This assumes mostly 512 x 512 backgrounds
		int textureSize;
		if( images.size() < 2 )
		{
			textureSize = 512;
		}
		else if( images.size() < 5 )
		{
			textureSize = 1024;
		}
		else if( images.size() < 17 )
		{
			textureSize = 2048;
		}
		else
		{
			textureSize = 4096;
		}

		// Create the image and corresponding animation
		std::vector<Animation::Frame> frames;
		Animation* animation = new Animation();
		Image textureImage( textureSize, textureSize );
		int x = 0;
		int y = 0;
		int maxHeight = 0;
		for( int i = 0; i < (int)images.size(); i++ )
		{
			// Figure out packing coordinates
			Image* image = images[i];
			if( y + image->getHeight() > textureSize )
			{
				LOG << "Warning: Failed to pack texture for background \"" << idAttr->value() << "\".\n";
				delete animation;
				continue;
			}
			if( x + image->getWidth() > textureSize )
			{
				x = 0;
				y += maxHeight;
				maxHeight = image->getHeight();
			}

			if( image->getHeight() > maxHeight )
			{
				maxHeight = image->getHeight();
			}

			// Pack the image
			textureImage.blit( *image, x, y );

			// Save it's animation frame
			Animation::Frame f;
			f.left = (double)x / (double)textureSize + 1e-6;
			f.right = (double)(x + image->getWidth()) / (double)textureSize - 1e-6;
			f.bottom = (double)(y + image->getHeight()) / (double)textureSize - 1e-6;
			f.top = (double)y / (double)textureSize + 1e-6;
			f.xOffset = 0;
			f.yOffset = 0;
			f.width = (double)image->getWidth() / (double)UNIT_SIZE;
			f.height = (double)image->getHeight() / (double)UNIT_SIZE;
			frames.push_back(f);

			x += image->getWidth();
		}

		// Copy frames to the animation
		for( int i = 0; i < (int)indices.size(); i++ )
		{
			animation->addFrame(frames[indices[i]], time );
		}

		// Copy the image to a texture
		Texture* texture = new Texture( textureImage );
		textures.push_back(texture);

		// Create the background resource
		Background* background = new Background( *texture, animation, tiling );
		Resource resource;
		resource.type = RESOURCE_BACKGROUND;
		resource.background = background;
		resources[idAttr->value()] = resource;

		// Free image resources
		for( auto image : images )
		{
			delete image;
		}

		LOG << "Loaded background \"" << idAttr->value() << "\".\n";
	} // Enumerate backgrounds
}

void ResourceManager::loadFonts( xml_node<>* root )
{
	// Enumerate fonts
	LOG << "Loading fonts...\n";
	for( xml_node<>* node = root->first_node("font"); node != nullptr; node = node->next_sibling("font") )
	{
		// Create a Font Resource
		Resource resource;
		resource.type = RESOURCE_FONT;

		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a font that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Check for a file attribute
		xml_attribute<>* fileAttr = node->first_attribute("file");
		if( fileAttr == nullptr )
		{
			LOG << "Warning: Ignoring font with id \"" << idAttr->value() << "\" because no file was specified.\n";
			continue;
		}

		// Check for a width attribute
		xml_attribute<>* widthAttr = node->first_attribute("width");
		if( widthAttr == nullptr )
		{
			LOG << "Warning: Ignoring font with id \"" << idAttr->value() << "\" because no width was specified.\n";
			continue;
		}

		// Check for a height attribute
		xml_attribute<>* heightAttr = node->first_attribute("height");
		if( heightAttr == nullptr )
		{
			LOG << "Warning: Ignoring font with id \"" << idAttr->value() << "\" because no height was specified.\n";
			continue;
		}

		std::string fileName = getResourceFileName(fileAttr->value());
		BitmapFont* font = new BitmapFont(fileName, std::atoi(widthAttr->value()), std::atoi(heightAttr->value()));

		// Add all characters to the font
		for( xml_node<>* ch = node->first_node("char"); ch != nullptr; ch = ch->next_sibling("char") )
		{
			// Check for attributes of the character
			xml_attribute<>* charIdAttr = ch->first_attribute("id");
			xml_attribute<>* xAttr = ch->first_attribute("x");
			xml_attribute<>* yAttr = ch->first_attribute("y");
			xml_attribute<>* numberAttr = ch->first_attribute("number");
			if( charIdAttr == nullptr || xAttr == nullptr || yAttr == nullptr )
			{
				continue;
			}

			char chId = charIdAttr->value()[0];
			int x = std::atoi(xAttr->value());
			int y = std::atoi(yAttr->value());

			if( numberAttr != nullptr )
			{
				int number = std::atoi(numberAttr->value());
				font->addCharacters( chId, x, y, number );
			}
			else
			{
				font->addCharacter( chId, x, y );
			}
		}

		// Save the resource
		resource.font = font;
		resources[idAttr->value()] = resource;

		LOG << "Loaded font \"" << idAttr->value() << "\" from file \"" << fileName<< "\".\n";
	} // Enumerate fonts
}

void ResourceManager::loadGroups( xml_node<>* root )
{
	// Enumerate groups
	LOG << "Loading resource groups...\n";
	for( xml_node<>* node = root->first_node("group"); node != nullptr; node = node->next_sibling("group") )
	{
		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a resource group that did not have an \"id\" attribute.\n";
			continue;
		}
		std::string name = idAttr->value();

		// Check that the parent resource manager exists
		xml_attribute<>* parentAttr = node->first_attribute("parent");
		const ResourceManager* parent = nullptr;
		if( parentAttr != nullptr )
		{
			auto it = groups.find(parentAttr->value());
			if( it != groups.end() )
			{
				parent = (*it).second;
			}
			else
			{
				LOG << "Warning: attempted to add a resource group \"" << name << "with parent \"" <<
					parentAttr->value() << "\" which did not exist. The group will not be added.\n";
				continue;
			}
		}

		auto it = groups.find(name);
		if( it != groups.end() )
		{
			LOG << "Warning: attempted to add a resource group to a name that was already being used: \"" <<
				name << "\". Any resources mapped by the resource group will not be available.\n";
			continue;
		}

		ResourceManager* group = new ResourceManager(*parent);
		groups.insert(it, std::pair<std::string, ResourceManager*>(name, group));

		// Add all key-value pairs to the child
		for( xml_node<>* res = node->first_node("resource"); res != nullptr; res = res->next_sibling("resource") )
		{
			// Check that it has a key-value pair specified
			xml_attribute<>* keyAttr = res->first_attribute("key");
			xml_attribute<>* valueAttr = res->first_attribute("value");
			xml_attribute<>* animationAttr = res->first_attribute("animation"); // Also allow specifying single-frame animations in place of the value
			xml_attribute<>* indexAttr = res->first_attribute("index");
			if( keyAttr == nullptr || (valueAttr == nullptr && (animationAttr == nullptr || indexAttr == nullptr )) )
			{
				LOG << "Warning: resource group \"" << name << "\" had an invalid key-value pair specified. It will be ignored.\n";
				continue;
			}

			// Check that the key is unique and that the value exists
			std::string key = keyAttr->value();
			auto iter = group->resources.find(key);
			if( iter != group->resources.end() )
			{
				LOG << "Warning: resource group \"" << name << "\" specified a key that was already in use: \"" << key << "\".\n";
				continue;
			}

			if( valueAttr != nullptr )
			{
				std::string value = valueAttr->value();
				iter = resources.find(value);
				if( iter == resources.end() )
				{
					LOG << "Warning: resource group \"" << name << "\" specified a value for a resource that does not exist: \"" << value << "\".\n";
					continue;
				}

				// Create the link to the resource
				const Resource* resource = getResource(value);
				Resource newResource(*resource);
				group->resources[key] = newResource;

				LOG << "Added resource key-value pair: " << key << " -> " << value << ".\n";
			}
			else
			{
				// Find the animation
				std::string animationName = animationAttr->value();
				const Animation* animation = getAnimation(animationName);
				if( animation == nullptr )
				{
					LOG << "Warning: resource group \"" << name << "\" with key \"" << key << "\" specified an animation that does not exist: \"" << animationName << "\".\n";
					continue;
				}

				// Read the index as hex
				int index = strtol( indexAttr->value(), NULL, 16 );
				if( index < 0 || index >= animation->getLength() )
				{
					LOG << "Warning: resource group name \"" << name << "\" with key \"" << key << "\" specified an animation as its value with the following issue:\n";
					LOG << "\tanimation frame for animation \"" << animationName << "\" had a bad index specified.\n";
					continue;
				}

				// Check if we've loaded this frame before
				std::string frameName = animationName + '[' + indexAttr->value() + ']';
				const Animation* frameAnimation = getAnimation(frameName);
				if( frameAnimation == nullptr )
				{
					// Does this name conflict with another resource that we've loaded? (Rare)
					if( getResource(frameName) != nullptr )
					{
						LOG << "Warning: couldn't load key-value pair with key \"" << key << "\" and animation \"" << animationName;
						LOG << "\" with index [" << index << "] since the name \"" << frameName << "\" was already being used for a different resource.\n";
						continue;
					}

					// Create the animation
					Animation* a = new Animation;
					a->addFrame( animation->getFrame(index) );

					//Store it in the resource map
					Resource r;
					r.animation = a;
					r.type = RESOURCE_ANIMATION;
					resources[frameName] = r;

					// Now, create a link to it for the group
					group->resources[key] = r;
				}
				else
				{
					// Just create a link to the animation frame resource since it's already been loaded
					const Resource* resource = getResource(frameName);
					Resource newResource(*resource);
					group->resources[key] = newResource;
				}

				LOG << "Added resource key-value pair (with implicit animation value): " << key << " -> " << frameName << ".\n";
			}
		}

		LOG << "Added resource group \"" << name << "\".\n";
	} // Enumerate groups
}

void ResourceManager::loadLevelThemes( xml_node<>* root )
{
	// Enumerate themes
	LOG << "Loading themes...\n";
	for( xml_node<>* node = root->first_node("theme"); node != nullptr; node = node->next_sibling("theme") )
	{
		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a theme that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Load the theme
		LevelTheme* theme = new LevelTheme;
		std::string name = idAttr->value();

		// Load entities
		for( xml_node<>* entity = node->first_node("entity"); entity != nullptr; entity = entity->next_sibling("entity") )
		{
			xml_attribute<>* id = entity->first_attribute("id");
			xml_attribute<>* group = entity->first_attribute("group");
			if( id == nullptr || group == nullptr )
			{
				LOG << "Warning: theme \"" << name << "\" had an entity specified without an id or group attribute.\n";
				continue;
			}

			auto it = groups.find(group->value());
			if( it == groups.end() )
			{
				LOG << "Warning: theme \"" << name << "\" had an entity with an invalid group attribute \"" << group->value() << "\" specified.\n";
				continue;
			}

			ResourceManager* resourceGroup = (*it).second;
			theme->addEntityType( id->value(), resourceGroup );
		}

		// Load additional resources
		for( xml_node<>* res = node->first_node("resource"); res != nullptr; res = res->next_sibling("resource") )
		{
			xml_attribute<>* id = res->first_attribute("id");
			if( id == nullptr )
			{
				LOG << "Warning: theme \"" << name << "\" had a resource specified without an id attribute.\n";
				continue;
			}

			const Resource* resource = getResource(id->value());
			if( resource == nullptr )
			{
				LOG << "Warning: theme \"" << name << "\" had a resource with an invalid id specified: \"" << id->value() << "\".\n";
				continue;
			}

			if( resource->type == RESOURCE_BACKGROUND )
			{
				theme->backgrounds.push_back(resource->background);
			}
			else if( resource->type == RESOURCE_MUSIC )
			{
				theme->musics.push_back(resource->music);
			}
			else
			{
				LOG << "Warning: theme \"" << name << "\" had a resource \"" << id->value() << "\" that was not a background or music track.\n";
				continue;
			}
		}

		// Check if a parent was specified
		xml_attribute<>* parentAttr = node->first_attribute("parent");
		if( parentAttr != nullptr )
		{
			///@todo this code branch is untested... try it out
			// Find the parent level theme
			std::string parentName = parentAttr->value();
			const LevelTheme* parent = getLevelTheme(parentName);
			if( parent == nullptr )
			{
				LOG << "Warning: theme \"" << name << "\" specified a parent theme \"" << parentName << "\" that did not exist.\n";
			}
			else
			{
				// Find anything that the parent has that we don't have and copy it
				theme->inherit( *parent );
			}
		}
		else if( name.compare("default_theme") != 0 )
		{
			// By default, inherit default_theme
			theme->inherit( *getLevelTheme("default_theme") );
		}

		// Create the resource
		Resource resource;
		resource.type = RESOURCE_LEVELTHEME;
		resource.levelTheme = theme;
		resources[name] = resource;
		levelThemes.push_back(theme);

		LOG << "Loaded theme \"" << name << "\".\n";
	} // Enumerate themes
}

void ResourceManager::loadMusic( xml_node<>* root )
{
	// Enumerate music
	LOG << "Loading music...\n";
	for( xml_node<>* node = root->first_node("music"); node != nullptr; node = node->next_sibling("music") )
	{
		// Create a Music Resource
		Resource resource;
		resource.type = RESOURCE_MUSIC;

		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a music track that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Check for a file attribute
		xml_attribute<>* fileAttr = node->first_attribute("file");
		if( fileAttr == nullptr )
		{
			LOG << "Warning: Ignoring music track with id \"" << idAttr->value() << "\" because no file was specified.\n";
			continue;
		}

		std::string fileName = getResourceFileName(fileAttr->value());
		Music* music = new Music(fileName);
		if( !music->isLoaded() )
		{
			LOG << "Warning: Unable to load music track \"" << idAttr->value() << "\" from file \"" << fileName << "\".\n";
			delete music;
			continue;
		}

		// Save the resource
		resource.music = music;
		resources[idAttr->value()] = resource;

		LOG << "Loaded music track \"" << idAttr->value() << "\" from file \"" << fileName<< "\".\n";
	} // Enumerate music
}

void ResourceManager::loadResources( const std::string& resourceFileName )
{
	// Only the root level resource manager can load resources
	assert(isMainResourceManager);

	// Load the main resource file
	loadResourcesFromFile(resourceFileName);

	// Generate the texture atlas
	textureAtlas = new Texture(*atlasImage);
	if( SETTINGS.debugMode )
	{
		atlasImage->save("atlas.png");
	}
	delete atlasImage;
	atlasImage = nullptr;

	LOG << "Done loading resources.\n";
}

void ResourceManager::loadResourcesFromFile( const std::string& resourceFileName )
{
	assert(isMainResourceManager);

	LOG << "Loading resources from file \"" << resourceFileName << "\"...\n";

	// Parse the XML document
	xml_document<> document;
	file<> xmlFile(resourceFileName.c_str());
	document.parse<0>(xmlFile.data());
	xml_node<>* root = document.first_node();

	// Load any resource file imports specified in the file
	for( xml_node<>* node = root->first_node("import"); node != nullptr; node = node->next_sibling("import") )
	{
		xml_attribute<>* fileAttr = node->first_attribute("file");
		if( fileAttr == nullptr )
		{
			// No point in having an error message here...
			continue;
		}

		std::string fileName = getResourceFileName(fileAttr->value());
        try
        {
        	loadResourcesFromFile(fileName);
        }
        catch( std::exception& e )
        {
        	LOG << "Failed to load resources from file \"" << fileName << "\". Exception: " << e.what() << std::endl;
        }
	}

	// Load all resources in the document
	loadMusic( root );
	loadSounds( root );
	loadFonts( root );
	loadAnimations( root );
	loadBackgrounds( root );
	loadGroups( root );
	loadLevelThemes( root );

	LOG << "Finished loading resources from file \"" << resourceFileName << "\".\n";
}

void ResourceManager::loadSounds( xml_node<>* root )
{
	// Enumerate sounds
	LOG << "Loading sounds...\n";
	for( xml_node<>* node = root->first_node("sound"); node != nullptr; node = node->next_sibling("sound") )
	{
		// Create a Sound Resource
		Resource resource;
		resource.type = RESOURCE_SOUND;

		// Check that it has an id
		xml_attribute<>* idAttr = node->first_attribute("id");
		if( idAttr == nullptr )
		{
			LOG << "Warning: Ignoring a sound effect that did not have an \"id\" attribute.\n";
			continue;
		}

		// Check for duplicates
		auto it = resources.find(idAttr->value());
		if( it != resources.end() )
		{
			LOG << "Warning: duplicate resource \"" << idAttr->value() << "\" found. It will be ignored.\n";
			continue;
		}

		// Check for a file attribute
		xml_attribute<>* fileAttr = node->first_attribute("file");
		if( fileAttr == nullptr )
		{
			LOG << "Warning: Ignoring sound effect with id \"" << idAttr->value() << "\" because no file was specified.\n";
			continue;
		}

		std::string fileName = getResourceFileName(fileAttr->value());
		Sound* sound = new Sound(fileName);
		if( !sound->isLoaded() )
		{
			LOG << "Warning: Unable to load sound effect \"" << idAttr->value() << "\" from file \"" << fileName << "\".\n";
			delete sound;
			continue;
		}

		// Save the resource
		resource.sound = sound;
		resources[idAttr->value()] = resource;

		LOG << "Loaded sound effect \"" << idAttr->value() << "\" from file \"" << fileName << "\".\n";
	} // Enumerate sounds
}

void ResourceManager::playMusic( const std::string& trackName, bool loop ) const
{
	// Only play music if the option is turned on
	if( !SETTINGS.music )
	{
		return;
	}

	const Music* music = getMusic(trackName);
	if( music == nullptr )
	{
		LOG << "Warning: requested music resource \"" << trackName << "\" not found!\n";
		return;
	}
	music->play(loop);
}

void ResourceManager::playSound( const std::string& soundName, int channel ) const
{
	// Only play sound if the option is turned on
	if( !SETTINGS.sound )
	{
		return;
	}

	const Sound* sound = getSound(soundName);
	if( sound == nullptr )
	{
		LOG << "Warning: requested sound resource \"" << soundName << "\" not found!\n";
		return;
	}
	sound->play(channel);
}
