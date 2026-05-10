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

		DirectX::XMMATRIX worldMatrix = GetOwner()->GetTransform().GetWorldMatrix();


		auto context = Engine::GetInstance().GetRenderer()->GetContext();
		m_model->Render(context, worldMatrix);
	}

	bool ModelRenderer::LoadModel(const std::string& filepath)
	{
		// Get Device

		auto device = Engine::GetInstance().GetRenderer()->GetDevice();
		auto loadedModel = ModelLoader::GetInstance().LoadModel(device, filepath);

		if (loadedModel)
		{
			m_model = loadedModel.release();
			m_modelPath = filepath;

			// setting bounding box
			GetOwner()->SetBounds(m_model->GetBounds());

			return true;
		}

		return false;
	}

}

