/*****************************************************************//**
 * \file   ModelLoader.cpp
 * \brief  ModelLoaderŽÀ‘•
 * 
 * \author tsunn
 * \date   2026/03/09
 *********************************************************************/
#include "ModelLoader.h"

#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

namespace Falu
{
	std::unique_ptr<Model> ModelLoader::LoadModel(ID3D11Device* device, const std::string& filepath)
	{
		Assimp::Importer importer;

		// Load Assimp Flag
		unsigned int flags =
			aiProcess_Triangulate | // everything to Triangle
			aiProcess_GenNormals |  // Create Normals
			aiProcess_CalcTangentSpace | // Calc tangent space
			aiProcess_FlipUVs | // Flip UV(for DirectX)
			aiProcess_JoinIdenticalVertices | // Join Idential Vertices
			aiProcess_SortByPType; // Sort for Primitive type

		// Load Scene
		const aiScene* scene = importer.ReadFile(filepath, flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			char msg[512];
			sprintf_s(msg, "[ModelLoader] ERROR: %s\n", importer.GetErrorString());
			OutputDebugStringA(msg);
			return nullptr;
		}

		// Get Directory Path
		std::filesystem::path path(filepath);
		std::string directory = path.parent_path().string();

		// Create Model
		auto model = std::make_unique<Model>();
		model->SetName(path.filename().string());

		// Process Node Recursively
		ProcessNode(scene->mRootNode, scene, device, model.get(), directory);

		// calclate bounding box
		model->CalculateBounds();

		char msg[256];
		sprintf_s(msg, "[ModelLoader] Loaded: %s (%zu meshes)\n",
			filepath.c_str(), model->GetSubMeshCount());
		OutputDebugStringA(msg);

		return model;
	}

	void ModelLoader::ProcessNode(void* nodePtr, const void* scenePtr, ID3D11Device* device, Model* model, const std::string& directory)
	{
		aiNode* node = static_cast<aiNode*>(nodePtr);
		const aiScene* scene = static_cast<const aiScene*>(scenePtr);

		// Process all mesh in this node 
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			auto loadedMesh = ProcessMesh(mesh, scene, device);
			auto loadedMaterial = ProcessMaterial(scene->mMaterials[mesh->mMaterialIndex],
				scene, device, directory);

			model->AddSubMesh(std::move(loadedMesh), std::move(loadedMaterial), mesh->mName.C_Str());
		}
		
		// Process Child node Recursively
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, device, model, directory);
		}
	}

	std::shared_ptr<Mesh> ModelLoader::ProcessMesh(void* meshPtr, const void* scenePtr, ID3D11Device* device)
	{
		aiMesh* mesh = static_cast<aiMesh*>(meshPtr);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Process Vertex Data
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex vertex;

			// Position
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			// Normals
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// UV coordinate (texture Coordinate)
			if (mesh->mTextureCoords[0])
			{
				vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoord = Math::Vector2(0.0f, 0.0f);
			}

			// Vertex Color
			if (mesh->HasVertexColors(0))
			{
				vertex.color.r = mesh->mColors[0][i].r;
				vertex.color.g = mesh->mColors[0][i].g;
				vertex.color.b = mesh->mColors[0][i].b;
				vertex.color.a = mesh->mColors[0][i].a;
			}
			else
			{
				vertex.color = Math::Color(1,1,1,1);
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[i]);
			}
		}

		// Create Mesh
		auto loadedMesh = std::make_shared<Mesh>();
		if (!loadedMesh->Initialize(device, vertices, indices))
		{
			OutputDebugStringA("[ModelLoader] ERROR: Failed to create mesh\n");
			return nullptr;
		}

		return loadedMesh;
	}

	std::shared_ptr<Material> ModelLoader::ProcessMaterial(void* materialPtr, const void* scenePtr, ID3D11Device* device, const std::string& directory)
	{
		aiMaterial* material = static_cast<aiMaterial*>(materialPtr);

		auto loadedMaterial = std::make_shared<Material>();

		// albedo (deffuse color)
		aiColor3D color(1.0f, 1.0f, 1.0f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		loadedMaterial->SetAlbedo(Math::Color(color.r,color.g,color.b,1.0f));

		// Metallic
		float shininess = 0.0f;
		material->Get(AI_MATKEY_SHININESS, shininess);
		loadedMaterial->SetMetallic(shininess / 128.0f);

		// Diffuse Texture
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString texPath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

			std::string fullpath = directory + "/" + texPath.C_Str();
			auto texture = LoadTexture(device, fullpath);

			if (texture)
			{
				loadedMaterial->SetAlbedoTexture(texture.get());
			}
		}

		// Normal map
		if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString texPath;
			material->GetTexture(aiTextureType_NORMALS, 0, &texPath);

			std::string fullpath = directory + "/" + texPath.C_Str();
			auto texture = LoadTexture(device, fullpath);

			if (texture)
			{
				loadedMaterial->SetNormalTexture(texture.get());
			}
		}

		return loadedMaterial;
	}

	std::shared_ptr<Texture> ModelLoader::LoadTexture(ID3D11Device* device, const std::string& filepath)
	{
		// Check Cache
		auto it = m_textureCache.find(filepath);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		// Load Texture
		auto texture = std::make_shared<Texture>();
		auto result = MultiByteToWideChar(CP_UTF8, 0, filepath.c_str(),
			filepath.length(), nullptr, 0);
		std::wstring wfilepath;
		wfilepath.resize(result);
		result = MultiByteToWideChar(CP_UTF8,
			0, filepath.c_str(),filepath.length(),
			wfilepath.data(), wfilepath.size());
		if (!texture->LoadFromFile(device, wfilepath))
		{
			char msg[512];
			sprintf_s(msg, "[ModelLoader] WARNING: Failed to load texture: %s\n", filepath.c_str());
			OutputDebugStringA(msg);
			return nullptr;
		}

		// Add cache
		m_textureCache[filepath] = texture;

		return texture;
	}
}
