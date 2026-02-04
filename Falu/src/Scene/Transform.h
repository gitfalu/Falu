/*****************************************************************//**
 * \file   Transform.h
 * \brief  トランスフォームコンポーネント
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/

#pragma once

#include "../Include/MathHelper.h"

namespace Falu
{
	class Transform
	{
	public:
		Transform();
		~Transform();

		//=== Position ===
		void SetPosition(const Math::Vector3& position);
		void SetPosition(float x, float y, float z);
		Math::Vector3 GetPosition()const { return m_position; }

		//=== Rotation === 
		void SetRotation(const Math::Vector3& rotation);
		void SetRotation(float x, float y, float z);
		Math::Vector3 GetRotation() const { return m_rotation;}

		//=== Scale ===
		void SetScale(Math::Vector3& scale);
		void SetScale(float x, float y, float z);
		void SetScale(float uniformScale);
		Math::Vector3 GetScale()const { return m_scale; }

		//=== Transform operations === 
		void Translate(const Math::Vector3& translation);
		void Rotate(const Math::Vector3& rotation);

		//=== Matrix === 
		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetWorldMatrixTranspose() const;

		//=== Directions vectors ===
		Math::Vector3 GetForward() const;
		Math::Vector3 GetRight() const;
		Math::Vector3 GetUp() const;

	private:
		void UpdateMatrix();

	private:
		Math::Vector3 m_position;
		Math::Vector3 m_rotation; //オイラー
		Math::Vector3 m_scale;

		mutable DirectX::XMMATRIX m_worldMatrix;
		mutable bool m_isDirty;
	};
}
