/*****************************************************************//**
 * \file   Ray.h
 * \brief  ÉåÉCÇÃçÏê¨
 * 
 * \author tsunn
 * \date   2026/02/24
 *********************************************************************/
#pragma once

#include "MathHelper.h"
#include <DirectXMath.h>
#include <algorithm>

namespace Falu
{
	namespace Math
	{
		// Ray
		struct Ray
		{
			Vector3 origin;
			Vector3 direction;

			Ray() : origin(0, 0, 0), direction(0, 0, 1) {}
			Ray(const Vector3& origin, const Vector3& direction)
				: origin(origin), direction(direction) {}

			// Set Position in Param
			Vector3 GetPoint(float t) const
			{
				return Vector3(
					origin.x + direction.x * t,
					origin.y + direction.y * t,
					origin.z + direction.z * t
				);
			}
		};

		// AABB (Axis-Aligned Bounding Box)
		struct AABB
		{
			Vector3 min;
			Vector3 max;

			AABB() : min(0, 0, 0), max(0, 0, 0) {}
			AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

			// Coordinate Center
			Vector3 GetCenter() const
			{
				return Vector3(
					(min.x + max.x) * 0.5f,
					(min.y + max.y) * 0.5f,
					(min.z + max.z) * 0.5f
				);
			}

			// Size
			Vector3 GetSize() const
			{
				return Vector3(
					max.x - min.x,
					max.y - min.y,
					max.z - min.z
				);
			}

			// included point/vertex
			bool Contains(const Vector3& point) const
			{
				return (point.x >= min.x && point.x <= max.x &&
						point.y >= min.y && point.y <= max.y &&
						point.z >= min.z && point.z <= max.z);
			}

			// Judge Crossed AABB
			bool Intersects(const AABB& other) const
			{
				return (min.x <= other.max.x && max.x >= other.min.x &&
						min.y <= other.max.y && max.y >= other.min.y &&
						min.z <= other.max.z && max.z >= other.min.z);
			}

			// Jedge Crossed Ray (Slab)
			bool IntersectsRay(const Ray& ray, float& tMin, float& tMax)const
			{
				tMin = 0.0f;
				tMax = FLT_MAX;

				// Shaft X
				if (fabs(ray.direction.x) > 0.0001f)
				{
					float t1 = (min.x - ray.origin.x) / ray.direction.x;
					float t2 = (max.x - ray.origin.x) / ray.direction.x;

					if (t1 > t2) std::swap(t1, t2);

					tMin = std::max(tMin, t1);
					tMax = std::min(tMax, t2);

					if (tMin > tMax)return false;
				}
				else if (ray.origin.x < min.x || ray.origin.x > max.x)
				{
					return false;
				}

				// Shaft Y
				if (fabs(ray.direction.y) > 0.0001f)
				{
					float t1 = (min.y - ray.origin.y) / ray.direction.y;
					float t2 = (max.y - ray.origin.y) / ray.direction.y;

					if (t1 > t2) std::swap(t1, t2);

					tMin = std::max(tMin, t1);
					tMax = std::min(tMax, t2);

					if (tMin > tMax)return false;
				}
				else if (ray.origin.y < min.y || ray.origin.y > max.y)
				{
					return false;
				}

				// Shaft Z
				if (fabs(ray.direction.z) > 0.0001f)
				{
					float t1 = (min.z - ray.origin.z) / ray.direction.z;
					float t2 = (max.z - ray.origin.z) / ray.direction.z;

					if (t1 > t2) std::swap(t1, t2);

					tMin = std::max(tMin, t1);
					tMax = std::min(tMax, t2);

					if (tMin > tMax)return false;
				}
				else if (ray.origin.z < min.z || ray.origin.z > max.z)
				{
					return false;
				}

				return true;
			}

			// AABB After Apply Transform
			AABB Transform(const DirectX::XMMATRIX& matrix) const
			{
				using namespace DirectX;

				// Convert eight Vertex
				Vector3 corners[8] = {
					Vector3(min.x,min.y,min.z),
					Vector3(min.x,min.y,max.z),
					Vector3(min.x,max.y,min.z),
					Vector3(min.x,max.y,max.z),
					Vector3(max.x,min.y,min.z),
					Vector3(max.x,min.y,max.z),
					Vector3(max.x,max.y,min.z),
					Vector3(max.x,max.y,max.z)
				};

				AABB result;

				result.min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
				result.max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

				for (int i = 0; i < 8; ++i) 
				{
					XMVECTOR v = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
					v = XMVector3Transform(v, matrix);

					XMFLOAT3 transformed;
					XMStoreFloat3(&transformed, v);

					result.min.x = std::min(result.min.x, transformed.x);
					result.min.y = std::min(result.min.y, transformed.y);
					result.min.z = std::min(result.min.z, transformed.z);

					result.max.x = std::max(result.max.x, transformed.x);
					result.max.y = std::max(result.max.y, transformed.y);
					result.max.z = std::max(result.max.z, transformed.z);
				}

				return result;
			}
		};
	}
}
