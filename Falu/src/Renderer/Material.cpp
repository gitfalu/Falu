/*****************************************************************//**
 * \file   Material.cpp
 * \brief  マテリアル実装
 * 
 * \author tsunn
 * \date   2026/02/07
 *********************************************************************/
#include "Material.h"

#include "Shader.h"
#include "Texture.h"

namespace Falu
{
	Material::Material()
		:m_shader(nullptr)
		, m_albedoTexture(nullptr)
		, m_normalTexture(nullptr)
		, m_metalicTexture(nullptr)
		, m_roughnesTexture(nullptr)
	{

	}

	Material::~Material()
	{

	}

	bool Material::Initialize(ID3D11Device* device)
	{
		// 定数バッファの作成
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MaterialConstantBuffer);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
		return SUCCEEDED(hr);
	}

	void Material::SetProperties(const MaterialProperties& props)
	{
		m_properties = props;
	}

	void Material::Bind(ID3D11DeviceContext* context)
	{
		// シェーダーのバインド
		if (m_shader)
		{
			m_shader->Bind(context);
		}

		// 定数バッファの更新
		UpdateConstantBuffer(context);

		// テクスチャのバインド
		if (m_albedoTexture)
			m_albedoTexture->Bind(context, 0);

		if (m_normalTexture)
			m_normalTexture->Bind(context, 1);

		if (m_metalicTexture)
			m_metalicTexture->Bind(context, 2);

		if (m_roughnesTexture)
			m_roughnesTexture->Bind(context, 3);
	}

	void Material::UpdateConstantBuffer(ID3D11DeviceContext* context)
	{
		if (!m_constantBuffer)
			return;

		MaterialConstantBuffer cb;
		cb.albedo = DirectX::XMFLOAT4(
			m_properties.albedo.r,
			m_properties.albedo.g,
			m_properties.albedo.b,
			m_properties.albedo.a
		);

		cb.properties = DirectX::XMFLOAT4(
			m_properties.metalic,
			m_properties.roughness,
			m_properties.ao,
			0.0f
		);

		cb.emissive = DirectX::XMFLOAT4(
			m_properties.emissive.x,
			m_properties.emissive.y,
			m_properties.emissive.z,
			0.0f
		);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr))
		{
			memcpy(mappedResource.pData, &cb, sizeof(MaterialConstantBuffer));
			context->Unmap(m_constantBuffer.Get(), 0);
		}

		// 定数バッファをピクセルシェーダーにバインド
		context->PSSetConstantBuffers(2, 1, m_constantBuffer.GetAddressOf());
	}
}
