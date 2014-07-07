//
//  Timer.cpp
//  Rendering
//
//  Created by Jiman Jeong on 13. 9. 18..
//  Copyright (c) 2013년 SOC. All rights reserved.
//

#include "Timer.h"

namespace Core
{

	Timer::Timer() : m_fps(0), m_now(0), m_prevTime(0), m_tick(0)
	{
		SetFPS(60.f);

		// first time correction
		m_endTick				= m_tick + m_reachingTime;
	}

	Timer::~Timer()
	{
	}

	void Timer::CheckFPS()
	{
		m_fpsPoint[m_now]++;
		if(m_tick - m_prevTime >= 1.0f)
		{
			m_prevTime			= m_tick;

			int					sum = 0;
			for(int i = 0; i < NUM_FPS_POINT; ++i)
				sum				+= m_fpsPoint[i];
			m_fps				= sum / (float)NUM_FPS_POINT;

			//		m_now				= (m_now + 1) % NUM_FPS_POINT;
			if(++m_now == NUM_FPS_POINT) m_now = 0;
			m_fpsPoint[m_now]	= 0;
		}
	}

	void Timer::SetFPS(float fps)
	{
		m_reachingTime			= 1.f / fps;
		m_fps					= fps; // correction for first checking time

		for(int i = 1; i < NUM_FPS_POINT; ++i)
			m_fpsPoint[i]		= static_cast<int>(fps);

		m_fpsPoint[m_now = 0]	= 0;
	}

	//bool Timer::elapsed()
	//{
	//	TIME::CHECK_TIME_END(m_tick);
	//	return (m_tick - m_endTick >= m_reachingTime) && (m_endTick = m_tick);
	//}

}