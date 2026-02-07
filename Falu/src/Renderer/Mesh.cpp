/*****************************************************************//**
 * \file   Mesh.cpp
 * \brief  Mesh実装
 * 
 * \author tsunn
 * \date   2026/02/07
 *********************************************************************/
#include "Mesh.h"


namespace Falu
{
	Mesh::Mesh()
		:m_vertexCount(0)
		,m_indexCount(0)
	{

	}

	Mesh::~Mesh()
	{
		Release();
	}

	bool Mesh::Create(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicecs)
	{
		m_vertices = vertices;
		m_indices = indicecs;
		m_vertexCount = static_cast<unsigned int>(vertices.size());
		m_indexCount = static_cast<unsigned int>(indicecs.size());

		// 頂点バッファの作成
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = vertices.data();

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hr))
			return false;

		// インデックスバッファの作成
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = indicecs.data();

		hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		return SUCCEEDED(hr);
	}

	void Mesh::Render(ID3D11DeviceContext* context)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(m_indexCount, 0, 0);
	}

	void Mesh::Release()
	{
		m_vertexBuffer.Reset();
		m_indexBuffer.Reset();
		m_vertices.clear();
		m_indices.clear();
	}

	//========= 図形作成 =========

	std::unique_ptr<Mesh> Mesh::CreateTriangle(ID3D11Device* device)
	{
		std::vector<Vertex> vertices =
		{
			{ Math::Vector3( 0.0f, 0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.5f,0.0f }, Math::Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,1.0f }, Math::Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ Math::Vector3(-0.5f,-0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(0.0f, 0.0f, 1.0f, 1.0f) }
		};

		std::vector<unsigned int>indices = { 0,1,2 };

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}

	std::unique_ptr<Mesh> Mesh::CreateQuad(ID3D11Device* device)
	{
		std::vector<Vertex> vertices =
		{
			{ Math::Vector3(-0.5f, 0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.5f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f,-0.5f, 0.0f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) }
		};

		std::vector<unsigned int>indices = { 0,1,2,0,2,3 };

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}

	std::unique_ptr<Mesh> Mesh::CreateCube(ID3D11Device* device)
	{
		std::vector<Vertex> vertices =
		{
			// Front
			{ Math::Vector3(-0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },

			// Back
			{ Math::Vector3( 0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },

			// Top
			{ Math::Vector3(-0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },

			// Bottom
			{ Math::Vector3(-0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },

			// Right
			{ Math::Vector3( 0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3( 0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },

			// Left
			{ Math::Vector3(-0.5f,-0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 0.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f, 0.5f, 0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	0.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f, 0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), {	1.0f,0.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ Math::Vector3(-0.5f,-0.5f,-0.5f), Math::Vector3(0.0f, 0.0f, -1.0f), { 1.0f,1.0f }, Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f) }
		};

		std::vector<unsigned int>indices = 
		{
			 0, 1, 2, 0, 2, 3,// Front
			 4, 5, 6, 4, 6, 7,// Back
			 8, 9,10, 8,10,11,// Top
			12,13,14,12,14,15,// Bottom
			16,17,18,16,18,19,// Right
			20,21,22,20,22,23 // Left
		};

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}

	std::unique_ptr<Mesh> Mesh::CreateSphere(ID3D11Device* device, int segments)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		const float PI = 3.14159265358979323846f;
		const int rings = segments;
		const int sectors = segments * 2; // 扇の個数(?)

		for (int i = 0; i <= rings; ++i)
		{
			float phi = PI * i / rings;

			for (int j = 0; j <= sectors; ++j)
			{
				float theta = 2.0f * PI * j / sectors;

				float x = sinf(phi) * cosf(theta);
				float y = cosf(phi);
				float z = sinf(phi) * sinf(theta);

				float u = static_cast<float>(j) / sectors;
				float v = static_cast<float>(i) / rings;

				Vertex vertex;
				vertex.position = Math::Vector3(x, y, z);
				vertex.normal = Math::Vector3(x, y, z);
				vertex.texCoord = DirectX::XMFLOAT2(u, v);
				vertex.color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

				vertices.push_back(vertex);
			}
		}

		for (int i = 0; i < rings; ++i)
		{
			for (int j = 0; j < sectors; ++j)
			{
				int first = i * (sectors + 1) + j;
				int second = first + sectors + 1;
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}

	std::unique_ptr<Mesh> Mesh::CreateCylinder(ID3D11Device* device, int segments)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		const float PI = 3.14159265358979323846f;
		const float height = 1.0f;
		const float radius = 0.5f;

		// 側面
		for (int i = 0; i <= segments; ++i)
		{
			float theta = 2.0f * PI * i / segments;
			float x = cosf(theta) * radius;
			float z = sinf(theta) * radius;

			// 上部
			vertices.push_back({
				Math::Vector3(x, height / 2.0f,z),
				Math::Vector3(x / radius, 0,z/radius),
				DirectX::XMFLOAT2(static_cast<float>(i) / segments,0.0f),
				Math::Vector4(1.0f,1.0f,1.0f,1.0f)
				});

			// 下部
			vertices.push_back({
				Math::Vector3(x, -height / 2.0f,z),
				Math::Vector3(x / radius, 0,z / radius),
				DirectX::XMFLOAT2(static_cast<float>(i) / segments,1.0f),
				Math::Vector4(1.0f,1.0f,1.0f,1.0f)
				});
		}

		for (int i = 0; i < segments; ++i)
		{
			int base = i * 2;
			indices.push_back(base);
			indices.push_back(base + 1);
			indices.push_back(base + 2);

			indices.push_back(base + 1);
			indices.push_back(base + 3);
			indices.push_back(base + 2);
		}

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}

	std::unique_ptr<Mesh> Mesh::CreatePlane(ID3D11Device* device, float width, float depth, int division)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		float halfWidth = width / 2.0f;
		float halfDepth = depth / 2.0f;

		for (int z = 0; z <= division; ++z)
		{
			for (int x = 0; x <= division; ++x)
			{
				float xPos = -halfWidth + (width * x / division);
				float zPos = -halfDepth + (depth * z / division);

				Vertex vertex;
				vertex.position = Math::Vector3(xPos, 0.0f, zPos);
				vertex.normal = Math::Vector3(0.0f, 1.0f, 0.0f);
				vertex.texCoord = DirectX::XMFLOAT2(
					static_cast<float>(x) / division,
					static_cast<float>(z) / division
				);
				vertex.color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

				vertices.push_back(vertex);
			}
		}

		for (int z = 0; z < division; ++z)
		{
			for (int x = 0; x < division; ++x)
			{
				int topLeft = z * (division + 1) + x;
				int topRight = topLeft + 1;
				int bottomLeft = (z + 1) * (division + 1) + x;
				int bottomRight = bottomLeft + 1;

				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		auto mesh = std::make_unique<Mesh>();
		if (mesh->Create(device, vertices, indices))
		{
			return mesh;
		}

		return nullptr;
	}
}
