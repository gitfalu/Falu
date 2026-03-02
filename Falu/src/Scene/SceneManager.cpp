/*****************************************************************//**
 * \file   SceneManager.cpp
 * \brief  シーンマネージャー実装
 * 
 * \author tsunn
 * \date   2026/02/08
 *********************************************************************/
#include "SceneManager.h"
#include "GameObject.h"
#include "Renderer/Camera.h"

namespace Falu
{
	//*****************************************************************
	// 
	// Scene
	// 
	//*****************************************************************


	Scene::Scene(const std::string& name)
		: m_name(name)
		, m_mainCamera(nullptr)
	{

	}

	Scene::~Scene()
	{
		m_gameObjects.clear();
	}

	void Scene::Update(float deltaTime)
	{
		for (auto& gameObject : m_gameObjects)
		{
			if (gameObject && gameObject->IsActive())
			{
				gameObject->Update(deltaTime);
			}
		}
	}

	void Scene::Render()
	{
		for (auto& gameObject : m_gameObjects)
		{
			if (gameObject && gameObject->IsActive())
			{
				gameObject->Render();
			}
		}
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(name);
		GameObject* ptr = gameObject.get();

		m_gameObjects.push_back(std::move(gameObject));
		return ptr;
	}

	void Scene::DestroyGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		m_gameObjects.erase(
			std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
				[gameObject](const std::unique_ptr<GameObject>& obj) {
					return obj.get() == gameObject;
				}),
			m_gameObjects.end()
		);
	}

	GameObject* Scene::FindGameObjectByName(const std::string& name)
	{
		for (const auto& obj : m_gameObjects)
		{
			if (obj->GetName() == name)
			{
				return obj.get();
			}
		}
		return nullptr;
	}

	GameObject* Scene::FindGameObjectByID(int id)
	{
		for (const auto& obj : m_gameObjects)
		{
			if (obj->GetID() == id)
			{
				return obj.get();
			}
		}
		return nullptr;
	}

	std::vector<GameObject*> Scene::FinGameObjectByTag(const std::string& tag)
	{
		std::vector<GameObject*> results;

		for (const auto& obj : m_gameObjects)
		{
			if (obj->GetTag() == tag)
			{
				results.push_back(obj.get());
			}
		}

		return results;
	}

	GameObject* Scene::RayCast(const Math::Ray& ray, float maxDistance)
	{
		GameObject* hitObject = nullptr;
		float closestDistancce = maxDistance;

		for (const auto& obj : m_gameObjects)
		{
			if (!obj->IsActive()) continue;

			float distance;
			if (obj->RayCastHit(ray, distance))
			{
				if (distance < closestDistancce)
				{
					closestDistancce = distance;
					hitObject = obj.get();
				}
			}
		}

		return hitObject;
	}

	std::vector<GameObject*> Scene::RaycastAll(const Math::Ray& ray, float maxDistance)
	{
		std::vector<GameObject*> hitObjects;

		for (const auto& obj : m_gameObjects)
		{
			if (!obj->IsActive()) continue;

			float distance;
			if (obj->RayCastHit(ray, distance) && distance <= maxDistance)
			{
				hitObjects.push_back(obj.get());
			}
		}

		// Sort for near
		std::sort(hitObjects.begin(), hitObjects.end(),
			[&ray](GameObject* a, GameObject* b) {
				float distA, distB;
				a->RayCastHit(ray, distA);
				b->RayCastHit(ray, distB);
				return distA < distB;
			});

		return hitObjects;
	}
	
	//****************************************************************
	// 
	// SceneManager
	// 
	//****************************************************************

	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{
		UnloadCurrentScene();
	}

	void SceneManager::Update(float deltaTime)
	{
		if (m_currentScene)
		{
			m_currentScene->Update(deltaTime);
		}
	}

	void SceneManager::Render()
	{
		if (m_currentScene)
		{
			m_currentScene->Render();
		}
	}

	void SceneManager::LoadScene(std::unique_ptr<Scene> scene)
	{
		// 現在のシーンをアンロード
		UnloadCurrentScene();

		// 新しいシーンをロード
		m_currentScene = std::move(scene);
		if (m_currentScene)
		{
			m_currentScene->OnLoad();
		}
	}

	void SceneManager::UnloadCurrentScene()
	{
		if (m_currentScene)
		{
			m_currentScene->OnUnload();
			m_currentScene.reset();
		}
	}
}
