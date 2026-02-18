/*****************************************************************//**
 * \file   Light.h
 * \brief  LightçÏê¨
 * 
 * \author tsunn
 * \date   2026/02/18
 *********************************************************************/
#pragma once

#include "Include/MathHelper.h"
#include "Scene/Transform.h"
#include <memory>
#include <vector>

namespace Falu
{
	enum class LightType
	{
		Directional = 0,
		Point = 1,
		Spot = 2,
	};

	class Light
	{
	public:
		Light(LightType type = LightType::Directional);
		~Light();

		// Transform
		Transform& GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }

		void SetIntensity(float intensity) { m_intensity = intensity; }
		float GetIntensity() const { return m_intensity; }

		void SetRange(float range) { m_range = range; }
		float GetRange()const { return m_range; }

		void SetSpotAngle(float angle) { m_spotAngle = angle; }
		float GetSpotAngle()const { return m_spotAngle; }

		void SetType(LightType type) { m_type = type; }
		LightType GetType()const { return m_type; }

		// óLå¯/ñ≥å¯
		void SetEnabled(bool enabled) { m_enabled = enabled; }
		bool IsEnabled()const { return m_enabled; }

	private:
		Transform m_transform;
		LightType m_type;
		Math::Color m_color;
		float m_intensity;
		float m_range;
		float m_spotAngle;
		bool m_enabled;
	};

	// ÉâÉCÉgä«óù
	class LightManager
	{
	public:
		static LightManager& GetInstace();

		Light* CreateLight(LightType type = LightType::Directional);
		void RemoveLight(Light* light);
		void Clear();

		const std::vector<std::unique_ptr<Light>>& GetLights()const { return m_lights; }

	private:
		LightManager() = default;
		~LightManager() = default;
		LightManager(const LightManager&) = delete;
		LightManager& operator=(const LightManager&) = delete;

	private:
		std::vector<std::unique_ptr<Light>> m_lights;
	};
}
