#ifndef _PORTABLE_TIMER_H
#define _PORTABLE_TIMER_H 


#ifdef WIN32
#include <windows.h>
#endif

//TODO portability 
class PortableTimer
{
public:
	void Init();
	void StartTimer();
	void EndTimer();
	float GetTimeSecond();

private:
#ifdef WIN32
	LARGE_INTEGER _ticksPerSecond;
	LARGE_INTEGER _tic, _toc; 
#endif

};


inline void PortableTimer::Init()
{
#ifdef WIN32
	QueryPerformanceFrequency(&_ticksPerSecond);

#endif
}


inline void PortableTimer::StartTimer()
{
#ifdef WIN32
	QueryPerformanceCounter(&_tic);
#endif
}
inline void PortableTimer::EndTimer()
{
#ifdef WIN32
	QueryPerformanceCounter(&_toc);
#endif
}

inline float PortableTimer::GetTimeSecond()
{
#ifdef WIN32
	return float(_toc.LowPart-_tic.LowPart)/float(_ticksPerSecond.LowPart);
#endif
}

#endif
