/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  エンジンクラス実装
 * 
 * \author tsunn
 * \date   2026/02/05
 *********************************************************************/
#include "Engine.h"

#include "Window.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "../Renderer/Renderer.h"
#include "../Scene/SceneManager.h"

#include "imgui.h"

namespace Falu
{
	Engine& Engine::GetInstance()
	{
		static Engine instance;
		return instance;
	}

	Engine::Engine()
		: m_isRunning(false)
	{

	}

	Engine::~Engine()
	{
		Shutdown();
	}

	bool Engine::Initialize(HINSTANCE hInstance, const std::wstring& title, int width, int height)
	{
		//ウィンドウの作成
		m_window = std::make_unique<Window>();
		if (!m_window->Create(hInstance, title, width, height))
		{
			return false;
		}

		//レンダラーの初期化
		RenderSettings settings;
		settings.enableVSync = true;
		settings.clearColor = Math::Color(0.1f, 0.1f, 0.3f, 1.0f);

		m_renderer = std::make_unique<Renderer>();
		if (!m_renderer->Initialize(m_window->GetHandle(), width, height, settings))
		{
			return false;
		}

		// 入力管理の初期化
		m_inputManager = std::make_unique<InputManager>();

		// シーン管理の初期化
		m_sceneManager = std::make_unique<SceneManager>();

		// 時間管理の初期化
		m_timeManager = std::make_unique<TimeManager>();
		m_timeManager->Initialize();

		// ImGui Initialize
		m_imguiManager = std::make_unique<ImGuiManager>();
		if (!m_imguiManager->Initialize(
			m_window->GetHandle(),
			m_renderer->GetDevice(),
			m_renderer->GetContext()))
		{
			OutputDebugStringW(L"[Engine] ERROR: ImGui initialization failed\n");
			return false;
		}

		m_isRunning = true;
		return true;
	}

	void Engine::Run()
	{
		while (m_isRunning)
		{
			// メッセージ管理
			if (!m_window->ProcessMessage())
			{
				m_isRunning = false;
				break;
			}

			// 時間更新
			m_timeManager->Update();
			float deltaTime = m_timeManager->GetDeltaTime();

			// 入力更新
			m_inputManager->Update();

			// 更新
			Update(deltaTime);

			// 描画
			Render();
		}
	}

	void Engine::Update(float deltaTime)
	{
		// シーンの更新
		if (m_sceneManager)
		{
			m_sceneManager->Update(deltaTime);
		}
	}

	void Engine::Render()
	{
		m_renderer->BeginFrame();

		// シーンのレンダリング
		if (m_sceneManager)
		{
			m_sceneManager->Render();
		}

		// ImGui Render
		m_imguiManager->BeginFrame();

		// Debug Render
		if (m_showDebugWindow)
		{
			m_imguiManager->ShowDebugWindow(&m_showDebugWindow,
				(float)m_timeManager->GetFPS(),
				m_timeManager->GetDeltaTime());
		}

		if (m_showHierarchy)
			m_imguiManager->ShowSceneHierarchy(&m_showHierarchy);

		if (m_showInspector)
			m_imguiManager->ShowInspector(&m_showInspector);

		if (m_showConsole)
			m_imguiManager->ShowConsole(&m_showConsole);

		// メインメニューバー
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Debug Info", nullptr, &m_showDebugWindow);
				ImGui::MenuItem("Scene Hierarchy", nullptr, &m_showHierarchy);
				ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
				ImGui::MenuItem("Console", nullptr, &m_showConsole);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		m_imguiManager->EndFrame();
		m_imguiManager->Render();

		m_renderer->EndFrame();
	}

	void Engine::Shutdown()
	{
		m_imguiManager.reset();
		m_sceneManager.reset();
		m_inputManager.reset();
		m_renderer.reset();
		m_window.reset();
	}
}

