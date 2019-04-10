#include<stdafx.h>

#include "PerfTimer.h"

#if defined(WIN32)

// Win32
#include <windows.h>

TimeVal getPerfTime()
{
	__int64 count;
	QueryPerformanceCounter((LARGE_INTEGER*)&count);
	return count;
}

int getPerfTimeUsec(const TimeVal duration)
{
	static __int64 freq = 0;
	if (freq == 0)
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return (int)(duration*1000000 / freq);
}

#else

// Linux, BSD, OSX

#include <sys/time.h>

TimeVal getPerfTime()
{
	timeval now;
	gettimeofday(&now, 0);
	return (TimeVal)now.tv_sec*1000000L + (TimeVal)now.tv_usec;
}

int getPerfTimeUsec(const TimeVal duration)
{
	return (int)duration;
}

#endif
