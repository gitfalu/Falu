/*****************************************************************//**
 * \file   Light.cpp
 * \brief  Light実装
 * 
 * \author tsunn
 * \date   2026/02/19
 *********************************************************************/
#include "Light.h"

namespace Falu
{
	Light::Light(LightType type):
		m_type(type),m_color(1.0f,1.0f,1.0f,1.0f),m_intensity(1.0f),
		m_range(10.0f),m_spotAngle(45.0f),m_enabled(true)
	{
		// デフォルトの方向(下向き)
		if (type == LightType::Directional)
		{
			m_transform.SetRotation(Math::ToRadians(45.0f), 0, 0);
		}
	}
	Light::~Light()
	{
	}

	/**
	 * 
	 * LightManager実装
	 * 
	 */

	LightManager& LightManager::GetInstace()
	{
		static LightManager instance;
		return instance;
	}

	Light* LightManager::CreateLight(LightType type)
	{
		auto light = std::make_unique<Light>(type);
		Light* ptr = light.get();
		m_lights.push_back(std::move(light));

		return ptr;
	}

	void LightManager::RemoveLight(Light* light)
	{
		m_lights.erase(
			std::remove_if(m_lights.begin(), m_lights.end(),
				[light](const std::unique_ptr<Light>& l)
				{
					return l.get() == light;
				}),
			m_lights.end()
		);
	}
	void LightManager::Clear()
	{
		m_lights.clear();
	}
}
