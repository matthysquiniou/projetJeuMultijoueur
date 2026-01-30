#include "SceneManager.h"
#include "SceneGameplay.h"
#include "SceneConnexion.h"
#include "SceneName.h"
#include "UIWrapper.h"


SceneManager::SceneManager()
{
	Init();
}

SceneManager::~SceneManager()
{
	Destroy();
}

void SceneManager::Update()
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		if (scene->IsActive())
			scene->Update();
	}
}

void SceneManager::OnRender(int pass) 
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		if (scene->IsActive())
			scene->OnRender(pass);
	}
}

void SceneManager::Reset()
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		scene->Reset();
	}
}

std::vector<MultiEntity*> SceneManager::GetMultiEntitiesOfTag(EntityTag tag)
{
	std::vector<MultiEntity*> entities;
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		std::vector<MultiEntity*> sceneEntities = scene->GetMultiEntitiesOfTag(tag);
		entities.insert(entities.end(), sceneEntities.begin(), sceneEntities.end());
	}
	return entities;
}

std::vector<MultiEntity*> SceneManager::GetMultiEntitiesOfTags(std::vector<EntityTag> tags, bool logicAnd)
{
	std::vector<MultiEntity*> entities;
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		std::vector<MultiEntity*> sceneEntities = scene->GetMultiEntitiesOfTags(tags, logicAnd);
		entities.insert(entities.end(), sceneEntities.begin(), sceneEntities.end());
	}
	return entities;
}

MultiEntity* SceneManager::GetFirstMultiEntityOfTag(EntityTag tag)
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		MultiEntity* entity = scene->GetFirstMultiEntityOfTag(tag);
		if (entity != nullptr)
			return entity;
	}
	return nullptr;
}

MultiEntity* SceneManager::GetFirstMultiEntityOfTags(std::vector<EntityTag> tags, bool logicAnd)
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		MultiEntity* entity = scene->GetFirstMultiEntityOfTags(tags, logicAnd);
		if (entity != nullptr)
			return entity;
	}
	return nullptr;
}

void SceneManager::Destroy()
{
	for (size_t i = 0; i < m_scenes.size(); i++)
	{
		Scene* scene = m_scenes[i];
		CPU_DELPTR(scene);
	}

	m_scenes.clear();
}

void SceneManager::Init()
{
	Scene* sceneConnexion = new SceneConnexion();
	Scene* sceneName = new SceneName();
	Scene* sceneGameplay = new SceneGameplay();

	m_scenes.push_back(sceneConnexion);
	m_scenes.push_back(sceneName);
	m_scenes.push_back(sceneGameplay);
}

