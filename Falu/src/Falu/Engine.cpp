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

		m_renderer->EndFrame();
	}

	void Engine::Shutdown()
	{
		m_sceneManager.reset();
		m_inputManager.reset();
		m_renderer.reset();
		m_window.reset();
	}
}

