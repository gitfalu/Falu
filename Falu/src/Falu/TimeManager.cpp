/*****************************************************************//**
 * \file   TimeManager.cpp
 * \brief  タイムマネージャー実装
 * 
 * \author tsunn
 * \date   February 2026
 *********************************************************************/
#include "TimeManager.h"

namespace Falu
{
	TimeManager::TimeManager()
		:m_deltaTime(0.0f)
		,m_totalTime(0.0f)
		,m_frameTime(0.0f)
		,m_frameCount(0)
		,m_FPS(0)
		,m_targetFPS(60)
	{

	}

	TimeManager::~TimeManager()
	{

	}

	void TimeManager::Initialize()
	{
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_startTime);
		m_lastTime = m_startTime;
		m_currentTime = m_startTime;
	}

	void TimeManager::Update()
	{
		QueryPerformanceCounter(&m_currentTime);

		// デルタタイムの計算
		m_deltaTime = static_cast<float>(m_currentTime.QuadPart - m_lastTime.QuadPart) /
			static_cast<float>(m_frequency.QuadPart);

		// 合計時間の計算
		m_totalTime = static_cast<float>(m_currentTime.QuadPart - m_startTime.QuadPart) /
			static_cast<float>(m_frequency.QuadPart);

		// FPSの計算
		CalculateFPS();
		m_lastTime = m_currentTime;
	}

	void TimeManager::CalculateFPS()
	{
		m_frameCount++;
		m_frameTime += m_deltaTime;

		// １秒ごとにFPSを更新
		if (m_frameTime >= 1.0f)
		{
			m_FPS = m_frameCount;
			m_frameCount = 0;
			m_frameTime = 0.0f;
		}
	}
}
