/*****************************************************************//**
 * \file   Model.h
 * \brief  ÉÇÉfÉãçÏê¨
 * 
 * \author tsunn
 * \date   2026/03/09
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <memory>
#include "Include/Math/MathHelper.h"
#include "Include/Math/Ray.h"

namespace Falu
{
	class Mesh;
	class Material;
	class Texture;

	// SubMesh
	struct SubMesh
	{
		std::unique_ptr<Mesh> mesh;
		std::unique_ptr<Material> material;
		std::string name;
	};

	// Model class
	class Model
	{
	public:
		Model();
		~Model();

		// Render
		void Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix);

		// Add SubMesh
		void AddSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material, const std::string& name = "");

		// Getter
		const std::vector<SubMesh>& GetSubMesh() const { return m_subMeshes; }
		size_t GetSubMeshCount() const { return m_subMeshes.size(); }

		// Bounding Box
		Math::AABB GetBounds() const { return m_bounds; }
		void CalculateBounds();

		// Model Infomation
		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

	private:
		std::vector<SubMesh> m_subMeshes;
		Math::AABB m_bounds;
		std::string m_name;
	};
}
