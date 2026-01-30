#pragma once
#include "Scene.h"
#include <vector>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Update();
	void OnRender(int pass);

	void Reset();

	template<typename SceneType>
	Scene* GetScene() 
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
			{
				return scene;
			}
		}
	}

	template<typename SceneType>
	void SetSceneActive(bool active)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{	

			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
			{
				scene->SetActive(active);
				return;
			}
		}
	}

	template<typename SceneType>
	void AddEntityToScene(MultiEntity* entity)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
			{
				scene->AddEntity(entity);
				return;
			}
		}
	}

	std::vector<MultiEntity*> GetMultiEntitiesOfTag(EntityTag tag);
	std::vector<MultiEntity*> GetMultiEntitiesOfTags(std::vector<EntityTag> tags, bool logicAnd = true);
	MultiEntity* GetFirstMultiEntityOfTag(EntityTag tag);
	MultiEntity* GetFirstMultiEntityOfTags(std::vector<EntityTag> tags, bool logicAnd = true);

	template<typename SceneType>
	void HandleNetworkEntity(NetworkEntityData& entityData)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
				scene->HandleNetworkEntity(entityData);
		}
	}

	template<typename SceneType>
	void HandleGiveEntityNetworkId(PayloadGiveEntityNetworkId& payload)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
				scene->HandleGiveEntityNetworkId(payload);
		}
	}

	template<typename SceneType>
	void HandleCollide(PayloadCollide& payload)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
				scene->HandleCollide(payload);
		}
	}

	template<typename SceneType>
	void HandleGameTimer(PayloadGameTimer& payload)
	{
		for (size_t i = 0; i < m_scenes.size(); i++)
		{
			Scene* scene = m_scenes[i];
			if (dynamic_cast<SceneType*>(scene) != nullptr)
				scene->HandleGameTimer(payload);
		}
	}

	
private:
	void Destroy();
	void Init();

private:
	std::vector<Scene*> m_scenes;
};

