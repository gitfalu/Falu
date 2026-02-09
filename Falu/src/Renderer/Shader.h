/*****************************************************************//**
 * \file   Shader.h
 * \brief  シェーダー
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Falu
{

	using Microsoft::WRL::ComPtr;

	struct ShaderDefine
	{
		std::string name;
		std::string value;
	};

	class Shader
	{
	public:
		Shader();
		~Shader();

		bool LoadVertexShader(ID3D11Device* device,const std::wstring& filename,
			const std::vector<ShaderDefine>& define = {});
		bool LoadPixelShader(ID3D11Device* device, const std::wstring& filename,
			const std::vector<ShaderDefine>& define = {});
		bool LoadGeometryShader(ID3D11Device* device, const std::wstring& filename,
			const std::vector<ShaderDefine>& define = {});

		bool CreateInputLayout(ID3D11Device* device, 
			const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);

		void Bind(ID3D11DeviceContext* context);
		void Unbind(ID3D11DeviceContext* context);

		ID3D11VertexShader* GetVertexShader()const { return m_vertexShader.Get(); }
		ID3D11PixelShader* GetPixelShader() const { return m_pixelShader.Get(); }
		ID3D11GeometryShader* GetGeometryShader() const { return m_geometryShader.Get(); }
		ID3D11InputLayout* GetInputLayput() const { return m_inputLayout.Get(); }

	private:
		bool CompileFromFile(const std::wstring& filename, const char* entryPoint,
			const char* profile, const std::vector<ShaderDefine>& defines,
			ID3DBlob** blob);

	private:
		ComPtr<ID3D11VertexShader> m_vertexShader;
		ComPtr<ID3D11PixelShader> m_pixelShader;
		ComPtr<ID3D11GeometryShader> m_geometryShader;
		ComPtr<ID3D11InputLayout> m_inputLayout;
		ComPtr<ID3DBlob> m_vertexShaderBlob;
	};

	class ShaderManager
	{
	public:
		static ShaderManager& GetInstance();

		Shader* LoadShader(ID3D11Device* device, const std::string& name,
			const std::wstring& vsFile, const std::wstring& psFile,
			const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);

		Shader* GetShader(const std::string& name);
		void Clear();

	private:
		ShaderManager() = default;
		~ShaderManager() = default;
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;

	private:
		std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

	};
}
