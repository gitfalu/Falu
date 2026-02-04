/*****************************************************************//**
 * \file   Mesh.h
 * \brief  ÉÅÉbÉVÉÖ
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "../Include/MathHelper.h"

namespace Falu
{
	using Microsoft::WRL::ComPtr;

	struct Vertex
	{
		Math::Vector3 position;
		Math::Vector3 normal;
		DirectX::XMFLOAT2 texCoord;
		Math::Vector4 color;

		Vertex()
			:position(0.0f,0.0f,0.0f)
			,normal(0.0f,1.0f,0.0f)
			,texCoord(0.0f,0.0f)
			,color(1.0f,1.0f,1.0f,1.0f)
		{}


		Vertex(const Math::Vector3& pos,const Math::Vector3& norm,
			const DirectX::XMFLOAT2& tex,const Math::Vector4& col)
			:position(pos),normal(norm),texCoord(tex),color(col)
		{}
	};


	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		bool Create(ID3D11Device* device, const std::vector<Vertex>& vertices,
			const std::vector<unsigned int>& indicecs);

		void Render(ID3D11DeviceContext* context);
		void Release();

		//=== Promitive creators ===
		static std::unique_ptr<Mesh> CreateTriangle(ID3D11Device* device);
		static std::unique_ptr<Mesh> CreateQuad(ID3D11Device* device);
		static std::unique_ptr<Mesh> CreateCube(ID3D11Device* device);
		static std::unique_ptr<Mesh> CreateSphere(ID3D11Device* device, int segments = 32);
		static std::unique_ptr<Mesh> CreateCylinder(ID3D11Device* device,int segments = 32);
		static std::unique_ptr<Mesh> CreatePlane(ID3D11Device* device,
			float width = 10.0f,float depth = 10.0f,int division = 10);
		
		//=== Getters ===
		unsigned int GetVertexCount() const { return m_vertexCount; }
		unsigned int GetIndexCount() const { return m_indexCount; }
		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		const std::vector<unsigned int>& GetIndices() const { return m_indices; }

	private:
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11Buffer> m_indexBuffer;

		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;

		unsigned int m_vertexCount;
		unsigned int m_indexCount;
	};
}
