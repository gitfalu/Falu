/*****************************************************************//**
 * \file   Renderer.h
 * \brief  レンダー
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <memory>
#include "../Include/MathHelper.h"

namespace Falu
{
	using Microsoft::WRL::ComPtr;


	class Camera;
	class Mesh;
	class Material;

	struct RenderSettings
	{
		bool enableVSync = true;
		bool enableMSAA = false;
		int msaaSampleCount = 4;
		Math::Color clearColor = Math::Color(0.1f, 0.1f, 0.3f, 1.0f);
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		bool Initialize(HWND hWnd, int width, int height, const RenderSettings& settings);
		void Shutdown();
		void OnResize(int width, int height);

		void BeginFrame();
		void EndFrame();

		void SetCamera(Camera* camera);
		void RenderMesh(Mesh* mesh, Material* material, const DirectX::XMMATRIX& worldMatrix);

		//=== Getters === 
		ID3D11Device* GetDevice() const { return m_device.Get(); }
		ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
		Camera* GetCamera() const { return m_currentCamera; }

		int GetWidth() const { return m_width;}
		int GetHeight() const { return m_height; }

		void SetClearColor(const Math::Color& color) { m_settings.clearColor = color; }

	private:
		bool CreateDeviceAndSwaphain(HWND hWnd);
		bool CreateRenderTargetView();
		bool CreateDepthStencilBuffer();
		bool CreateDepthStencilStates();
		bool CreateRasterizerStates();
		bool CreateBlendStates();
		bool CreateSamplerStates();
		void SetupViewport();

	private:
		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_context;
		ComPtr<IDXGISwapChain> m_swapChain;
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		ComPtr<ID3D11Texture2D> m_depthStencilBuffer;//深度ステンシル/Zバッファ
		ComPtr<ID3D11DepthStencilView> m_depthStencView;//深度ステンシル/Zバッファのビュー
		ComPtr<ID3D11DepthStencilState> m_depthStencilState;//深度ステンシル/Zバッファの状態
		ComPtr<ID3D11DepthStencilState> m_depthDisabledState;
		ComPtr<ID3D11RasterizerState> m_rasterizerState;
		ComPtr<ID3D11RasterizerState> m_wireframeState;//ワイヤーフレーム
		ComPtr<ID3D11BlendState> m_alphaBlendState;
		ComPtr<ID3D11SamplerState> m_samplerState;
		

		Camera* m_currentCamera;
		RenderSettings m_settings;

		int m_width;
		int m_height;
	};
}
