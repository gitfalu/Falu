/*****************************************************************//**
 * \file   Texture.cpp
 * \brief  テクスチャ実装
 * 
 * \author tsunn
 * \date   2026/02/07
 * 
 * wincodecについてのリファレンス
 * https://learn.microsoft.com/ja-jp/windows/win32/api/wincodec
 * 
 * WIC API についてのリファレンス
 * https://learn.microsoft.com/ja-jp/windows/win32/wic/-wic-api
 * 
 *********************************************************************/
#include "Texture.h"

#define USE_STB_IMAGE

#ifdef USE_STB_IMAGE
 // stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#else
#include <wincodec.h>// 画像処理に使うヘッダ
#pragma comment(lib,"windowscodecs.lib")
#endif

namespace Falu
{
	Texture::Texture()
		: m_width(0)
		, m_height(0)
	{

	}

	Texture::~Texture()
	{

	}

	bool Texture::LoadFromFile(ID3D11Device* device, const std::wstring& filename)
	{
#ifdef USE_STB_IMAGE
		return LoadFromFile_STB(device, filename);
#else
		return LoadFromFile_WIC(device, filename);
#endif
	}

	bool Texture::LoadFromFile_STB(ID3D11Device* device, const std::wstring& filename)
	{
		// ワイド文字列をマルチバイト文字列に変換
		char filenameStr[512];
		size_t convertedChars = 0;
		wcstombs_s(&convertedChars, filenameStr, filename.c_str(), sizeof(filenameStr));

		// stb_imageで画像を読み込み
		int width, height, channels;
		unsigned char* data = stbi_load(filenameStr, &width, &height, &channels, 4);

		if (!data)
		{
			OutputDebugStringA("[Texture::STB] Failed to Load: ");
			OutputDebugStringA(filenameStr);
			OutputDebugStringA("\n");
			return false;
		}

		m_width = width;
		m_height = height;

		bool result = CreateFromData(device, data, width, height, 4);
		stbi_image_free(data);

		return result;
	}

	bool Texture::LoadFromFile_WIC(ID3D11Device* device, const std::wstring& filename)
	{
#ifndef USE_STB_IMAGE
		// WICファクトリーの作成
		ComPtr<IWICImagingFactory> wicFactory;
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);

		if (FAILED(hr))
			return false;

		// デコーダの作成
		ComPtr<IWICBitmapDecoder> decoder;
		hr = wicFactory->CreateDecoderFromFilename(
			filename.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);

		if (FAILED(hr))
			return false;

		// フレームの取得
		ComPtr<IWICBitmapFrameDecode> frame;
		hr = decoder->GetFrame(0, &frame);
		if (FAILED(hr))
			return false;

		// サイズの取得
		UINT width, height;
		hr = frame->GetSize(&width, &height);
		if (FAILED(hr))
			return false;

		m_width = static_cast<int>(width);
		m_height = static_cast<int>(height);

		// フォーマットコンバータの作成
		ComPtr<IWICFormatConverter> converter;
		hr = wicFactory->CreateFormatConverter(&converter);
		if (FAILED(hr))
			return false;

		hr = converter->Initialize(
			frame.Get(),
			GUID_WICPixelFormat32bppRGBA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0,
			WICBitmapPaletteTypeCustom
		);

		if(FAILED(hr))
			return false;

		// ピクセルデータ取得
		UINT stride = width * 4;
		UINT imageSize = stride * height;
		std::vector<BYTE> pixels(imageSize);

		hr = converter->CopyPixels(
			nullptr,
			stride,
			imageSize,
			pixels.data()
		);

		if (FAILED(hr))
			return false;

		// テクスチャの作成
		return CreateFromData(device,pixels.data(),m_width,m_height,4);
#else
		return false;
#endif
	}

	bool Texture::CreateFromData(ID3D11Device* device, const void* data, int width, int height, int channels)
	{
		m_width = width;
		m_height = height;

		// テクスチャの作成
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;
		initData.SysMemPitch = width * channels;

		HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &m_texture);
		if (FAILED(hr))
			return false;

		// シェーダーリソースビューの作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_shaderResourceView);
		return SUCCEEDED(hr);
	}

	void Texture::Bind(ID3D11DeviceContext* context, unsigned int slot)
	{
		if (m_shaderResourceView)
		{
			context->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
		}
	}

	void Texture::Unbind(ID3D11DeviceContext* context, unsigned int slot)
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->PSSetShaderResources(slot, 1, &nullSRV);
	}

	//******************************************************
	// 
	// TextureManager
	// 
	//******************************************************

	TextureManager& TextureManager::GetInstance()
	{
		static TextureManager instance;
		return instance;
	}
	Texture* TextureManager::LoadTexture(ID3D11Device* device, const std::string& name, const std::wstring& filename)
	{
		// すでに読み込まれているか確認
		auto it = m_texture.find(name);
		if (it != m_texture.end())
		{
			// 見つかったテクスチャを返す
			return it->second.get();
		}

		// 新しいテクスチャを読み込み
		auto texture = std::make_unique<Texture>();
		if (!texture->LoadFromFile(device, filename))
		{
			return nullptr;
		}

		Texture* ptr = texture.get();
		m_texture[name] = std::move(texture);
		return ptr;
	}

	Texture* TextureManager::GetTexture(const std::string& name)
	{
		auto it = m_texture.find(name);
		if (it != m_texture.end())
		{
			return it->second.get();
		}
		return nullptr;
	}

	void TextureManager::Clear()
	{
		m_texture.clear();
	}
}
