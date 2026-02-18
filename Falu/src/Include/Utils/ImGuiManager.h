/*****************************************************************//**
 * \file   ImGuiManager.h
 * \brief  ImGuiManagerの作成
 * 
 * \author tsunn
 * \date   2026/02/18
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <memory>

namespace Falu
{
	class ImGuiManager
	{
	public:
		ImGuiManager();
		~ImGuiManager();

		bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);
		void Shutdown();

		void BeginFrame();
		void EndFrame();
		void Render();

		// デバッグ情報表示用
		void ShowDebugWindow(bool* open, float fps, float deltaTime);
		void ShowSceneHierarchy(bool* open);
		void ShowInspector(bool* open);
		void ShowConsole(bool* open);

	private:
		bool m_initialized;
	};
}
