/*****************************************************************//**
 * \file   Gizmo.cpp
 * \brief  ƒMƒYƒ‚ŽÀ‘•
 * 
 * \author tsunn
 * \date   2026/02/26
 *********************************************************************/
#include "Gizmo.h"
#include "Scene/GameObject.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

namespace Falu
{
	Gizmo::Gizmo()
		: m_mode(GizmoMode::Translate)
		, m_space(GizmoSpace::World)
		, m_selectedAxis(GizmoAxis::None)
		, m_size(1.0f)
		, m_isDragging(false)
		, m_shader(nullptr)
	{
	}

	Gizmo::~Gizmo()
	{
	}

	bool Gizmo::Initialize(ID3D11Device* device)
	{
		// Create Arrow mesh
		m_arrowMesh = Mesh::CreateCylinder(device, 16);
		if(!m_arrowMesh)
			return false;

		// Craete Circle mesh
		m_circleMesh = Mesh::CreateSphere(device, 32);
		if (!m_circleMesh)
			return false;

		// Create Cube Mesh
		m_cubeMesh = Mesh::CreateCube(device);
		if (!m_cubeMesh)
			return false;

		// Create Constant Buffer
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(GizmoConstantBuffer);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
		return SUCCEEDED(hr);
	}

	void Gizmo::Render(ID3D11DeviceContext* context, Camera* camera, GameObject* target)
	{
		if (!target || !camera)
			return;

		Math::Vector3 position = target->GetTransform().GetPosition();

		DirectX::XMMATRIX rotation;
		if (m_space == GizmoSpace::Local)
		{
			rotation = DirectX::XMMatrixRotationRollPitchYaw(
				target->GetTransform().GetRotation().x,
				target->GetTransform().GetRotation().y,
				target->GetTransform().GetRotation().z
			);
		}
		else
		{
			rotation = DirectX::XMMatrixIdentity();
		}

		switch (m_mode)
		{
		case Falu::GizmoMode::Translate:
			RenderTranslateGizmo(context, camera, position, rotation);
			break;
		case Falu::GizmoMode::Rotate:
			RenderRotationGizmo(context, camera, position, rotation);
			break;
		case Falu::GizmoMode::Scale:
			RenderScaleGizmo(context, camera, position, rotation);
			break;
		}
	}

	void Gizmo::OnMouseDown(const Math::Ray& ray, GameObject* target, Camera* camera)
	{
		if (!target)
			return;

		m_selectedAxis = GetHoveredAxis(ray, target,camera);

		if (m_selectedAxis != GizmoAxis::None)
		{
			m_isDragging = true;
			m_dragStartPosition = target->GetTransform().GetPosition();
			m_dragStartRotation = target->GetTransform().GetRotation();
			m_dragStartScale = target->GetTransform().GetScale();

			Math::Vector3 camDir = camera->GetTransform().GetForward();

			switch (m_selectedAxis)
			{
			case Falu::GizmoAxis::X:
				m_dragPlaneNormal = Math::Vector3(1, 0, 0);
				break;
			case Falu::GizmoAxis::Y:
				m_dragPlaneNormal = Math::Vector3(0, 1, 0);
				break;
			case Falu::GizmoAxis::Z:
				m_dragPlaneNormal = Math::Vector3(0, 0, 1);
				break;
			default:
				m_dragPlaneNormal = Math::Vector3(0, 1, 0);
				break;
			}

			m_dragStartMouseWorld = GetMouseWorldPosition(ray, m_dragPlaneNormal, m_dragStartPosition);

			char msg[256];
			sprintf_s(msg, "[Gizmo] Drag started on axis: %d\n", (int)m_selectedAxis);
			OutputDebugStringA(msg);
		}
	}

	void Gizmo::OnMouseDrag(const Math::Ray& ray, GameObject* target, Camera* camera)
	{
		if (!m_isDragging || !target || m_selectedAxis == GizmoAxis::None)
			return;

		Math::Vector3 currentMouseWorld = GetMouseWorldPosition(ray, m_dragPlaneNormal, m_dragStartPosition);

		switch (m_mode)
		{
		case Falu::GizmoMode::Translate:
		{
			Math::Vector3 delta = Math::Vector3(
				currentMouseWorld.x - m_dragStartMouseWorld.x,
				currentMouseWorld.y - m_dragStartMouseWorld.y,
				currentMouseWorld.z - m_dragStartMouseWorld.z
			);

			Math::Vector3 newPosition = m_dragStartPosition;

			switch (m_selectedAxis)
			{
			case Falu::GizmoAxis::X:
				newPosition.x = m_dragStartPosition.x + delta.x;
				break;
			case Falu::GizmoAxis::Y:
				newPosition.y = m_dragStartPosition.y + delta.y;
				break;
			case Falu::GizmoAxis::Z:
				newPosition.z = m_dragStartPosition.z + delta.z;
				break;
			}
			target->GetTransform().SetPosition(newPosition);
		}
		break;
		case Falu::GizmoMode::Rotate:
		{
			Math::Vector3 startVec = Math::Vector3(
				m_dragStartMouseWorld.x - m_dragStartPosition.x,
				m_dragStartMouseWorld.y - m_dragStartPosition.y,
				m_dragStartMouseWorld.z - m_dragStartPosition.z
			);

			Math::Vector3 currentVec = Math::Vector3(
				currentMouseWorld.x - m_dragStartPosition.x,
				currentMouseWorld.y - m_dragStartPosition.y,
				currentMouseWorld.z - m_dragStartPosition.z
			);

			float startLen = sqrtf(startVec.x * startVec.x + startVec.y * startVec.y + startVec.z * startVec.z);
			float currentLen = sqrtf(currentVec.x * currentVec.x + currentVec.y * currentVec.y + currentVec.z * currentVec.z);

			if (startLen < 0.0001f || currentLen < 0.0001f)
				break;

			startVec.x /= startLen;
			startVec.y /= startLen;
			startVec.z /= startLen;

			currentVec.x /= currentLen;
			currentVec.y /= currentLen;
			currentVec.z /= currentLen;

			float dot = startVec.x * currentVec.x + startVec.y * currentVec.y + startVec.z * currentVec.z;
			dot = std::max(-1.0f, std::min(1.0f, dot));
			float angle = acosf(dot);

			Math::Vector3 cross = Math::Vector3(
				startVec.y * currentVec.z - startVec.z * currentVec.y,
				startVec.z * currentVec.x - startVec.x * currentVec.z,
				startVec.x * currentVec.y - startVec.y * currentVec.x
			);

			float sign = 1.0f;
			switch (m_selectedAxis)
			{
			case Falu::GizmoAxis::X:
				sign = (cross.x < 0) ? -1.0f : 1.0f;
				break;
			case Falu::GizmoAxis::Y:
				sign = (cross.y < 0) ? -1.0f : 1.0f;
				break;
			case Falu::GizmoAxis::Z:
				sign = (cross.z < 0) ? -1.0f : 1.0f;
				break;
			}
			angle *= sign;

			Math::Vector3 newRotation = m_dragStartRotation;

			switch (m_selectedAxis)
			{
			case Falu::GizmoAxis::X:
				newRotation.x = m_dragStartRotation.x + angle;
				break;
			case Falu::GizmoAxis::Y:
				newRotation.y = m_dragStartRotation.y + angle;
				break;
			case Falu::GizmoAxis::Z:
				newRotation.z = m_dragStartRotation.z + angle;
				break;
			}
			target->GetTransform().SetRotation(newRotation);
		}
			break;
		case Falu::GizmoMode::Scale:
		{
			Math::Vector3 delta = Math::Vector3(
				currentMouseWorld.x - m_dragStartMouseWorld.x,
				currentMouseWorld.y - m_dragStartMouseWorld.y,
				currentMouseWorld.z - m_dragStartMouseWorld.z
			);

			float deltaLength = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

			Math::Vector3 dragDir = Math::Vector3(
				currentMouseWorld.x - m_dragStartPosition.x,
				currentMouseWorld.y - m_dragStartPosition.y,
				currentMouseWorld.z - m_dragStartPosition.z
			);

			Math::Vector3 startDir = Math::Vector3(
				m_dragStartMouseWorld.x - m_dragStartPosition.x,
				m_dragStartMouseWorld.y - m_dragStartPosition.y,
				m_dragStartMouseWorld.z - m_dragStartPosition.z
			);

			float dot = dragDir.x * startDir.x + dragDir.y * startDir.y + dragDir.z * startDir.z;
			float scaleFactor = (dot > 0) ? (1.0f + deltaLength * 0.1f) : (1.0f - deltaLength * 0.1f);
			scaleFactor = std::max(0.01f, scaleFactor);

			Math::Vector3 newScale = m_dragStartScale;

			switch (m_selectedAxis)
			{
			case Falu::GizmoAxis::X:
				newScale.x = m_dragStartScale.x * scaleFactor;
				break;
			case Falu::GizmoAxis::Y:
				newScale.y = m_dragStartScale.y * scaleFactor;
				break;
			case Falu::GizmoAxis::Z:
				newScale.z = m_dragStartScale.z * scaleFactor;
				break;
			case Falu::GizmoAxis::XYZ:
				newScale.x = m_dragStartScale.x * scaleFactor;
				newScale.y = m_dragStartScale.y * scaleFactor;
				newScale.z = m_dragStartScale.z * scaleFactor;
				break;
			}

			target->GetTransform().SetScale(newScale);
		}
			break;
		}
	}

	void Gizmo::OnMouseUp()
	{
		if (m_isDragging)
		{
			OutputDebugStringA("[Gizmo] Drag ended\n");
		}

		m_isDragging = false;
		m_selectedAxis = GizmoAxis::None;
	}

	GizmoAxis Gizmo::GetHoveredAxis(const Math::Ray& ray, GameObject* target, Camera* camera) const
	{
		if (!target || !camera)
			return GizmoAxis::None;

		Math::Vector3 position = target->GetTransform().GetPosition();
		float adjustedSize = GetAdjustedSize(camera, position);
		float minDistance = FLT_MAX;
		GizmoAxis closestAxis = GizmoAxis::None;
		float threshold = adjustedSize * 0.1f;

		switch (m_mode)
		{
		case Falu::GizmoMode::Translate:
		{
			float distX;
			if (RayIntersectsAxis(ray, position, Math::Vector3(1, 0, 0), adjustedSize, threshold, distX))
			{
				if(distX < minDistance)
				{
					minDistance = distX;
					closestAxis = GizmoAxis::X;
				}
			}

			float distY;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 1, 0), adjustedSize, threshold, distY))
			{
				if (distY < minDistance)
				{
					minDistance = distY;
					closestAxis = GizmoAxis::Y;
				}
			}

			float distZ;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 0, 1), adjustedSize, threshold, distZ))
			{
				if (distZ < minDistance)
				{
					minDistance = distZ;
					closestAxis = GizmoAxis::Z;
				}
			}
		}
			break;
		case Falu::GizmoMode::Rotate:
		{
			float distX;
			if (RayIntersectsAxis(ray, position, Math::Vector3(1, 0, 0), adjustedSize, threshold, distX))
			{
				if (distX < minDistance)
				{
					minDistance = distX;
					closestAxis = GizmoAxis::X;
				}
			}
			float distY;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 1, 0), adjustedSize, threshold, distY))
			{
				if (distY < minDistance)
				{
					minDistance = distY;
					closestAxis = GizmoAxis::Y;
				}
			}

			float distZ;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 0, 1), adjustedSize, threshold, distZ))
			{
				if (distZ < minDistance)
				{
					minDistance = distZ;
					closestAxis = GizmoAxis::Z;
				}
			}
		}
			break;
		case Falu::GizmoMode::Scale:
		{
			float distX;
			if (RayIntersectsAxis(ray, position, Math::Vector3(1, 0, 0), adjustedSize, threshold, distX))
			{
				if (distX < minDistance)
				{
					minDistance = distX;
					closestAxis = GizmoAxis::X;
				}
			}

			float distY;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 1, 0), adjustedSize, threshold, distY))
			{
				if (distY < minDistance)
				{
					minDistance = distY;
					closestAxis = GizmoAxis::Y;
				}
			}

			float distZ;
			if (RayIntersectsAxis(ray, position, Math::Vector3(0, 0, 1), adjustedSize, threshold, distZ))
			{
				if (distZ < minDistance)
				{
					minDistance = distZ;
					closestAxis = GizmoAxis::Z;
				}
			}
			Math::AABB centerBox(
				Math::Vector3(position.x - threshold, position.y - threshold, position.z - threshold),
				Math::Vector3(position.x + threshold, position.y + threshold, position.z + threshold)
			);

			float tMin, tMax;
			if (centerBox.IntersectsRay(ray, tMin, tMax))
			{
				if (tMin < minDistance)
				{
					minDistance = tMin;
					closestAxis = GizmoAxis::XYZ;
				}
			}
		}
			break;
		}

		return closestAxis;
	}

	void Gizmo::RenderTranslateGizmo(ID3D11DeviceContext* context, Camera* camera, 
		const Math::Vector3& position, const DirectX::XMMATRIX& rotation)
	{
		using namespace DirectX;

		Math::Vector3 camPos = camera->GetTransform().GetPosition();
		float distance = sqrtf(
			(position.x - camPos.x) * (position.x - camPos.x) +
			(position.y - camPos.y) * (position.y - camPos.y) +
			(position.z - camPos.z) * (position.z - camPos.z)
		);
		float adjustedSize = m_size * distance * 0.1f;

		// X red
		Math::Color xColor = (m_selectedAxis == GizmoAxis::X) ? Math::Color(1, 1, 0, 1) : Math::Color(1, 0, 0, 1);
		XMVECTOR xDir = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotation);
		XMFLOAT3 xDirFloat;
		XMStoreFloat3(&xDirFloat, xDir);
		RenderAxis(context, camera, position, Math::Vector3(xDirFloat.x, xDirFloat.y, xDirFloat.z),
			xColor, adjustedSize);

		// Y green
		Math::Color yColor = (m_selectedAxis == GizmoAxis::Y) ? Math::Color(1, 1, 0, 1) : Math::Color(0, 1, 0, 1);
		XMVECTOR yDir = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
		XMFLOAT3 yDirFloat;
		XMStoreFloat3(&yDirFloat, yDir);
		RenderAxis(context, camera, position, Math::Vector3(yDirFloat.x, yDirFloat.y, yDirFloat.z),
			yColor, adjustedSize);

		// Z blue
		Math::Color zColor = (m_selectedAxis == GizmoAxis::Z) ? Math::Color(1, 1, 0, 1) : Math::Color(0, 0, 1, 1);
		XMVECTOR zDir = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotation);
		XMFLOAT3 zDirFloat;
		XMStoreFloat3(&zDirFloat, zDir);
		RenderAxis(context, camera, position, Math::Vector3(zDirFloat.x, zDirFloat.y, zDirFloat.z),
			zColor, adjustedSize);
	}

	void Gizmo::RenderRotationGizmo(ID3D11DeviceContext* context, Camera* camera, 
		const Math::Vector3& position, const DirectX::XMMATRIX& rotation)
	{

	}

	void Gizmo::RenderScaleGizmo(ID3D11DeviceContext* context, Camera* camera, 
		const Math::Vector3& position, const DirectX::XMMATRIX& rotation)
	{

	}

	void Gizmo::RenderAxis(ID3D11DeviceContext* context, Camera* camera, 
		const Math::Vector3& position, const Math::Vector3& direction, const Math::Color& color, float length)
	{
		using namespace DirectX;

		if (!m_arrowMesh || !m_shader)
			return;

		XMVECTOR forward = XMVectorSet(direction.x, direction.y, direction.z,0);
		forward = XMVector3Normalize(forward);

		XMFLOAT3 forwardFloat;
		XMStoreFloat3(&forwardFloat, forward);

		XMMATRIX rotationMatrix;
		XMVECTOR defaultUp = XMVectorSet(0, 1, 0, 0);

		float dot = XMVectorGetX(XMVector3Dot(defaultUp, forward));
		
		if (fabs(dot) < 0.9999f)
		{
			XMVECTOR rotationAxis = XMVector3Cross(defaultUp, forward);
			rotationAxis = XMVector3Normalize(rotationAxis);

			float angle = acosf(dot);

			rotationMatrix = XMMatrixRotationAxis(rotationAxis, angle);
		}
		else
		{
			if (dot > 0)
			{
				rotationMatrix = XMMatrixIdentity();
			}
			else
			{
				rotationMatrix = XMMatrixRotationX(Math::PI);
			}
		}

		XMVECTOR offsetVector = XMVectorSet(0, length * 0.5f, 0, 0);
		offsetVector = XMVector3Transform(offsetVector, rotationMatrix);

		XMFLOAT3 offset;
		XMStoreFloat3(&offset, offsetVector);

		XMMATRIX scaleMatrix = XMMatrixScaling(0.05f, length, 0.05f);
		Math::Vector3 finalPos = Math::Vector3(
			position.x + offset.x,
			position.y + offset.y,
			position.z + offset.z
		);
		XMMATRIX translationMatrix = XMMatrixTranslation(finalPos.x, finalPos.y, finalPos.z);

		XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		GizmoConstantBuffer cb;
		XMStoreFloat4x4(&cb.world, XMMatrixTranspose(worldMatrix));
		XMStoreFloat4x4(&cb.viewProjection,
			XMMatrixTranspose(camera->GetViewProjectionMatrix()));
		cb.color = XMFLOAT4(color.r, color.g, color.b, color.a);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map(m_constantBuffer.Get(), 
			0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr))
		{
			memcpy(mappedResource.pData, &cb, sizeof(GizmoConstantBuffer));
			context->Unmap(m_constantBuffer.Get(), 0);
		}

		context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

		m_shader->Bind(context);
		m_arrowMesh->Render(context);
	}

	void Gizmo::RenderCircle(ID3D11DeviceContext* context, Camera* camera, const Math::Vector3& position, const Math::Vector3& normal, const Math::Color& color, float radius)
	{
	}

	void Gizmo::RenderScaleAxis(ID3D11DeviceContext* context, Camera* camera, const Math::Vector3& position, const Math::Vector3& direction, const Math::Color& color, float length)
	{
	}

	void Gizmo::RenderLine(ID3D11DeviceContext* context, Camera* camera, const Math::Vector3& start, const Math::Vector3& end, const Math::Color& color, float thickness)
	{
	}

	void Gizmo::RenderCube(ID3D11DeviceContext* context, Camera* camera, const Math::Vector3& position, const Math::Color& color, float size)
	{
	}

	bool Gizmo::RayIntersectsAxis(const Math::Ray& ray, const Math::Vector3& origin, 
		const Math::Vector3& direction, float length, float threshold, float& distance) const
	{
		Math::Vector3 axisEnd = Math::Vector3(
			origin.x + direction.x * length,
			origin.y + direction.y * length,
			origin.z + direction.z * length
		);

		Math::Vector3 w0 = Math::Vector3(
			ray.origin.x - origin.x,
			ray.origin.y - origin.y,
			ray.origin.z - origin.z
		);

		float a = 
			ray.direction.x * ray.direction.x +
			ray.direction.y * ray.direction.y +
			ray.direction.z * ray.direction.z;

		float b = 
			ray.direction.x * direction.x +
			ray.direction.y * direction.y +
			ray.direction.z * direction.z;

		float c = 
			direction.x * direction.x +
			direction.y * direction.y +
			direction.z * direction.z;

		float d = 
			ray.direction.x * w0.x +
			ray.direction.y * w0.y +
			ray.direction.z * w0.z;

		float e = 
			direction.x * w0.x +
			direction.y * w0.y +
			direction.z * w0.z;

		float denom = a * c - b * b;

		if (fabs(denom) < 0.0001f)
			return false;

		float sc = (b * e - c * d) / denom;
		float tc = (a * e - b * d) / denom;

		if (tc < 0.0f || tc > length)
			return false;

		// Calc Nearest Distance
		Math::Vector3 closestOnRay = ray.GetPoint(sc);
		Math::Vector3 closestOnAxis = Math::Vector3(
			origin.x + direction.x * tc,
			origin.y + direction.y * tc,
			origin.z + direction.z * tc
		);

		float dx = closestOnRay.x - closestOnAxis.x;
		float dy = closestOnRay.y - closestOnAxis.y;
		float dz = closestOnRay.z - closestOnAxis.z;
		float dist = sqrt(dx * dx + dy * dy + dz * dz);

		if (dist <= threshold)
		{
			distance = sc;
			return true;
		}

		return false;
	}

	bool Gizmo::RayInteersectsCircle(const Math::Ray& ray, const Math::Vector3& center, const Math::Vector3& normal, float radius, float threshold, float& distance) const
	{
		float denom =
			normal.x * ray.direction.x +
			normal.y * ray.direction.y +
			normal.z * ray.direction.z;

		if (fabs(denom) < 0.0001f)
			return false;

		Math::Vector3 diff = Math::Vector3(
			center.x - ray.origin.x,
			center.y - ray.origin.y,
			center.z - ray.origin.z
		);

		float t = (normal.x * diff.x + normal.y * diff.y + normal.z * diff.z) / denom;

		if (t < 0)
			return false;

		Math::Vector3 intersect = ray.GetPoint(t);

		float dx = intersect.x - center.x;
		float dy = intersect.y - center.y;
		float dz = intersect.z - center.z;
		float dist = sqrt(dx * dx + dy * dy + dz * dz);

		if (dist >= radius - threshold && dist <= radius + threshold)
		{
			distance = t;
			return true;
		}

		return false;
	}

	Math::Vector3 Gizmo::GetMouseWorldPosition(const Math::Ray& ray, 
		const Math::Vector3& planeNormal, const Math::Vector3& planePoint) const
	{
		float denom = planeNormal.x * ray.direction.x +
			planeNormal.y * ray.direction.y +
			planeNormal.z * ray.direction.z;

		if (fabs(denom) < 0.0001f)
		{
			return planePoint;
		}

		Math::Vector3 diff = Math::Vector3(
			planePoint.x - ray.origin.x,
			planePoint.y - ray.origin.y,
			planePoint.z - ray.origin.z
		);

		float t = (planeNormal.x * diff.x + planeNormal.y * diff.y + planeNormal.z * diff.z) / denom;

		return ray.GetPoint(t);
	}
	float Gizmo::GetAdjustedSize(Camera* camera, const Math::Vector3& position) const
	{
		Math::Vector3 camPos = camera->GetTransform().GetPosition();
		float distance = sqrtf(
			(position.x - camPos.x) * (position.x - camPos.x) +
			(position.y - camPos.y) * (position.y - camPos.y) +
			(position.z - camPos.z) * (position.z - camPos.z)
		);
		return m_size * distance * 0.1f;
	}
}
