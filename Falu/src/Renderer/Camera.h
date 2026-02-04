/*****************************************************************//**
 * \file   Camera.h
 * \brief  ÉJÉÅÉâ
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <DirectXMath.h>
#include "../Include/MathHelper.h"
#include "../Scene/Transform.h"

namespace Falu
{
	enum class ProjectionType
	{
		Perspective,
		Orthographic
	};

	class Camera
	{
	public:
		Camera();
		~Camera();

		//=== Transform ===
		Transform& GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }

		//=== Projection settings ===
		void SetPerspective(float fov, float aspectRatio, float nearZ, float farZ);
		void SetOrthographic(float width,float height,float nearZ,float farZ);

		//=== Matrices ===
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }
		DirectX::XMMATRIX GEtViewProjectionMatrix() const;

		//=== Camera controls ===
		void LookAt(const Math::Vector3& target);
		void LookAt(const Math::Vector3& position, const Math::Vector3& target, const Math::Vector3& up);

		//=== Getters ===
		float GetFOV() const { return m_fov; }
		float GetNearZ() const { return m_nearZ; }
		float GetFarZ()const { return m_farZ; }
		ProjectionType GetProjectionType() const { return m_projectionType; }

		//=== Movement helpers ===
		void MoveForward(float distance);
		void MoveRight(float distance);
		void MoveUp(float distance);

	private:
		void UpdateProjectionMatrix();

	private:
		Transform m_transform;
		DirectX::XMMATRIX m_projectionMatrix;

		ProjectionType m_projectionType;
		float m_fov;
		float m_aspectRatio;
		float m_nearZ;
		float m_farZ;
		float m_orthoWidth;
		float m_orthoHeight;
	};
}
