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
	class Component;
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
		GameObject* GetParent() const { return m_parent; }
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);
		const std::vector<GameObject*>& GetChildren() const { return m_children; }

		//=== Properties ==
		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		void SetActive(bool active) { m_isActive = active; }
		bool IsActive()const { return m_isActive; }

	protected:
		std::string m_name;
		Transform m_transform;
		bool m_isActive;

		GameObject* m_parent;//親
		std::vector<GameObject*> m_children;//子供
		std::vector<std::unique_ptr<Component>> m_components;//所有コンポーネント
	};

	//====== Component ======
	class Component
	{
	public:
		Component(GameObject* owner) :m_owner(owner), m_isEnabled(true){}
		virtual ~Component() = default;

		virtual void Update(float deltaTime) {}
		virtual void Render(){}

		GameObject* GetOwner() const { return m_owner; }
		void SetEnabled(bool enabled) { m_isEnabled = enabled; }
		bool IsEnabled() const { return m_isEnabled; }

	protected:
		GameObject* m_owner;
		bool m_isEnabled;
	};

	//====== Template implementations(コンポーネント塁) ======

	//=== コンポーネント追加 ===
	template<typename T>
	T* GameObject::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

		auto component = std::make_unique<T>(this);
		T* ptr = component.get();
		m_components.push_back(std::move(component));
		return ptr;
	}

	//=== コンポーネント情報取得 ===
	template<typename T>
	T* GameObject::GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T mmust inhirt from Component");

		for (auto& component : m_components)
		{
			T* ptr = dynamic_cast<T*>(component.get());
			if (ptr != nullptr)
				return ptr;
		}
		return nullptr;
	}

	//=== コンポーネント除外 ===
	template<typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inhirt from Component");

		m_components.erase(
			std::remove_if(m_components.begin(), m_components.end(),
				[](const std::unique_ptr<Component>& component) {
					return dynamic_cast<T*>(component.get()) != nullptr;
				}),
			m_components.end()
		);
	}
}
