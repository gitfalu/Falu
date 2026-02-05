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

namespace Falu
{
	class GameObject;
	class Camera;

	/// @brief シーン基底クラス
	class Scene
	{
	public:
		Scene(const std::string& name);
		virtual ~Scene();

		virtual void OnLoad() {}
		virtual void OnUnload() {}
		virtual void UpDate(float deltaTime);
		virtual void Render();

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		void DestroyGameObject(GameObject* gameObject);

		void SetMainCamera(Camera* camera) { m_mainCamera = camera; }
		Camera* GetMainCamera() const { return m_mainCamera; }

		const std::string& GetName() const { return m_name;}
		const std::vector<std::unique_ptr<GameObject>>& GetGameObject() const { return m_gameObjects; }


	protected:
		std::string m_name;
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;
		Camera* m_mainCamera;
	};

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
