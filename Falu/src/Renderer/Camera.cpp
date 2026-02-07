/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  ÉJÉÅÉâé¿ëï
 * 
 * \author tsunn
 * \date   2026/02/06
 *********************************************************************/
#include "Camera.h"

namespace Falu
{
	Camera::Camera()
		:m_projectionType(ProjectionType::Perspective)
		,m_fov(Math::ToRadians(60.0f))
		,m_aspectRatio(16.0f/9.0f)
		,m_nearZ(0.1f)
		,m_farZ(100.0f)
		,m_orthoWidth(10.0f)
		,m_orthoHeight(10.0f)
	{
		m_transform.SetPosition(0.0f, 0.0f, -5.0f);
		UpdateProjectionMatrix();
	}

	Camera::~Camera()
	{

	}

	void Camera::SetPerspective(float fov, float aspectRatio, float nearZ, float farZ)
	{
		m_projectionType = ProjectionType::Perspective;
		m_fov = fov;
		m_aspectRatio = aspectRatio;
		m_nearZ = nearZ;
		m_farZ = farZ;
		UpdateProjectionMatrix();
	}

	void Camera::SetOrthographic(float width, float height, float nearZ, float farZ)
	{
		m_projectionType = ProjectionType::Orthographic;
		m_orthoWidth = width;
		m_orthoHeight = height;
		m_nearZ = nearZ;
		m_farZ = farZ;
		UpdateProjectionMatrix();
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
		if (m_projectionType == ProjectionType::Perspective)
		{
			UpdateProjectionMatrix();
		}
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		using namespace DirectX;

		Math::Vector3 pos = m_transform.GetPosition();
		Math::Vector3 forward = m_transform.GetForward();
		Math::Vector3 up = m_transform.GetUp();

		XMVECTOR eyePosition = XMVectorSet(pos.x, pos.y, pos.z,1.0f);
		XMVECTOR focusPosition = XMVectorSet(
			pos.x + forward.x,
			pos.y + forward.y,
			pos.z + forward.z,
			1.0f
		);

		XMVECTOR upDirection = XMVectorSet(up.x, up.y, up.z, 0.0f);
		return XMMatrixLookAtLH(eyePosition,focusPosition,upDirection);
	}

	DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
	{
		return GetViewMatrix() * m_projectionMatrix;
	}

	void Camera::LookAt(const Math::Vector3& target)
	{
		using namespace DirectX;

		Math::Vector3 pos = m_transform.GetPosition();

		XMVECTOR eyePos = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
		XMVECTOR targetPos = XMVectorSet(target.x, target.y, target.z, 1.0f);
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMVECTOR forward = XMVector3Normalize(targetPos - eyePos);
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));
		XMVECTOR newUp = XMVector3Cross(forward, right);

		// âÒì]ÇÃåvéZ
		XMFLOAT3 forwardFloat, rightFloat, upFloat;
		XMStoreFloat3(&forwardFloat, forward);
		XMStoreFloat3(&rightFloat, right);
		XMStoreFloat3(&upFloat, newUp);

		float pitch = asinf(-forwardFloat.y);
		float yaw = atan2f(forwardFloat.x, forwardFloat.z);
		float roll = 0.0f;

		m_transform.SetRotation(pitch, yaw, roll);

	}

	void Camera::LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up)
	{
		m_transform.SetPosition(position);
		LookAt(target);
	}

	void Camera::MoveForward(float distance)
	{
		Math::Vector3 forward = m_transform.GetForward();
		m_transform.Translate(Math::Vector3(
			forward.x * distance,
			forward.y * distance,
			forward.z * distance
		));
	}

	void Camera::MoveRight(float distance)
	{
		Math::Vector3 right = m_transform.GetRight();
		m_transform.Translate(Math::Vector3(
			right.x * distance,
			right.y * distance,
			right.z * distance
		));
	}

	void Camera::MoveUp(float distance)
	{
		Math::Vector3 up = m_transform.GetUp();
		m_transform.Translate(Math::Vector3(
			up.x * distance,
			up.y * distance,
			up.z * distance
		));
	}

	void Camera::UpdateProjectionMatrix()
	{
		using namespace DirectX;

		if (m_projectionType == ProjectionType::Perspective)
		{
			m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearZ, m_farZ);
		}
		else
		{
			m_projectionMatrix = XMMatrixOrthographicLH(m_orthoWidth, m_orthoHeight, m_nearZ, m_farZ);
		}
	}
}
