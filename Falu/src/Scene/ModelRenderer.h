/*****************************************************************//**
 * \file   ModelRenderer.h
 * \brief  ModelRendererçÏê¨
 * 
 * \author tsunn
 * \date   2026/03/10
 *********************************************************************/
#pragma once

#include "GameObject.h"
#include <memory>
#include <string>

namespace Falu
{
	class Model;
	
	class ModelRenderer : public Component
	{
	public:
		ModelRenderer(GameObject* owner);
		~ModelRenderer() override;

		void Update(float deltaTime) override;
		void Render() override;

		// Setting Model
		void SetModel(Model* model) { m_model = model; }
		Model* GetModel()const { return m_model; }

		// Load Model
		bool LoadModel(const std::string& filepath);

	private:
		Model* m_model;
		std::string m_modelPath;
	};
}
