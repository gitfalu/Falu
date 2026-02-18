/*****************************************************************//**
 * \file   ImGuiManager.cpp
 * \brief  ImGuiManager実装
 * 
 * \author tsunn
 * \date   2026/02/18
 *********************************************************************/
#include "ImGuiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Falu
{
	ImGuiManager::ImGuiManager()
		:m_initialized(false)
	{
	}

	ImGuiManager::~ImGuiManager()
	{
		Shutdown();
	}

	bool ImGuiManager::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		// ImGuiコンテキストの作成
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// 設定
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// スタイル設定
		ImGui::StyleColorsDark();

		// バックエンドの初期化
		if (!ImGui_ImplWin32_Init(hWnd))
			return false;

		if (!ImGui_ImplDX11_Init(device, context))
			return false;

		m_initialized = true;
		return true;
	}

	void ImGuiManager::Shutdown()
	{
		if (m_initialized)
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			m_initialized = false;
		}
	}

	void ImGuiManager::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiManager::EndFrame()
	{
		ImGui::Render();
	}

	void ImGuiManager::Render()
	{
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiManager::ShowDebugWindow(bool* open, float fps, float deltaTime)
	{
		if (!ImGui::Begin("Debug Info", open))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("FPS: %.1f", fps);
		ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
		ImGui::Separator();

		ImGui::Text("Application avarage %.3f ms/frame (%.1f FPS)",
			1000.0f / fps, fps);
		ImGui::End();
	}

	void ImGuiManager::ShowSceneHierarchy(bool* open)
	{
		if (!ImGui::Begin("Scene Hierarchy", open))
		{
			ImGui::End();
			return;
		}

		// TODO: シーン階層の表示
		ImGui::Text("Scene Objects:");
		if (ImGui::TreeNode("Root"))
		{
			ImGui::TreePop();
		}

		ImGui::End();
	}

	void ImGuiManager::ShowInspector(bool* open)
	{
		if (!ImGui::Begin("Inspector", open))
		{
			ImGui::End();
			return;
		}

		// TODO: 選択オブジェクトのプロパティ表示
		ImGui::Text("Select an object to inspect");

		ImGui::End();

	}

	void ImGuiManager::ShowConsole(bool* open)
	{
		if (!ImGui::Begin("Console", open))
		{
			ImGui::End();
			return;
		}

		// TODO: コンソールログの表示
		ImGui::TextWrapped("Console output will apper here");

		ImGui::End();
	}
}
