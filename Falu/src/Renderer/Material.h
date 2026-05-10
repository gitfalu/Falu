/*****************************************************************//**
 * \file   Material.h
 * \brief  マテリアル
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Include/Math/MathHelper.h"
#include <memory>

namespace Falu
{
	using Microsoft::WRL::ComPtr;
	class Shader;
	class Texture;

	struct MaterialProperties
	{
		Math::Color albedo;
		float metallic;
		float roughness;
		float ao;
		Math::Vector3 emissive;

		MaterialProperties()
			:albedo(1.0f,1.0f,1.0f,1.0f)
			,metallic(0.0f)
			,roughness(0.5f)
			,ao(1.0f)
			,emissive(0.0f,0.0f,0.0f)
		{}
	};

	struct MaterialConstantBuffer
	{
		DirectX::XMFLOAT4 albedo;
		DirectX::XMFLOAT4 properties; // x: metaric ,y: roughness, z: ao ,w: unused
		DirectX::XMFLOAT4 emissive;
	};

	class Material
	{
	public:
		Material();
		~Material();

		bool Initialize(ID3D11Device* device);

		// Material Param Setter
		void SetAlbedo(const Math::Color& color) { m_albedo = color; }
		void SetMetallic(float metallic) { m_metallic = metallic; }
		void SetRoughness(float roughness) { m_roughness = roughness; }
		void SetAO(float ao) { m_ao = ao; }
		void SetEmissive(const Math::Color& emissive) { m_emissive = emissive; }

		// Material Param Getter
		Math::Color GetAlbedo()const { return m_albedo; }
		float GetMetallic() const { return m_metallic; }
		float GetRoughness()const { return m_roughness; }
		float GetAO() const { return m_ao; }
		Math::Color GetEmissive() const { return m_emissive; }

		// Texture Setter
		void SetAlbedoTexture(Texture* texture) { m_albedoTexture = texture; }
		void SetNormalTexture(Texture* texture) { m_normalTexture = texture; }
		void SetMetallicTexture(Texture* texture) { m_metalicTexture = texture; }
		void SetRoughnessTexture(Texture* texture) { m_roughnesTexture = texture; }

		// Texture Getter
		Texture* GetAlbedoTexture() const { return m_albedoTexture; }
		Texture* GetNormalTexture() const { return m_normalTexture; }
		Texture* GetMetallicTexture() const { return m_metalicTexture; }
		Texture* GetRoughnessTexture() const { return m_roughnesTexture; }

		void Bind(ID3D11DeviceContext* context);
		void UpdateConstantBuffer(ID3D11DeviceContext* context);

		// Settings Shader
		Shader* GetShader() const { return m_shader; }
		void SetShader(Shader* shader) { m_shader = shader; }

		// Settings Properties
		const MaterialProperties& GetProperties() const { return m_properties; }
		void SetProperties(const MaterialProperties& props);
	private:
		Shader* m_shader;
		MaterialProperties m_properties;
		ComPtr<ID3D11Buffer> m_constantBuffer;

		// Texture
		Texture* m_albedoTexture;
		Texture* m_normalTexture;
		Texture* m_metalicTexture;
		Texture* m_roughnesTexture;//反射する滑らかさを調整するマテリアル

		// Material Paramator
		Math::Color m_albedo;
		float m_metallic;
		float m_roughness;
		float m_ao;
		Math::Color m_emissive;
	};
}
