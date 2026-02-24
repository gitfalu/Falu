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

#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Scene/MeshRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"
#include "Falu/Engine.h"

namespace Falu
{
	ImGuiManager::ImGuiManager()
		:m_initialized(false)
		,m_selectedObject(nullptr)
		,m_selectedLight(nullptr)
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

		ImGui::Text("Scene Objects:");
		ImGui::Separator();

		// シーンからオブジェクトを取得
		Scene* scene = Engine::GetInstance().GetSceneManager()->GetCurrentScene();
		if (!scene)
		{
			ImGui::Text("No Scene loaded");
			ImGui::End();
			return;
		}

		const auto& gameobjects = scene->GetGameObject();

		if (ImGui::TreeNode("Root"))
		{
			for (auto& obj : gameobjects)
			{
				if (ImGui::Selectable(obj->GetName().c_str(), m_selectedObject == obj.get()))
				{
					m_selectedObject = obj.get();
					m_selectedLight = nullptr;
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
					{
						scene->DestroyGameObject(obj.get());
					}
					/*
					if (ImGui::MenuItem("Deplicate"))
					{
						scene->CreateGameObject
					}
					*/
					ImGui::EndPopup();
				}
			}
			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::Text("Lights:");

		auto& lights = LightManager::GetInstace().GetLights();
		for (size_t i = 0; i < lights.size(); ++i)
		{
			char label[64];
			sprintf_s(label, "Light %zu", i);

			if (ImGui::Selectable(label, m_selectedLight == lights[i].get()))
			{
				m_selectedLight = lights[i].get();
				m_selectedObject = nullptr;
			}
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

		if (m_selectedObject)
		{
			ImGui::Text("Selected: %s", m_selectedObject->GetName().c_str());
			ImGui::Separator();

			// Edit Transform
			ShowTransformEditor(open, &m_selectedObject->GetTransform(), "Transform");
		}
		else if (m_selectedLight)
		{
			ImGui::Text("Selected Light");
			ImGui::Separator();
			ShowLightEditor(open, m_selectedLight);
		}
		else
		{
			ImGui::Text("No object selected");
			ImGui::Text("Click on an object in Scene Hierarchy");
		}

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

	void ImGuiManager::ShowTransformEditor(bool* open, Transform* transform, const char* name)
	{
		if (!transform)
			return;

		if (!ImGui::Begin(name, open))
		{
			ImGui::End();
			return;
		}

		// Position
		Math::Vector3 pos = transform->GetPosition();
		float position[3] = { pos.x,pos.y,pos.z };
		if (ImGui::DragFloat3("Position", position, 0.1f))
		{
			transform->SetPosition(
				position[0],
				position[1],
				position[2]
			);
		}

		// Rotation(Degree)
		Math::Vector3 rot = transform->GetPosition();
		float rotation[3] = {
			Math::ToDegrees(rot.x),
			Math::ToDegrees(rot.y),
			Math::ToDegrees(rot.z)
		};
		if (ImGui::DragFloat3("Rotation", rotation, 1.0f))
		{
			transform->SetRotation(
				Math::ToDegrees(rotation[0]),
				Math::ToDegrees(rotation[1]),
				Math::ToDegrees(rotation[2])
			);
		}

		// Scale
		Math::Vector3 scl = transform->GetScale();
		float scale[3] = { scl.x,scl.y, scl.z };
		if (ImGui::DragFloat3("Scale", scale, 0.1f, 0.01f, 100.0f))
		{
			transform->SetScale(scale[0], scale[1], scale[2]);
		}

		// Reset Button
		if (ImGui::Button("Reset Postion"))
			transform->SetPosition(0, 0, 0);

		ImGui::SameLine();

		if (ImGui::Button("Reset Rotation"))
			transform->SetRotation(0, 0, 0);

		ImGui::SameLine();

		if (ImGui::Button("Reset Scale"))
			transform->SetScale(1, 1, 1);

		ImGui::End();
	}
	void ImGuiManager::ShowLightEditor(bool* open, Light* light)
	{
		if (!light)
			return;

		if (!ImGui::Begin("Light Editor", open))
		{
			ImGui::End();
			return;
		}

		// 有効/無効
		bool enabled = light->IsEnabled();
		if (ImGui::Checkbox("Enable", &enabled))
		{
			light->SetEnabled(enabled);
		}

		// ライトタイプ
		const char* lightTypes[] = { "Directional", "Point","Spot" };
		int currentType = (int)light->GetType();
		if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes)))
		{
			light->SetType((LightType)currentType);
		}

		// 色
		Math::Color color = light->GetColor();
		float col[4] = { color.r,color.g, color.b,color.a, };
		if (ImGui::ColorEdit4("Color", col))
		{
			light->SetColor(Math::Color(col[0], col[1], col[2], col[3]));
		}

		// 強度
		float intensity = light->GetIntensity();
		if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
		{
			light->SetIntensity(intensity);
		}

		// 範囲(Point/Spotライトの場合)
		if (light->GetType() != LightType::Directional)
		{
			float range = light->GetRange();
			if (ImGui::DragFloat("Range", &range, 0.1f, 0.1f, 100.0f))
			{
				light->SetRange(range);
			}
		}

		// スポットライト角度
		if (light->GetType() == LightType::Spot)
		{
			float angle = light->GetSpotAngle();
			if (ImGui::SliderFloat("Spot Angle", &angle, 1.0f, 90.0f))
			{
				light->SetSpotAngle(angle);
			}
		}

		ImGui::Separator();

		// Transform
		ImGui::Text("Transform");
		ShowTransformEditor(open, &light->GetTransform(), "Light Transform");

		ImGui::End();
	}

	void ImGuiManager::ShowMaterialEditor(bool* open)
	{
		if (!ImGui::Begin("Material Editor", open))
		{
			ImGui::End();
			return;
		}

		if (!m_selectedObject)
		{
			ImGui::Text("No Object selected");
			ImGui::End();
			return;
		}

		// Get MeshRnederer
		auto meshRenderer = m_selectedObject->GetComponent<MeshRenderer>();
		if (!meshRenderer)
		{
			ImGui::Text("No MeshRenderer component");
			ImGui::End();
			return;
		}
		
		Material* material = meshRenderer->GetMaterial();
		if (!material)
		{
			ImGui::Text("No material assigned");
			ImGui::End();
			return;
		}

		MaterialProperties props = material->GetProperties();

		ImGui::Text("Material Propaties");
		ImGui::Separator();

		// Albedo Color
		float albedo[4] = { props.albedo.r,props.albedo.g,props.albedo.b,props.albedo.a };
		if (ImGui::ColorEdit4("Albedo", albedo))
		{
			props.albedo = Math::Color(albedo[0], albedo[1], albedo[2], albedo[3]);
			material->SetProperties(props);
		}

		// Metallic
		if (ImGui::SliderFloat("Metallic", &props.metallic, 0.0f, 1.0f))
		{
			material->SetProperties(props);
		}

		// Roughness
		if (ImGui::SliderFloat("Roughness", &props.roughness, 0.0f, 1.0f))
		{
			material->SetProperties(props);
		}

		// Ambieent Occlusion
		if (ImGui::SliderFloat("AO", &props.ao, 0.0f, 1.0f))
		{
			material->SetProperties(props);
		}

		ImGui::Separator();

		ImGui::Text("Textures");

		// Albedo Texture
		Texture* albedoTex = material->GetAlbedoTexture();
		if (albedoTex)
		{
			ImGui::Text("Albedo Texture: %d%d", albedoTex->GetWidth(), albedoTex->GetHeight());

			// Texture Preview
			ImGui::Image(
				(ImTextureID)albedoTex->GetShaderResourceView(),
				ImVec2(128, 128)
			);

			if (ImGui::Button("Remove Texture"))
			{
				material->SetAlbedoTexture(nullptr);
			}
		}
		else
		{
			ImGui::Text("No albedo texture");

			// Texture Load Button
			if (ImGui::Button("Load Texture"))
			{
				ImGui::OpenPopup("Load Texture");
			}
		}

		// Load Texture Popup
		if (ImGui::BeginPopup("Load Texture"))
		{
			ImGui::Text("Enter texture path:");

			static char texturePath[256] = "Assets/Textures/example.png";
			ImGui::InputText("Path", texturePath, sizeof(texturePath));

			if (ImGui::Button("Load"))
			{
				auto device = Engine::GetInstance().GetRenderer()->GetDevice();

				std::string pathStr(texturePath);
				std::wstring wPath(pathStr.begin(), pathStr.end());

				Texture* tex = TextureManager::GetInstance().LoadTexture(
					device,
					"loaded_texture",
					wPath
				);

				if (tex)
				{
					material->SetAlbedoTexture(tex);
					ImGui::CloseCurrentPopup();
				}
				else
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load texture");
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}
