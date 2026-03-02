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
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"

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

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		// Outline Initialize
		ID3D11Device* device = m_renderer->GetDevice();
		Shader* outlineShader = ShaderManager::GetInstance().LoadShader(
			device,
			"Outline",
			L"Shaders/HLSL/Outline.hlsl",
			L"Shaders/HLSL/Outline.hlsl",
			layout,
			ARRAYSIZE(layout)
		);
		if (outlineShader)
		{
			m_renderer->SetOutlineShader(outlineShader);
		}

		// Gizmo Initialize
		// Gizmo Shader
		
		Shader* gizmoShader = ShaderManager::GetInstance().LoadShader(
			device,
			"Gizmo",
			L"Shaders/HLSL/Gizmo.hlsl",
			L"Shaders/HLSL/Gizmo.hlsl",
			layout,
			ARRAYSIZE(layout)
		);

		m_gizmo = std::make_unique<Gizmo>();
		if (!m_gizmo->Initialize(m_renderer->GetDevice()))
		{
			OutputDebugStringW(L"[Engine] WARNING : Gizmo initialization failed\n");
		}
		m_gizmo->SetShader(gizmoShader);
		m_showGizmo = true;

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

		//Gizmo Control
		HandleGizmoInput();

		// MouseInput
		HandleMousePicking();
	}

	void Engine::Render()
	{
		m_renderer->BeginFrame();

		// シーンのレンダリング
		if (m_sceneManager)
		{
			m_sceneManager->Render();
		}

		// Selected Object Outline
		if (m_imguiManager)
		{
			GameObject* selectedObject = m_imguiManager->GetSelectedObject();
			if (selectedObject)
			{
				m_renderer->RenderOutline(selectedObject, Math::Color(1, 1, 0, 1), 0.02f);
			}
		}

		// Gizmo Render
		if (m_showGizmo && m_gizmo && m_imguiManager)
		{
			GameObject* selectedObject = m_imguiManager->GetSelectedObject();
			if (selectedObject)
			{
				auto scene = m_sceneManager->GetCurrentScene();
				if (scene)
				{
					Camera* camera = scene->GetMainCamera();
					m_gizmo->Render(m_renderer->GetContext(), camera, selectedObject);
				}
			}
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

		static bool showMaterialEditor = true;
		if (showMaterialEditor)
			m_imguiManager->ShowMaterialEditor(&showMaterialEditor);

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

	void Engine::HandleMousePicking()
	{
		auto input = GetInputManager();
		if (!input) return;

		// Left Click
		if (input->IsMouseButtonPressed(MouseButton::Left))
		{
			// Get Mouse position
			MouseState mouseState = input->GetMouseState();
			int mouseX = mouseState.x;
			int mouseY = mouseState.y;

			// Get window size
			int screenWidth = m_window->GetWidth();
			int screenHeight = m_window->GetHeight();

			// GEt Camera
			auto scene = m_sceneManager->GetCurrentScene();
			if (!scene) return;

			Camera* camera = scene->GetMainCamera();
			if (!camera) return;

			// Create Ray from screen coordinate
			Math::Ray ray = camera->ScreenPointToRay(
				(float)mouseX,
				(float)mouseY,
				(float)screenWidth,
				(float)screenHeight
			);

			// Ray Cast
			GameObject* hitObject = scene->RayCast(ray);

			if (hitObject)
			{
				char msg[256];
				sprintf_s(msg, "[Engine] Clicked: %s (ID:%d)\n",
					hitObject->GetName(),
					hitObject->GetID()
					);
				OutputDebugStringA(msg);

				// Notify ImguiManager
				if (m_imguiManager)
				{
					m_imguiManager->SetSelectedObject(hitObject);
				}
			}
			else
			{
				OutputDebugStringA("[Engine] Click missed\n");

				if (m_imguiManager)
				{
					m_imguiManager->SetSelectedObject(nullptr);
				}
			}
		}
	}

	void Engine::HandleGizmoInput()
	{
		if (!m_gizmo || !m_imguiManager)
			return;

		auto input = GetInputManager();
		if (!input)
			return;

		GameObject* selectedObject = m_imguiManager->GetSelectedObject();
		if (!selectedObject)
			return;

		if (input->IsKeyPressed(KeyCode::W))
		{
			m_gizmo->SetMode(GizmoMode::Translate);
			OutputDebugStringW(L"[Gizmo] Mode: Translate\n");
		}
		if (input->IsKeyPressed(KeyCode::E))
		{
			m_gizmo->SetMode(GizmoMode::Rotate);
			OutputDebugStringW(L"[Gizmo] Mode: Rotate\n");
		}
		if (input->IsKeyPressed(KeyCode::R))
		{
			m_gizmo->SetMode(GizmoMode::Scale);
			OutputDebugStringW(L"[Gizmo] Mode: Scale\n");
		}

		// Change coordinate System
		if (input->IsKeyPressed(KeyCode::Space))
		{
			if (m_gizmo->GetSpace() == GizmoSpace::World)
			{
				m_gizmo->SetSpace(GizmoSpace::Local);
				OutputDebugStringW(L"[Gizmo] Space: Local");
			}
			else
			{
				m_gizmo->SetSpace(GizmoSpace::World);
				OutputDebugStringW(L"[Gizmo] Space: World");
			}
		}

		// Mouse Control
		MouseState mouseState = input->GetMouseState();
		auto scene = m_sceneManager->GetCurrentScene();
		if (!scene)
			return;

		Camera* camera = scene->GetMainCamera();
		if (!camera)
			return;

		Math::Ray ray = camera->ScreenPointToRay(
			(float)mouseState.x,
			(float)mouseState.y,
			(float)m_window->GetWidth(),
			(float)m_window->GetHeight()
		);

		if (input->IsMouseButtonPressed(MouseButton::Left))
		{
			GizmoAxis hoveredAxis = m_gizmo->GetHoveredAxis(ray, selectedObject,camera);
			if (hoveredAxis != GizmoAxis::None)
			{
				m_gizmo->OnMouseDown(ray, selectedObject,camera);
			}
		}
		else if (input->IsMouseButtonHeld(MouseButton::Left))
		{
			m_gizmo->OnMouseDrag(ray, selectedObject,camera);
		}
		else if (input->IsMouseButtonReleased(MouseButton::Left))
		{
			m_gizmo->OnMouseUp();
		}

	}
}

