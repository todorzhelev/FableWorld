#pragma once

#ifdef __GNUC__
#include <stdint.h>
typedef int64_t TimeVal;
#else
typedef __int64 TimeVal;
#endif

TimeVal getPerfTime();
int getPerfTimeUsec(const TimeVal duration);

