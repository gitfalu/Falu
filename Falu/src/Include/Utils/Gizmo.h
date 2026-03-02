/*****************************************************************//**
 * \file   Gizmo.h
 * \brief  ÉMÉYÉÇçÏê¨
 * 
 * \author tsunn
 * \date   2026/02/26
 *********************************************************************/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include "Include/Math/MathHelper.h"
#include "Include/Math/Ray.h"

namespace Falu
{
	using Microsoft::WRL::ComPtr;

	class GameObject;
	class Mesh;
	class Shader;
	class Camera;

	// Gizmo Control mode
	enum class GizmoMode
	{
		Translate,
		Rotate,
		Scale
	};

	// Gizmo coordinate
	enum class GizmoSpace
	{
		World,
		Local
	};

	enum class GizmoAxis
	{
		None = 0,
		X = 1,
		Y = 2,
		Z = 4,
		XY =  X | Y,
		YZ =  Y | Z,
		XZ =  X | Z,
		XYZ = X | Y | Z
	};

	class Gizmo
	{
	public:
		Gizmo();
		~Gizmo();

		bool Initialize(ID3D11Device* device);
		void Render(ID3D11DeviceContext* context, Camera* camera, GameObject* target);

		// Change Mode
		void SetMode(GizmoMode mode) { m_mode = mode; }
		GizmoMode GetMode() const { return m_mode; }

		// 
		void SetSpace(GizmoSpace space) { m_space = space; }
		GizmoSpace GetSpace() const { return m_space; }

		// Mouse Event
		void OnMouseDown(const Math::Ray& ray, GameObject* target, Camera* camera);
		void OnMouseDrag(const Math::Ray& ray, GameObject* target, Camera* camera);
		void OnMouseUp();

		// 
		GizmoAxis GetHoveredAxis(const Math::Ray& ray, GameObject* target, Camera* camera) const;

		void SetSize(float size) { m_size = size; }
		float GetSize() { return m_size; }

		void SetShader(Shader* shader) { m_shader = shader; }
		Shader* GetShader() const { return m_shader; }

	private:
		GizmoMode m_mode;
		GizmoSpace m_space;
		GizmoAxis m_selectedAxis;
		float m_size;

		// Start Drag info
		bool m_isDragging;
		Math::Vector3 m_dragStartPosition;
		Math::Vector3 m_dragStartRotation;
		Math::Vector3 m_dragStartScale;
		Math::Vector3 m_dragStartMouseWorld;
		Math::Vector3 m_dragPlaneNormal;
		// Mesh
		std::unique_ptr<Mesh> m_arrowMesh;
		std::unique_ptr<Mesh> m_circleMesh;
		std::unique_ptr<Mesh> m_cubeMesh;
		// Shader
		Shader* m_shader;

	private:
		// ConatantBuffer
		struct GizmoConstantBuffer
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4X4 viewProjection;
			DirectX::XMFLOAT4 color;
		};
		ComPtr<ID3D11Buffer> m_constantBuffer;

		void RenderTranslateGizmo(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const DirectX::XMMATRIX& rotation);
		void RenderRotationGizmo(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const DirectX::XMMATRIX& rotation);
		void RenderScaleGizmo(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const DirectX::XMMATRIX& rotation);

		void RenderAxis(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const Math::Vector3& direction,
			const Math::Color& color,float length);

		void RenderCircle(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const Math::Vector3& normal,
			const Math::Color& color, float radius);

		void RenderScaleAxis(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, const Math::Vector3& direction,
			const Math::Color& color, float length);

		void RenderLine(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& start, const Math::Vector3& end,
			const Math::Color& color, float thickness);

		void RenderCube(ID3D11DeviceContext* context, Camera* camera,
			const Math::Vector3& position, 
			const Math::Color& color, float size);

		// Ray Cast
		bool RayIntersectsAxis(const Math::Ray& ray, const Math::Vector3& origin,
			const Math::Vector3& direction, float length,float threshold,
			float& distance) const;

		bool RayInteersectsCircle(const Math::Ray& ray, const Math::Vector3& center,
			const Math::Vector3& normal, float radius,float threshold,
			float& distance) const;

		// calc mouse position from world coordinate
		Math::Vector3 GetMouseWorldPosition(const Math::Ray& ray, const Math::Vector3& planeNormal,
			const Math::Vector3& planePoint) const;

		float GetAdjustedSize(Camera* camera, const Math::Vector3& position) const;
	};
}
