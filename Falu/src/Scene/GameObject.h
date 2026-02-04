/*****************************************************************//**
 * \file   GameObject.h
 * \brief  ゲームオブジェクト
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Transform.h"

namespace Falu
{
	class Compoenent;
	class MeshRenderer;

	class GameObject
	{
	public:
		GameObject(const std::string& name = "Gameobject");
		virtual ~GameObject();

		virtual void Update(float deltaTime);
		virtual void Render();

		//=== Transform ===
		Transform& GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }

		//=== Component management ===
		template<typename T>
		T* AddComponent();

		template<typename T>
		T* GetComponent();

		template<typename T>
		void RemoveComponent();

		//=== Hierarchy ===
		void SetParent(GameObject* parent);


	private:
		std::string m_name;
		Transform m_transform;
		bool m_isActive;

		GameObject* m_parent;//親
		std::vector<GameObject*> m_children;//子供
		std::vector<std::unique_ptr<Compoenent>> m_components;//所有コンポーネント
	};
}
