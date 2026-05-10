/*****************************************************************//**
 * \file   ModelLoader.h
 * \brief  ModelLoaderçÏê¨
 * 
 * \author tsunn
 * \date	2026/03/09
 *********************************************************************/
#pragma once
#include <d3d11.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Falu
{
	class Model;
	class Mesh;
	class Material;
	class Texture;

	class ModelLoader
	{
	public:
		static ModelLoader& GetInstance()
		{
			static ModelLoader instance;
			return instance;
		}

		// Load Model
		std::unique_ptr<Model> LoadModel(ID3D11Device* device, const std::string& filepath);

		// Setting Texture Path
		void SetTextureDirectory(const std::string& directory) { m_textureDirectory = directory; }

	private:
		ModelLoader() = default;
		~ModelLoader() = default;
		ModelLoader(const ModelLoader&) = delete;

		// Process of Assimp Scene
		void ProcessNode(void* nodePtr, const void* scenePtr, ID3D11Device* device,
			Model* model, const std::string& directory);

		std::unique_ptr<Mesh> ProcessMesh(void* meshPtr, const void* scenePtr, ID3D11Device* device);

		std::unique_ptr<Material> ProcessMaterial(void* materialPtr, const void* scenePtr,
			ID3D11Device* device, const std::string& directory);

		std::shared_ptr<Texture> LoadTexture(ID3D11Device* device, const std::string& filepath);
	private:
		std::string m_textureDirectory;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;
	};
}
