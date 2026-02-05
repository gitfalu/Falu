/*****************************************************************//**
 * \file   MeshRenderer.h
 * \brief  メッシュレンダラー
 * 
 * \author tsunn
 * \date   2026/02/05
 *********************************************************************/
#pragma once

#include "GameObject.h"

namespace Falu
{
	class Mesh;
	class Material;

	class MeshRenderer :public Component
	{
	public:
		MeshRenderer(GameObject* owner);
		~MeshRenderer() override;

		void Render() override;

		void SetMesh(Mesh* mesh) { m_mesh = mesh; }
		void SetMaterial(Material* material) { m_material = material; }

		Mesh* GetMesh() const { return m_mesh; }
		Material* GetMaterial() const { return m_material; }

	private:
		Mesh* m_mesh;
		Material* m_material;
	};
}