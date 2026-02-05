/*****************************************************************//**
 * \file   TimeManager.h
 * \brief  éûä‘ä«óù
 * 
 * \author tsunn
 * \date   2026/02/04
 *********************************************************************/
#pragma once

#include <Windows.h>

namespace Falu
{
	class TimeManager
	{
	public:
		TimeManager();
		~TimeManager();

		void Initialize();
		void Update();

		float GetDeltaTime() { return m_deltaTime; }
		float GetTotalTime() { return m_totalTime; }
		int GetFPS() { return m_FPS; }
		void SetTargetFPS(int targetFPS) { m_targetFPS = targetFPS; }

	private:
		void CalculateFPS();

	private:
		LARGE_INTEGER m_frequency;
		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_currentTime;
		LARGE_INTEGER m_lastTime;

		float m_deltaTime;
		float m_totalTime;
		float m_frameTime;
		int m_frameCount;
		int m_FPS;
		int m_targetFPS;
	};
}
