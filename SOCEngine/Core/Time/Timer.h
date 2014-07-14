//
//  Timer.h
//  Rendering
//
//  Created by Jiman Jeong on 13. 9. 18..
//  Copyright (c) 2013년 SOC. All rights reserved.
//

#pragma once

#if defined(WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
#include <time.h>
#endif

#include <stdio.h>

#if defined(WIN32)
class BaseTimer
{
	__int64					m_start;
	__int64					m_freq;
	__int64					m_end;
	
	
public:
	BaseTimer()
	{
		QueryPerformanceFrequency ((_LARGE_INTEGER*)&m_freq);
		QueryPerformanceCounter((_LARGE_INTEGER*)&m_start);
	}
	
	void CheckElapsed(float& a)
	{
		QueryPerformanceCounter((_LARGE_INTEGER*)&m_end);
		a = (float)((float) (m_end - m_start)/m_freq);
	}
};
#elif defined(__APPLE__)
class BaseTimer
{
#if defined(__MACH__)
	//	clock_serv_t			m_cclock;
	//	mach_timespec_t			m_start;
	//	mach_timespec_t			m_end;
	int64_t					m_start;
	int64_t					m_end;
	double					m_freq;
#else
	struct timespec         m_start;
	struct timespec			m_end;
#endif
    
public:
#if defined(__MACH__)
    BaseTimer()
    {
		mach_timebase_info_data_t	freqInfo;
		if ( freqInfo.denom == 0 )
		{
			(void)mach_timebase_info(&freqInfo);
			m_freq			= freqInfo.numer / freqInfo.denom * 1000000000.0;
		}
		m_start				= mach_absolute_time();
		
		//		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &m_cclock);
		//		clock_get_time(m_cclock, &m_start);
		//		mach_port_deallocate(mach_task_self(), m_cclock);
    }
	
	void CheckElapsed(float& a)
	{
		m_end				= mach_absolute_time();
		a					= ( (m_end - m_start) / (float)m_freq);
		//		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &m_cclock);
		//		clock_get_time(m_cclock, &m_end);
		//		mach_port_deallocate(mach_task_self(), m_cclock);
	}
#else
	BaseTimer()
	{
		clock_gettime(CLOCK_REALTIME, &m_start);
	}
	
	void CheckElapsed(float& a)
	{
		clock_gettime(CLOCK_REALTIME, &m_end);
	}
#endif
};
#endif

class Timer : public BaseTimer
{
	static const int		NUM_FPS_POINT = 1;
	
private:
	int						m_fpsPoint[NUM_FPS_POINT];
	int						m_now;
	float					m_fps;
	
	float					m_reachingTime;
	float					m_tick;
	float					m_endTick;
	
	float					m_prevTime;
	
	
public:
	Timer();
	~Timer();
	
	
public:
	static Timer*			GetDefaultTimer()
	{
		static Timer		singleton;
		return &singleton;
	}
	
	
public:
	void CheckFPS();
	float GetFPS() { return m_fps; }
	
	
public:
	void SetFPS(float fps);
	
	
public:
	inline bool Elapsed()
	{
		CheckElapsed(m_tick);
		return (m_tick - m_endTick >= m_reachingTime) && (m_endTick = m_tick);
	}
};