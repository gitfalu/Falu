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
	class GameObject;
	class Transform;
	class Light;

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

		// 追加機能
		void ShowTransformEditor(bool* open, 
			Transform* transform, const char* name = "Transform");
		void ShowLightEditor(bool* open, Light* light);
		void ShowMaterialEditor(bool* open);

		// 選択オブジェクト管理
		void SetSelectedObject(GameObject* obj) { m_selectedObject = obj; }
		GameObject* GetSelectedObject()const { return m_selectedObject; }

		void SetSelectedLight(Light* light) { m_selectedLight = light; }
		Light* GetSelectedLight() const { return m_selectedLight; }

	private:
		bool m_initialized;
		GameObject* m_selectedObject;
		Light* m_selectedLight;
	};
}
