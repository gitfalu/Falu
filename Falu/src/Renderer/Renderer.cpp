/*****************************************************************//**
 * \file   Renderer.cpp
 * \brief  レンダラー実装
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

namespace Falu
{
	Renderer::Renderer()
		:m_currentCamera(nullptr)
		,m_width(0)
		,m_height(0)
	{

	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	bool Renderer::Initialize(HWND hWnd, int width, int height, const RenderSettings& settings)
	{
		m_width = width;
		m_height = height;
		m_settings = settings;

		if (!CreateDeviceAndSwapChain(hWnd))
			return false;
		if (!CreateRenderTargetView())
			return false;
		if (!CreateDepthStencilBuffer())
			return false;
		if (!CreateDepthStencilStates())
			return false;
		if (!CreateRasterizerStates())
			return false;
		if (!CreateBlendStates())
			return false;
		if (!CreateSamplerStates())
			return false;

		// 2026/02/17
		if (!m_perObjectCB.Initialize(m_device.Get()))
			return false;
		if (!m_perFrameCB.Initialize(m_device.Get()))
			return false;

		SetupViewport();

		return true;
	}

	void Renderer::Shutdown()
	{
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(FALSE, nullptr);
		}

		m_samplerState.Reset();
		m_alphaBlendState.Reset();
		m_wireframeState.Reset();
		m_rasterizerState.Reset();
		m_depthDisabledState.Reset();
		m_depthStencilState.Reset();
		m_depthStencilView.Reset();
		m_depthStencilBuffer.Reset();
		m_renderTargetView.Reset();
		m_swapChain.Reset();
		m_context.Reset();
		m_device.Reset();
	}

	void Renderer::OnResize(int width, int height)
	{
		if (!m_device || !m_swapChain)
			return;

		m_width = width;
		m_height = height;

		// リソースの解放
		m_context->OMSetRenderTargets(0, nullptr, nullptr);
		m_renderTargetView.Reset();
		m_depthStencilView.Reset();
		m_depthStencilBuffer.Reset();

		// スワップチェーンのリサイズ
		HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
			return;

		// リソースの作成
		CreateRenderTargetView();
		CreateDepthStencilBuffer();

		// ビューポートの更新
		SetupViewport();

		// レンダーターゲットの設定
		m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	}

	void Renderer::BeginFrame()
	{
		float clearColor[4] = {
			m_settings.clearColor.r,
			m_settings.clearColor.g,
			m_settings.clearColor.b,
			m_settings.clearColor.a
		};

		m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_context->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Renderer::EndFrame()
	{
		UINT syncInterval = m_settings.enableVSync ? 1 : 0;
		m_swapChain->Present(syncInterval, 0);
	}

	void Renderer::SetCamera(Camera* camera)
	{
		m_currentCamera = camera;
	}

	void Renderer::RenderMesh(Mesh* mesh, Material* material, const DirectX::XMMATRIX& worldMatrix)
	{
		if (!mesh || !material || !m_currentCamera)
			return;
		using namespace DirectX;

		PerFrameConstantBuffer perFrame;
		perFrame.view = XMMatrixTranspose(m_currentCamera->GetViewMatrix());
		perFrame.projection = XMMatrixTranspose(m_currentCamera->GetProjectionMatrix());
		perFrame.viewProjection = XMMatrixTranspose(m_currentCamera->GetViewProjectionMatrix());

		Math::Vector3 camPos = m_currentCamera->GetTransform().GetPosition();
		perFrame.cameraPosition = XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0f);
		perFrame.ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		perFrame.time = 0.0f;
		perFrame.deltaTime = 0.0f;

		m_perFrameCB.Update(m_context.Get(), perFrame);
		m_perFrameCB.BindVS(m_context.Get(), 1);// register(b1)
		m_perFrameCB.BindPS(m_context.Get(), 1);

		PerObjectConstantBuffer perObject;
		perObject.world = XMMatrixTranspose(worldMatrix);
		perObject.worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, worldMatrix));

		m_perObjectCB.Update(m_context.Get(), perObject);
		m_perObjectCB.BindVS(m_context.Get(), 0);

		// マテリアルのバインド
		material->Bind(m_context.Get());

		// メッシュの描画
		mesh->Render(m_context.Get());
	}

	bool Renderer::CreateDeviceAndSwapChain(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = m_width;
		swapChainDesc.BufferDesc.Height = m_height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.SampleDesc.Count = m_settings.enableMSAA ? m_settings.msaaSampleCount : 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			&featureLevel,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_context
		);
		return SUCCEEDED(hr);
	}

	bool Renderer::CreateRenderTargetView()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
		if (FAILED(hr))
			return false;

		hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
		return SUCCEEDED(hr);
	}

	bool Renderer::CreateDepthStencilBuffer()
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = m_width;
		depthStencilDesc.Height = m_height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = m_settings.enableMSAA ? m_settings.msaaSampleCount : 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		HRESULT hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, &m_depthStencilBuffer);
		if (FAILED(hr))
			return false;

		hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, &m_depthStencilView);
		if (FAILED(hr))
			return false;

		m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		return true;
	}

	bool Renderer::CreateDepthStencilStates()
	{
		// 深度テスト有効
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = FALSE;

		HRESULT hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(hr))
			return false;

		// 深度テスト無効
		depthStencilDesc.DepthEnable = FALSE;
		hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthDisabledState);
		if (FAILED(hr))
			return false;

		m_context->OMSetDepthStencilState(m_depthStencilState.Get(),1);
		return true;
	}

	bool Renderer::CreateRasterizerStates()
	{
		// 標準ラスタライザステート
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthClipEnable = TRUE;

		HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
		if (FAILED(hr))
			return false;

		// ワイヤーフレーム
		hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_wireframeState);
		if (FAILED(hr))
			return false;

		m_context->RSSetState(m_rasterizerState.Get());
		return true;
	}

	bool Renderer::CreateBlendStates()
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);
		return SUCCEEDED(hr);
	}

	bool Renderer::CreateSamplerStates()
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
		if (SUCCEEDED(hr))
		{
			m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		}
		return SUCCEEDED(hr);
	}

	void Renderer::SetupViewport()
	{
		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(m_width);
		viewport.Height = static_cast<float>(m_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		m_context->RSSetViewports(1, &viewport);
	}
}
