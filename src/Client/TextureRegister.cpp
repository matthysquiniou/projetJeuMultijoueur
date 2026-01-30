#include "TextureRegister.h"

std::unordered_map<EntityType, cpu_texture*> TextureRegister::m_mapTexture;

bool TextureRegister::HasTexture(EntityType entityType)
{
    return m_mapTexture.contains(entityType);
}

cpu_texture* TextureRegister::GetTexture(EntityType entityType)
{
    auto it = m_mapTexture.find(entityType);
    if (it != m_mapTexture.end())
        return it->second;
    else
        return RegisterTexture(entityType);
}

cpu_texture* TextureRegister::RegisterTexture(EntityType entityType)
{
    auto [it, _] = m_mapTexture.emplace(entityType, TextureFactory::Create(entityType));
    return it->second;
}