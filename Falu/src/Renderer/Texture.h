/*****************************************************************//**
 * \file   Texture.h
 * \brief  テクスチャ
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace Falu
{
	using Microsoft::WRL::ComPtr;

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool LoadFromFile(ID3D11Device* device, const std::wstring& filename);
		bool CreateFromData(ID3D11Device* device,
			const void* data, int width, int height, int channels);

		void Bind(ID3D11DeviceContext* context, unsigned int slot = 0);
		void Unbind(ID3D11DeviceContext* context, unsigned int slot = 0);

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }
		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

	private:
		ComPtr<ID3D11Texture2D> m_texture;//テクスチャ情報
		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		int m_width;
		int m_height;
	};

	/// @brief テクスチャ管理クラス
	class TextureManager
	{
	public:
		static TextureManager& GetInstance();

		Texture* LoadTexture(ID3D11Device* device, 
			const std::string& name, const std::wstring& filename);
		Texture* GetTexture(const std::string& name);
		void Clear();

	private:
		TextureManager() = default;
		~TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_texture;
	};
}
