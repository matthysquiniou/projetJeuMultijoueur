#pragma once
#include "app.h"
#include <cpu-render.h>
#include "Player.h"

class UIWrapper
{
public:

	UIWrapper(EntityType entityType);
	~UIWrapper();

	void UIposition(float h, float w);
	void UIpositionPixels(float h, float w);
	void SetActive(bool active);
	void SetHeight(UIWrapper* ui, float h);

	cpu_sprite* m_pSprite;
	cpu_texture* m_pTexture;

private:
	static std::unordered_map<cpu_texture*, uint16_t> m_mapTextureUsage;
};
