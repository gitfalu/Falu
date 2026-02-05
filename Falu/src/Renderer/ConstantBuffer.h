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

	};

}
