/*****************************************************************//**
 * \file   ModelRenderer.cpp
 * \brief  ModelRendererÀ‘•
 * 
 * \author tsunn
 * \date   2026/03/10
 *********************************************************************/
#include "ModelRenderer.h"
#include "GameObject.h"
#include "Renderer/Model.h"
#include "Renderer/ModelLoader.h"
#include "Renderer/Renderer.h"
#include "Falu/Engine.h"


namespace Falu
{
	ModelRenderer::ModelRenderer(GameObject* owner) :
		Component(owner),m_model(nullptr)
	{

	}

	ModelRenderer::~ModelRenderer()
	{

	}

	void ModelRenderer::Update(float deltaTime)
	{

	}

	void ModelRenderer::Render()
	{
		if (!m_model) return;

		auto renderer = Engine::GetInstance().GetRenderer();
		if (!renderer) return;

		DirectX::XMMATRIX worldMatrix = GetOwner()->GetTransform().GetWorldMatrix();

		for (const auto& subMesh : m_model->GetSubMeshes())
		{
			if (subMesh.mesh && subMesh.material) {
				renderer->RenderMesh(subMesh.mesh.get(),
					subMesh.material.get(),
					worldMatrix);
			}
		}
	}

	bool ModelRenderer::LoadModel(const std::string& filepath)
	{
		// Get Device

		auto device = Engine::GetInstance().GetRenderer()->GetDevice();
		auto loadedModel = ModelLoader::GetInstance().LoadModel(device, filepath);

		if (loadedModel)
		{
			m_model = std::move(loadedModel);
			m_modelPath = filepath;

			// setting bounding box
			GetOwner()->SetBounds(m_model->GetBounds());

			return true;
		}

		return false;
	}

}

