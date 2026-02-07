/*****************************************************************//**
 * \file   GameObject.cpp
 * \brief  ゲームオブジェクト実装
 * 
 * \author tsunn
 * \date   2026/02/07
 *********************************************************************/
#include "GameObject.h"

namespace Falu
{
	GameObject::GameObject(const std::string& name)
		: m_name(name)
		, m_isActive(true)
		, m_parent(nullptr)
	{

	}

	GameObject::~GameObject()
	{
		// 子オブジェクトのクリア
		for (auto child : m_children)
		{
			if (child)
			{
				child->m_parent = nullptr;
			}
		}
		m_children.clear();

		// コンポーネントのクリア
		m_components.clear();
	}

	void GameObject::Update(float deltaTime)
	{
		if (!m_isActive)
			return;

		// コンポーネントの更新
		for (auto& component : m_components)
		{
			if (component && component->IsEnabled())
			{
				component->Update(deltaTime);
			}
		}

		// 子オブジェクトの更新
		for (auto child : m_children)
		{
			if (child && child->IsActive())
			{
				child->Update(deltaTime);
			}
		}
	}

	void GameObject::Render()
	{
		if (!IsActive())
			return;

		// コンポーネントの描画
		for (auto& component : m_components)
		{
			if (component && component->IsEnabled())
			{
				component->Render();
			}
		}

		// 子オブジェクトの描画
		for (auto child : m_children)
		{
			if (child && child->IsActive())
			{
				child->Render();
			}
		}
	}

	void GameObject::SetParent(GameObject* parent)
	{
		// 既存の親から削除
		if (m_parent)
		{
			m_parent->RemoveChild(this);
		}

		m_parent = parent;

		// 新しい親に追加
		if (m_parent)
		{
			m_parent->AddChild(this);
		}
	}

	void GameObject::AddChild(GameObject* child)
	{
		if (!child)
			return;

		// すでに親として登録されているか確認
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it == m_children.end())
		{
			m_children.push_back(child);
			child->m_parent = this;
		}
	}

	void GameObject::RemoveChild(GameObject* child)
	{
		if (!child)
			return;

		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			(*it)->m_parent = nullptr;
			m_children.erase(it);
		}
	}
}
