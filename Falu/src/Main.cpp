/*****************************************************************//**
 * \file   Main.cpp
 * \brief  エントリーポイント
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Falu/Engine.h"
#include "Falu/InputManager.h"
#include "Falu/Window.h"
#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/MeshRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"
#include "Renderer/Light.h"
#include "Renderer/Texture.h"

using namespace Falu;

/// @brief サンプルシーン
class SampleScene : public Scene
{
public:
	SampleScene():Scene("SampleScene"){}

	void OnLoad()override
	{
		auto device = Engine::GetInstance().GetRenderer()->GetDevice();
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

		// ライトの作成
		Light* mainLight = LightManager::GetInstace().CreateLight(LightType::Directional);
		mainLight->SetColor(Math::Color(1.0f, 1.0f, 1.0f, 1.0f));
		mainLight->SetIntensity(1.0f);
		mainLight->GetTransform().SetRotation(
			Math::ToRadians(45.0f),
			Math::ToRadians(-30.0f),
			0.0f
		);

		// layoutの作成
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// シェーダーの読み込み

		m_shader = ShaderManager::GetInstance().LoadShader(
			device,
			"Basic",
			L"Shaders/HLSL/Basic.hlsl",
			L"Shaders/HLSL/Basic.hlsl",
			layout,
			_countof(layout)
		);

		// マテリアルの作成
		m_material = new Material();
		m_material->Initialize(device);
		m_material->SetShader(m_shader);

		MaterialProperties props;
		props.albedo = Math::Color(0.8f, 0.3f, 0.3f, 1.0f);
		props.roughness = 0.5f;
		props.metallic = 0.0f;
		m_material->SetProperties(props);

		Texture* texture = TextureManager::GetInstance().LoadTexture(
			device,
			"test_Texture",
			L"Assets/Textures/test.png"
		);

		if (texture)
		{
			m_material->SetAlbedoTexture(texture);
		}

		m_mesh = Mesh::CreateCube(device).release();

		m_cubeObject = CreateGameObject("Cube");
		auto meshRenderer = m_cubeObject->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(m_mesh);
		meshRenderer->SetMaterial(m_material);
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
		if (m_cubeObject)
		{
			static float rotation = 0.0f;
			rotation += deltaTime * 0.5f;
			m_cubeObject->GetTransform().SetRotation(Math::ToRadians(30.0f),rotation, 0);
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
	GameObject* m_cubeObject = nullptr;
};

// MultiObjectScene
class MultiObjectScene : public Scene
{
public:
	MultiObjectScene() : Scene("MultiObjectScene") {}

	void OnLoad() override
	{
		auto device = Engine::GetInstance().GetRenderer()->GetDevice();

		// Create Camera
		m_camera = new Camera();
		m_camera->GetTransform().SetPosition(0, 3, -10);
		m_camera->LookAt(Math::Vector3(0, 0, 0));
		m_camera->SetPerspective(
			Math::ToRadians(60.0f),
			Engine::GetInstance().GetWindow()->GetAspectRatio(),
			0.1f,
			100.0f
		);
		SetMainCamera(m_camera);
		Engine::GetInstance().GetRenderer()->SetCamera(m_camera);

		// Create Light
		Light* mainLight = LightManager::GetInstace().CreateLight(LightType::Directional);
		mainLight->SetColor(Math::Color(1.0f, 1.0f, 1.0f, 1.0f));
		mainLight->SetIntensity(1.0f);
		mainLight->GetTransform().SetRotation(
			Math::ToRadians(45.0f),
			Math::ToRadians(-30.0f),
			0.0f
		);

		// Create Shader
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",	0,DXGI_FORMAT_R32G32B32_FLOAT,		0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,			0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",	0,DXGI_FORMAT_R32G32B32A32_FLOAT,	0,32,D3D11_INPUT_PER_VERTEX_DATA,0},
		};

		Shader* shader = ShaderManager::GetInstance().LoadShader(
			device,"Basic",
			L"Shaders/HLSL/Basic.hlsl",
			L"Shaders/HLSL/Basic.hlsl",
			layout,ARRAYSIZE(layout)
		);

		// Create Multiple Cube
		CreateCube(device, shader, "RedCube", Math::Vector3(-3, 0, 0), Math::Color(1, 0, 0, 1));
		CreateCube(device, shader, "GreenCube", Math::Vector3(0, 0, 0), Math::Color(0, 1, 0, 1));
		CreateCube(device, shader, "BlueCube", Math::Vector3(3, 0, 0), Math::Color(0, 0, 1, 1));

		// Create Multiple Sphere
		CreateSphere(device, shader, "YellowSphere", Math::Vector3(-3, 2, 0), Math::Color(1,1, 0, 1));
		CreateSphere(device, shader, "CyanSphere", Math::Vector3(0, 2, 0), Math::Color(0,1, 1, 1));
		CreateSphere(device, shader, "MagentaSphere", Math::Vector3(3, 2, 0), Math::Color(1,0, 1, 1));

		// Create Ground
		CreatePlane(device, shader, "Ground", Math::Vector3(0, -1, 0), Math::Color(0.3f, 0.3f, 0.3f, 1));

		OutputDebugStringW(L"[MultipleObjectScene] Loaded with multiple objects\n");
	}

	void Update(float deltaTime) override
	{
		Scene::Update(deltaTime);

		auto input = Engine::GetInstance().GetInputManager();

		// Move Camera
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

		// Object Animation
		static float time = 0.0f;
		time += deltaTime;

		GameObject* redCube = FindGameObjectByName("RedCube");
		if (redCube)
		{
			redCube->GetTransform().SetRotation(0, time, 0);
		}

		GameObject* greenCube = FindGameObjectByName("GreenCube");
		if (greenCube)
		{
			greenCube->GetTransform().SetRotation(time, 0, 0);
		}

		GameObject* blueCube = FindGameObjectByName("BlueCube");
		if (blueCube)
		{
			blueCube->GetTransform().SetRotation(0, 0, time);
		}

		// Float Sphere
		GameObject* yellowSphere = FindGameObjectByName("YellowSphere");
		if (yellowSphere)
		{
			Math::Vector3 pos = yellowSphere->GetTransform().GetPosition();
			pos.y = 2.0f + sinf(time * 2.0f) * 0.5f;
			yellowSphere->GetTransform().SetPosition(pos);
		}

		GameObject* cyanSphere = FindGameObjectByName("CyanSphere");
		if (cyanSphere)
		{
			Math::Vector3 pos = cyanSphere->GetTransform().GetPosition();
			pos.y = 2.0f + sinf(time * 2.0f + 2.0f) * 0.5f;
			cyanSphere->GetTransform().SetPosition(pos);
		}

		GameObject* magentaSphere = FindGameObjectByName("MagentaSphere");
		if (magentaSphere)
		{
			Math::Vector3 pos = magentaSphere->GetTransform().GetPosition();
			pos.y = 2.0f + sinf(time * 2.0f + 4.0f) * 0.5f;
			magentaSphere->GetTransform().SetPosition(pos);
		}
	}

	void OnUnload() override
	{
		delete m_camera;
	}

private:
	void CreateCube(ID3D11Device* device, Shader* shader, const std::string& name,
		const Math::Vector3& position, const Math::Color& color)
	{
		GameObject* obj = CreateGameObject(name);
		obj->SetTag("Cube");
		obj->GetTransform().SetPosition(position);

		auto meshRenderer = obj->AddComponent<MeshRenderer>();
		auto mesh = Mesh::CreateCube(device).release();
		meshRenderer->SetMesh(mesh);

		obj->SetBounds(mesh->CalculateBounds());

		auto material = new Material();
		material->Initialize(device);
		material->SetShader(shader);

		MaterialProperties props;
		props.albedo = color;
		props.roughness = 0.5f;
		material->SetProperties(props);

		meshRenderer->SetMaterial(material);
	}

	void CreateSphere(ID3D11Device* device, Shader* shader, const std::string& name,
		const Math::Vector3& position, const Math::Color& color)
	{
		GameObject* obj = CreateGameObject(name);
		obj->SetTag("Sphere");
		obj->GetTransform().SetPosition(position);

		auto meshRenderer = obj->AddComponent<MeshRenderer>();
		auto mesh = Mesh::CreateSphere(device,32).release();
		meshRenderer->SetMesh(mesh);

		auto material = new Material();
		material->Initialize(device);
		material->SetShader(shader);

		MaterialProperties props;
		props.albedo = color;
		props.roughness = 0.3f;
		material->SetProperties(props);

		meshRenderer->SetMaterial(material);
	}

	void CreatePlane(ID3D11Device* device, Shader* shader, const std::string& name,
		const Math::Vector3& position, const Math::Color& color)
	{
		GameObject* obj = CreateGameObject(name);
		obj->SetTag("Ground");
		obj->GetTransform().SetPosition(position);

		auto meshRenderer = obj->AddComponent<MeshRenderer>();
		auto mesh = Mesh::CreatePlane(device,10.0f,10.0f,10).release();
		meshRenderer->SetMesh(mesh);

		auto material = new Material();
		material->Initialize(device);
		material->SetShader(shader);

		MaterialProperties props;
		props.albedo = color;
		props.roughness = 0.8f;
		material->SetProperties(props);

		meshRenderer->SetMaterial(material);
	}
private:
	Camera* m_camera = nullptr;
};

/// @brief エントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// エンジンの初期化
	if (!Engine::GetInstance().Initialize(hInstance, L"FaluEngine", 1280, 720))
	{
		MessageBox(nullptr, "Failed to initialize engine!", "Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// サンプルシーンのロード
	auto Scene = std::make_unique<MultiObjectScene>();
	Engine::GetInstance().GetSceneManager()->LoadScene(std::move(Scene));

	// メインループの実行
	Engine::GetInstance().Run();

	// 終了処理
	Engine::GetInstance().Shutdown();

	return 0;
}
