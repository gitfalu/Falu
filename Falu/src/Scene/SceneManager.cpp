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
