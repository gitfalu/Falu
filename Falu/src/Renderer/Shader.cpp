/*****************************************************************//**
 * \file   Shader.cpp
 * \brief  シェーダー実装
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Shader.h"

#include <vector>

namespace Falu
{
	Shader::Shader()
	{

	}

	Shader::~Shader()
	{

	}

	bool Shader::LoadVertexShader(ID3D11Device* device, const std::wstring& filename, const std::vector<ShaderDefine>& define)
	{
		if (!CompileFromFile(filename, "VS_Main", "vs_5_0", define, &m_vertexShaderBlob))
			return false;

		HRESULT hr = device->CreateVertexShader(
			m_vertexShaderBlob->GetBufferPointer(),
			m_vertexShaderBlob->GetBufferSize(),
			nullptr,
			&m_vertexShader		
		);

		return SUCCEEDED(hr);
	}

	bool Shader::LoadPixelShader(ID3D11Device* device, const std::wstring& filename, const std::vector<ShaderDefine>& define)
	{
		ComPtr<ID3DBlob> pixelShaderBlob;
		if (!CompileFromFile(filename, "PS_Main", "ps_5_0", define, &pixelShaderBlob))
			return false;

		HRESULT hr = device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize(),
			nullptr,
			&m_pixelShader
		);

		return SUCCEEDED(hr);
	}

	bool Shader::LoadGeometryShader(ID3D11Device* device, const std::wstring& filename, const std::vector<ShaderDefine>& define)
	{
		ComPtr<ID3DBlob> geometryShaderBlob;
		if (!CompileFromFile(filename, "GS_Main", "gs_5_0", define, &geometryShaderBlob))
			return false;

		HRESULT hr = device->CreateGeometryShader(
			geometryShaderBlob->GetBufferPointer(),
			geometryShaderBlob->GetBufferSize(),
			nullptr,
			&m_geometryShader
		);

		return SUCCEEDED(hr);
	}

	bool Shader::CreateInputLayout(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements)
	{
		if (!m_vertexShaderBlob)
			return false;

		HRESULT hr = device->CreateInputLayout(
			layout,
			numElements,
			m_vertexShaderBlob->GetBufferPointer(),
			m_vertexShaderBlob->GetBufferSize(),
			&m_inputLayout
		);

		return SUCCEEDED(hr);
	}

	void Shader::Bind(ID3D11DeviceContext* context)
	{
		if (m_vertexShader)
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		if (m_pixelShader)
			context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		if (m_geometryShader)
			context->GSSetShader(m_geometryShader.Get(), nullptr, 0);

		if (m_inputLayout)
			context->IASetInputLayout(m_inputLayout.Get());
	}

	void Shader::Unbind(ID3D11DeviceContext* context)
	{
		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
		context->GSSetShader(nullptr, nullptr, 0);
		context->IASetInputLayout(nullptr);
	}

	bool Shader::CompileFromFile(const std::wstring& filename, const char* entryPoint, const char* profile, const std::vector<ShaderDefine>& defines, ID3DBlob** blob)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		// ShaderDefineをD3D_SHADER_MACROに変換
		std::vector<D3D_SHADER_MACRO> macros;
		for (const auto& define : defines)
		{
			D3D_SHADER_MACRO macro;
			macro.Name = define.name.c_str();
			macro.Definition = define.value.c_str();
			macros.push_back(macro);
		}

		// 終端マーカー
		const D3D_SHADER_MACRO* macroPtr = nullptr;
		if (!defines.empty())
		{
			macros.push_back({nullptr,nullptr});
			macroPtr = macros.data();
		}
		

		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(),
			macroPtr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint,
			profile,
			shaderFlags,
			0,
			blob,
			&errorBlob
		);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
			}
			return false;
		}

		return true;
	}

	//****************************************************************
	// 
	// ShaderManager
	// 
	//****************************************************************

	ShaderManager& ShaderManager::GetInstance()
	{
		static ShaderManager Instance;
		return Instance;
	}
	Shader* ShaderManager::LoadShader(ID3D11Device* device, const std::string& name, const std::wstring& vsFile, const std::wstring& psFile, const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements)
	{
		// すでに読み込まれているか確認
		auto it = m_shaders.find(name);
		if (it != m_shaders.end())
		{
			// 見つかったらシェーダーを返す
			return it->second.get();
		}

		// 見つからなかったら新しいシェーダーを作成
		auto shader = std::make_unique<Shader>();
		// VertexShader
		if (!shader->LoadVertexShader(device, vsFile))
			return nullptr;
		// PixelShader
		if (!shader->LoadPixelShader(device, psFile))
			return nullptr;
		
		if (layout && numElements > 0)
		{
			if (!shader->CreateInputLayout(device, layout, numElements))
				return nullptr;
		}

		Shader* ptr = shader.get();
		m_shaders[name] = std::move(shader);
		return ptr;
	}
	Shader* ShaderManager::GetShader(const std::string& name)
	{
		auto it = m_shaders.find(name);
		if (it != m_shaders.end())
		{
			return it->second.get();
		}

		return nullptr;
	}
	void ShaderManager::Clear()
	{
		m_shaders.clear();
	}
}
