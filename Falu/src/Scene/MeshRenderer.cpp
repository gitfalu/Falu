/*****************************************************************//**
 * \file   MeshRenderer.cpp
 * \brief  メッシュレンダラー実装
 * 
 * \author tsunn
 * \date   2026/02/08
 *********************************************************************/
#include "MeshRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Falu/Engine.h"
#include "Renderer/Renderer.h"

namespace Falu
{
	MeshRenderer::MeshRenderer(GameObject* owner)
		: Component(owner)
		, m_mesh(nullptr)
		, m_material(nullptr)
	{

	}

	MeshRenderer::~MeshRenderer()
	{

	}

	void MeshRenderer::Render()
	{
		if (!m_mesh || !m_material || !m_owner)
			return;

		auto renderer = Engine::GetInstance().GetRenderer();
		if (!renderer)
			return;

		// ワールド行列の取得
		DirectX::XMMATRIX worldMatrix = m_owner->GetTransform().GetWorldMatrix();

		// メッシュの描画
		renderer->RenderMesh(m_mesh, m_material, worldMatrix);
	}
}
