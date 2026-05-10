/*****************************************************************//**
 * \file   Transform.cpp
 * \brief  トランスフォーム実装
 * 
 * \author tsunn
 * \date   2026/02/07
 *********************************************************************/
#include "Transform.h"

namespace Falu
{
	Transform::Transform()
		: m_position(0.0f,0.0f,0.0f)
		, m_rotation(0.0f,0.0f,0.0f)
		, m_scale(1.0f,1.0f,1.0f)
		, m_isDirty(true)
	{
		m_worldMatrix = DirectX::XMMatrixIdentity();
	}

	Transform::~Transform()
	{

	}

	void Transform::SetPosition(const Math::Vector3& position)
	{
		m_position = position;
		m_isDirty = true;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_position = Math::Vector3(x, y, z);
		m_isDirty = true;
	}

	void Transform::SetRotation(const Math::Vector3& rotation)
	{
		m_rotation = rotation;
		m_isDirty = true;
	}

	void Transform::SetRotation(float x, float y, float z)
	{
		m_rotation = Math::Vector3(x, y, z);
		m_isDirty = true;
	}

	void Transform::SetScale(const Math::Vector3& scale)
	{
		m_scale = scale;
		m_isDirty = true;
	}

	void Transform::SetScale(float x, float y, float z)
	{
		m_scale = Math::Vector3(x, y, z);
		m_isDirty = true;
	}

	void Transform::SetScale(float uniformScale)
	{
		m_scale = Math::Vector3(uniformScale, uniformScale, uniformScale);
		m_isDirty = true;
	}

	void Transform::Translate(const Math::Vector3& translation)
	{
		m_position.x += translation.x;
		m_position.y += translation.y;
		m_position.z += translation.z;
		m_isDirty = true;
	}

	void Transform::Rotate(const Math::Vector3& rotation)
	{
		m_rotation.x += rotation.x;
		m_rotation.y += rotation.y;
		m_rotation.z += rotation.z;
		m_isDirty = true;
	}

	DirectX::XMMATRIX Transform::GetWorldMatrix() const
	{
		if (m_isDirty)
		{
			const_cast<Transform*>(this)->UpdateMatrix();
		}
		return m_worldMatrix;
	}

	DirectX::XMMATRIX Transform::GetWorldMatrixTranspose() const
	{
		return DirectX::XMMatrixTranspose(GetWorldMatrix());
	}
	Math::Vector3 Transform::GetForward() const
	{
		return GetDirections().forward;
	}

	Math::Vector3 Transform::GetRight() const
	{
		return GetDirections().right;
	}

	Math::Vector3 Transform::GetUp() const
	{
		return GetDirections().up;
	}

	Transform::Directions Transform::GetDirections() const
	{
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
			m_rotation.x,m_rotation.y,m_rotation.z
		);

		DirectX::XMFLOAT3 f, r, u;

		DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(
			DirectX::XMVectorSet(0, 0, 1, 0), rotationMatrix);
		DirectX::XMVECTOR right = DirectX::XMVector3TransformNormal(
			DirectX::XMVectorSet(1, 0, 0, 0), rotationMatrix);
		DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(
			DirectX::XMVectorSet(0, 1, 0, 0), rotationMatrix);

		DirectX::XMStoreFloat3(&f, forward);
		DirectX::XMStoreFloat3(&r, right);
		DirectX::XMStoreFloat3(&u, up);
		return {
			Math::Vector3(f.x, f.y, f.z),
			Math::Vector3(r.x, r.y, r.z),
			Math::Vector3(u.x, u.y, u.z)
		};
	}

	void Transform::UpdateMatrix()
	{
		using namespace DirectX;

		XMMATRIX scaleMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
		m_isDirty = false;
	}
}
