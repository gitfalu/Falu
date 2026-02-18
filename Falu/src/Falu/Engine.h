/*****************************************************************//**
 * \file   Window.h
 * \brief  ÉGÉìÉWÉìçÏê¨/ä«óù
 *
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <Windows.h>
#include <memory>
#include <string>
#include "Include/Utils/ImGuiManager.h"

namespace Falu
{
	class Renderer;
	class Window;
	class InputManager;
	class SceneManager;
	class TimeManager;

	class Engine
	{
	public:
		static Engine& GetInstance();

		bool Initialize(HINSTANCE hInstance, const std::wstring& title, int width, int height);
		void Run();
		void Shutdown();

		Renderer* GetRenderer() const { return m_renderer.get(); }
		Window* GetWindow() const { return m_window.get(); }
		InputManager* GetInputManager() const { return m_inputManager.get(); }
		SceneManager* GetSceneManager() const { return m_sceneManager.get(); }
		TimeManager* GetTimaManager() const { return m_timeManager.get(); }

		bool IsRunning() const { return m_isRunning; }
		void Quit() { m_isRunning = false; }

	private:
		Engine();
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Update(float deltaTime);
		void Render();

	private:
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		std::unique_ptr<InputManager> m_inputManager;
		std::unique_ptr<SceneManager> m_sceneManager;
		std::unique_ptr<TimeManager> m_timeManager;

		bool m_isRunning;

		// ImGui menber
		std::unique_ptr<ImGuiManager> m_imguiManager;

		// Debug Window flags
		bool m_showDebugWindow = true;
		bool m_showHierarchy = true;
		bool m_showInspector = true;
		bool m_showConsole = true;
	};
}
