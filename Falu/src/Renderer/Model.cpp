/*****************************************************************//**
 * \file   Model.cpp
 * \brief  ModelŽÀ‘•
 * 
 * \author tsunn
 * \date   2026/03/10
 *********************************************************************/
#include "Model.h"
#include "Mesh.h"
#include "Material.h"

namespace Falu
{
	Model::Model()
	{

	}

	Model::~Model()
	{

	}

	void Model::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix)
	{
		for (auto& subMesh : m_subMeshes)
		{
			if (subMesh.mesh && subMesh.material)
			{
				// Bind Material
				subMesh.material->Bind(context);

				// Render Mesh
				subMesh.mesh->Render(context);
			}
		}
	}

	void Model::AddSubMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const std::string& name)
	{
		SubMesh subMesh;
		subMesh.mesh = mesh;
		subMesh.material = material;
		subMesh.name = name;

		m_subMeshes.push_back(std::move(subMesh));
	}

	void Model::CalculateBounds()
	{
		if (m_subMeshes.empty())
		{
			m_bounds = Math::AABB(Math::Vector3(0, 0, 0), Math::Vector3(0, 0, 0));
			return;
		}

		Math::Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
		Math::Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (const auto& subMesh : m_subMeshes)
		{
			if (subMesh.mesh)
			{
				Math::AABB meshBounds = subMesh.mesh->CalculateBounds();

				min.x = std::min(min.x, meshBounds.min.x);
				min.y = std::min(min.y, meshBounds.min.y);
				min.z = std::min(min.z, meshBounds.min.z);

				max.x = std::max(max.x, meshBounds.max.x);
				max.y = std::max(max.y, meshBounds.max.y);
				max.z = std::max(max.z, meshBounds.max.z);
			}
		}

		m_bounds = Math::AABB(min, max);
	}
}
