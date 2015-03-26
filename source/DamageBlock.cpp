#include "DamageBlock.hpp"
#include "Player.hpp"

DamageBlock::DamageBlock( bool fatal ) :
	Tile(fatal ? TILE_NONE : TILE_SOLID),
	fatal(fatal)
{
	if( fatal )
	{
		setAnimation("damage_block_fatal");
	}
	else
	{
		setAnimation("damage_block");
	}
}

void DamageBlock::onCollision( Sprite& sprite, Edge edge )
{
	// Have the player take damage when colliding with the damage block
	Player* player = dynamic_cast<Player*>(&sprite);
	if( player != nullptr )
	{
		player->takeDamage( fatal );
	}
}
