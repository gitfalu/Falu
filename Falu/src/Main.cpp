/*****************************************************************//**
 * \file   Main.cpp
 * \brief  エントリーポイント
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Falu/Engine.h"
#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/MeshRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"
#include "Falu/InputManager.h"

using namespace Falu;

/// @brief サンプルシーン
class SampleScene : public Scene
{
public:
	SampleScene():Scene("SampleScene"){}

	void OnLoad()override
	{
		// カメラの作成
		auto cameraObject = CreateGameObject("MainCamera");
		m_camera = new Camera();
		m_camera->GetTransform().SetPosition(0,0,-5);
		m_camera->SetPerspective(
			Math::ToRadians(60.0f),
			16.0f / 9.0f,
			0.1f,
			100.0f
		);
		SetMainCamera(m_camera);

		// レンダラーにカメラを設定
		Engine::GetInstance().GetRenderer()->SetCamera(m_camera);

		// シェーダーの読み込み
		auto device = Engine::GetInstance().GetRenderer()->GetDevice();

		m_shader = ShaderManager::GetInstance().LoadShader(
			device,
			"Basic",
			L"Shader/Basic.hlsl",
			L"Shader/Basic.hlsl",
			nullptr,
			0
		);

		// マテリアルの作成
		m_material = new Material();
		m_material->Initialize(device);
		m_material->SetShader(m_shader);

		MaterialProperties props;
		props.albedo = Math::Color(0.8f, 0.3f, 0.3f, 1.0f);
		m_material->SetProperties(props);

		// 回転する三角形の作成
		auto triangleObject = CreateGameObject("Triangle");
		auto meshRenderer = triangleObject->AddComponent<MeshRenderer>();
		m_mesh = Mesh::CreateTriangle(device).release();
		meshRenderer->SetMesh(m_mesh);
		meshRenderer->SetMaterial(m_material);

		m_triangleObject = triangleObject;
	}

	void Update(float deltaTime)override
	{
		Scene::Update(deltaTime);

		// 入力処理
		auto input = Engine::GetInstance().GetInputManager();

		// カメラの移動
		float moveSpeed = 5.0f * deltaTime;
		if (input->IsKeyHeld(KeyCode::W))
			m_camera->MoveForward(moveSpeed);
		if (input->IsKeyHeld(KeyCode::S))
			m_camera->MoveForward(-moveSpeed);
		if (input->IsKeyHeld(KeyCode::A))
			m_camera->MoveRight(-moveSpeed);
		if (input->IsKeyHeld(KeyCode::D))
			m_camera->MoveRight(moveSpeed);
		if (input->IsKeyHeld(KeyCode::Q))
			m_camera->MoveUp(moveSpeed);
		if (input->IsKeyHeld(KeyCode::E))
			m_camera->MoveUp(-moveSpeed);

		// 三角形を回転
		if (m_triangleObject)
		{
			static float rotation = 0.0f;
			rotation += deltaTime;
			m_triangleObject->GetTransform().SetRotation(0, rotation, 0);
		}
	}

	void OnUnload()override
	{
		delete m_mesh;
		delete m_material;
		delete m_camera;
	}

private:
	Camera* m_camera = nullptr;
	Shader* m_shader = nullptr;
	Material* m_material = nullptr;
	Mesh* m_mesh = nullptr;
	GameObject* m_triangleObject = nullptr;
};


/// @brief エントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// エンジンの初期化
	if (!Engine::GetInstance().Initialize(hInstance, L"Falu Game Engine", 1280, 720))
	{
		MessageBox(nullptr, L"Failed to initialize engine!", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// サンプルシーンのロード
	auto Scene = std::make_unique<SampleScene>();
	Engine::GetInstance().GetSceneManager()->LoadScene(std::move(Scene));

	// メインループの実行
	Engine::GetInstance().Run();

	// 終了処理
	Engine::GetInstance().Shutdown();

	return 0;
}
