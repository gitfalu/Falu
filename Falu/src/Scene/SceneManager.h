/*****************************************************************//**
 * \file   SceneManager.h
 * \brief  シーンマネージャー
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Scene/GameObject.h"
#include "Include/Math/Ray.h"

namespace Falu
{
	class Camera;

	/// @brief シーン基底クラス
	class Scene
	{
	public:
		Scene(const std::string& name);
		virtual ~Scene();

		virtual void OnLoad() {}
		virtual void OnUnload() {}
		virtual void Update(float deltaTime);
		virtual void Render();

		//=== Management GameObject ===
		GameObject* CreateGameObject(const std::string& name = "GameObject");
		void DestroyGameObject(GameObject* gameObject);

		//=== Find System ===
		GameObject* FindGameObjectByName(const std::string& name);
		GameObject* FindGameObjectByID(int id);
		std::vector<GameObject*> FinGameObjectByTag(const std::string& tag);

		template<typename T>
		std::vector<GameObject*> FindGameObjectWithComponent();

		//=== Management Camera ===
		void SetMainCamera(Camera* camera) { m_mainCamera = camera; }
		Camera* GetMainCamera() const { return m_mainCamera; }

		//=== Ray Cast ===
		GameObject* RayCast(const Math::Ray& ray, float maxDistance = 1000.0f);
		std::vector<GameObject*> RaycastAll(const Math::Ray& ray, float maxDistance = 1000.0f);

		//=== Getter ===
		const std::string& GetName() const { return m_name; }
		const std::vector<std::unique_ptr<GameObject>>& GetGameObject() const { return m_gameObjects; }


	protected:
		std::string m_name;
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;
		Camera* m_mainCamera;
	};
	//=== Implimentation Template ===
	template<typename T>
	inline std::vector<GameObject*> Scene::FindGameObjectWithComponent()
	{
		std::vector<GameObject*> results;

		for (const auto& obj : m_gameObjects)
		{
			if (obj->GetComponent<T>() != nullptr)
			{// if obj have component
				results.push_back(obj.get());// push_back results
			}
		}

		return results;
	}

	/// @brief シーンマネージャークラス
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void Update(float deltaTime);
		void Render();

		void LoadScene(std::unique_ptr<Scene> scene);
		void UnloadCurrentScene();

		Scene* GetCurrentScene() const { return m_currentScene.get(); }

	private:
		std::unique_ptr<Scene> m_currentScene;

	};
}
