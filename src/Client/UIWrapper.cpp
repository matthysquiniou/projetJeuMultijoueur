#include "UIWrapper.h"
#include "App.h"
#include <cpu-render.h>
#include "cpu_texture.h"
#include <iostream>
#include "EntityType.h"
#include "TextureRegister.h"
#include "Player.h"
#include "Scene.h"


std::unordered_map<cpu_texture*, uint16_t> UIWrapper::m_mapTextureUsage;


UIWrapper::UIWrapper(EntityType entityType)
{
	m_pTexture = TextureRegister::GetTexture(entityType);

	m_pSprite = cpuEngine.CreateSprite();
	m_pSprite->pTexture = m_pTexture;
	m_pSprite->CenterAnchor();

	auto it = m_mapTextureUsage.find(m_pTexture);
	if (it == m_mapTextureUsage.end())
		m_mapTextureUsage[m_pTexture] = 1;
	else
		it->second++;

	switch (entityType) {
	case EntityType::TITLE:
		UIposition(0.25f, 0.5f);
		break;
	case EntityType::ARROW:
		UIposition(0.6125f, 0.8975f);
		break;
	case EntityType::ARROW_DOWN:
		UIposition(0.89f, 0.4925f);
		break;
	case EntityType::ENGINE_DIRECTION:
		UIposition(.95f, 0.5f);
		break;
	case EntityType::ENGINE_POWER:
		UIposition(0.5f, .95f);
		break;
	case EntityType::HEALTH6:
	case EntityType::HEALTH5:
	case EntityType::HEALTH4:
	case EntityType::HEALTH3:
	case EntityType::HEALTH2:
	case EntityType::HEALTH1:
	case EntityType::HEALTH0:
		UIposition(0.1f, 0.1f);
		break;
	case EntityType::BULLET:
	case EntityType::BULLET_EMPTY:
		UIposition(0.95f, 0.7f);
		break;
	case EntityType::CROSSHAIR:
		UIposition(0.45f, 0.5f);
		break;
	}
}

UIWrapper::~UIWrapper()
{
	auto it = m_mapTextureUsage.find(m_pTexture);
	it->second--;

	if (it->second == 0)
	{
		m_mapTextureUsage.erase(m_pTexture);
		CPU_DELPTR(m_pTexture);
	}
}

void UIWrapper::UIposition(float h, float w)
{
	m_pSprite->x = cpuDevice.GetWidth() * w;
	m_pSprite->y = cpuDevice.GetHeight() * h;
}

void UIWrapper::UIpositionPixels(float h, float w)
{
	m_pSprite->x = w;
	m_pSprite->y = h;
}

void UIWrapper::SetHeight(UIWrapper* ui, float h) {
	if (m_mapTextureUsage.size() > 0) {
		ui->m_pSprite->pTexture->height = h;
	}
}

void UIWrapper::SetActive(bool active)
{
	m_pSprite->visible = active;
}