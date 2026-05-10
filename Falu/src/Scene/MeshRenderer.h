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

		void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
		void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }

		std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
		std::shared_ptr<Material> GetMaterial() const { return m_material; }

	private:
		std::shared_ptr<Mesh> m_mesh;
		std::shared_ptr<Material> m_material;
	};
}