/*****************************************************************//**
 * \file   ConstantBuffer.h
 * \brief  コンスタントバッファー(行列、ベクトル、
 *							色などの頻繁に更新される定数データ)
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

namespace Falu
{
	using Microsoft::WRL::ComPtr;

	//====== 基本的な定数バッファ構造体 =======
	struct PerObjectConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
	};

	struct PerFrameConstantBuffer
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMFLOAT4 cameraPosition;
		DirectX::XMFLOAT4 ambientLight;
		float time;
		float deltaTime;
		DirectX::XMFLOAT2 padding;
	};

	struct LightConstantBuffer
	{
		DirectX::XMFLOAT4 lightPosition;
		DirectX::XMFLOAT4 lightDirection;
		DirectX::XMFLOAT4 lightColor;
		DirectX::XMFLOAT4 lightParam; // x: intensity, y: range, z:type, w: unused
	};

	//====== 汎用定数バッファクラス ======
	template<typename T>
	class ConstantBuffer
	{
	public:
		ConstantBuffer() :m_isDirty(true) {}
		~ConstantBuffer() = default;

		bool Initialize(ID3D11Device* device)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(T);
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = device->CreateBuffer(&desc, nullptr, &m_buffer);
			return SUCCEEDED(hr);
		}

		void Update(ID3D11DeviceContext* context, const T& data)
		{
			m_data = data;
			m_isDirty = true;

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			if (SUCCEEDED(hr))
			{
				memcpy(mappedResource.pData, &m_data, sizeof(T));
				context->Unmap(m_buffer.Get(), 0);
				m_isDirty = false;
			}
		}

		//=== Vertex Shader ===
		void BindVS(ID3D11DeviceContext* context, UINT slot)
		{
			context->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
		}

		//=== Pixel Shader ===
		void BindPS(ID3D11DeviceContext* context, UINT slot)
		{
			context->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
		}

		//=== Geometry Shader ===
		void BindGS(ID3D11DeviceContext* context, UINT slot)
		{
			context->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
		}

		T& GetData() { return m_data; }
		const T& GetData() const { return m_data; }
		ID3D11Buffer* GetBuffer() const { return m_buffer.Get(); }

	private:
		ComPtr<ID3D11Buffer> m_buffer;
		T m_data;
		bool m_isDirty;
	};
}
