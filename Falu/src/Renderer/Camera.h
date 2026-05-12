/*****************************************************************//**
 * \file   Camera.h
 * \brief  カメラ
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <DirectXMath.h>
#include "Include/Math/MathHelper.h"
#include "Include/Math/Ray.h"
#include "Scene/Transform.h"

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
		void SetAspectRatio(float aspectRatio);

		//=== Matrices ===
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }
		DirectX::XMMATRIX GetViewProjectionMatrix() const;

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

		//=== マウス回転・ズーム ===
		void Rotate(float deltaPithch, float deltaYaw);
		void Zoom(float delta);

		//=== 感度設定 ===
		void SetRotateSensitivity(float sensitivity) { m_rotateSensitivity = sensitivity; }
		void SetZoomSensitivity(float sensitivity) { m_zoomSensitivity = sensitivity; }

		//=== Create Ray from Screen ===
		Math::Ray ScreenPointToRay(float screenX, float screenY, float screenWidth, float screenHeight)const;

	private:
		void UpdateProjectionMatrix();

	private:
		Transform m_transform;
		DirectX::XMMATRIX m_projectionMatrix;

		//=== 感度パラメータ ===
		float m_rotateSensitivity = 0.003f;
		float m_zoomSensitivity = 0.5f;

		//=== ジンバルロック防止 ===
		float m_pitchMin = Math::ToRadians(-89.0f);
		float m_pitchMax = Math::ToRadians(89.0f);

		ProjectionType m_projectionType;
		float m_fov;
		float m_aspectRatio;
		float m_nearZ;
		float m_farZ;
		float m_orthoWidth;
		float m_orthoHeight;
	};
}
