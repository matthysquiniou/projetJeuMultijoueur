#include "TextureFactory.h"

cpu_texture* TextureFactory::Create(EntityType entityType)
{
	cpu_texture* texture = new cpu_texture();

	switch (entityType)
	{
	case EntityType::TITLE:
		texture->Load("../../res/title.png");
		break;
	case EntityType::ARROW:
		texture->Load("../../res/arrow.png");
		break;
	case EntityType::ARROW_DOWN:
		texture->Load("../../res/arrow_down.png");
		break;
	case EntityType::ENGINE_DIRECTION:
		texture->Load("../../res/direction_indicator.png");
		break;
	case EntityType::ENGINE_POWER:
		texture->Load("../../res/engine_indicator.png");
		break;
	case EntityType::HEALTH6:
		texture->Load("../../res/Health/HP_6.png");
		break;
	case EntityType::HEALTH5:
		texture->Load("../../res/Health/HP_5.png");
		break;
	case EntityType::HEALTH4:
		texture->Load("../../res/Health/HP_4.png");
		break;
	case EntityType::HEALTH3:
		texture->Load("../../res/Health/HP_3.png");
		break;
	case EntityType::HEALTH2:
		texture->Load("../../res/Health/HP_2.png");
		break;
	case EntityType::HEALTH1:
		texture->Load("../../res/Health/HP_1.png");
		break;
	case EntityType::HEALTH0:
		texture->Load("../../res/Health/HP_0.png");
		break;
	case EntityType::BULLET:
		texture->Load("../../res/bullet.png");
		break;
	case EntityType::BULLET_EMPTY:
		texture->Load("../../res/bullet_empty.png");
		break;
	case EntityType::CROSSHAIR:
		texture->Load("../../res/crosshair.png");
		break;
	}
	return texture;
}
