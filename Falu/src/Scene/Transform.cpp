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

	void Transform::SetScale(Math::Vector3& scale)
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
		using namespace DirectX;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			m_rotation.x,
			m_rotation.y,
			m_rotation.z
		);

		XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		forward = XMVector3TransformNormal(forward, rotationMatrix);

		XMFLOAT3 result;
		XMStoreFloat3(&result, forward);

		return Math::Vector3(result.x, result.y, result.z);
	}

	Math::Vector3 Transform::GetRight() const
	{
		using namespace DirectX;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			m_rotation.x,
			m_rotation.y,
			m_rotation.z
		);

		XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		right = XMVector3TransformNormal(right, rotationMatrix);

		XMFLOAT3 result;
		XMStoreFloat3(&result, right);

		return Math::Vector3(result.x, result.y, result.z);
	}

	Math::Vector3 Transform::GetUp() const
	{
		using namespace DirectX;

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			m_rotation.x,
			m_rotation.y,
			m_rotation.z
		);

		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		up = XMVector3TransformNormal(up, rotationMatrix);

		XMFLOAT3 result;
		XMStoreFloat3(&result, up);

		return Math::Vector3(result.x, result.y, result.z);
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
