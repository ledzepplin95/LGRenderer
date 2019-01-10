#include "StdAfx.h"
#include "Timer.h"

Timer::~Timer(void)
{
}

Timer::Timer() 
{
	startTime=endTime=0;
}

void Timer::start() 
{
	startTime=endTime=::GetTickCount();
}

void Timer::end() 
{
	endTime=::GetTickCount();
}

long Timer::nanos() const
{
	return endTime-startTime;
}

double Timer::seconds()const
{
	return (endTime-startTime)*1e-9;
}

CString Timer::toString(long nanos) 
{
	Timer t;
	t.endTime=nanos;

	return t.toString();
}

CString Timer::toString(double seconds) 
{
	Timer t;
	t.endTime=(long)(seconds*1e9);

	return t.toString();
}

CString Timer::toString() const
{
	long millis=nanos();
	if(millis<10000)
	{
		CString str;
		str.Format(_T("%dms"),millis);

		return str;
	}
	long hours=millis/(60*60*1000);
	millis-=hours*60*60*1000;
	long minutes=millis/(60*1000);
	millis-=minutes*60*1000;
	long seconds=millis/1000;
	millis-=seconds*1000;
	CString str;
	str.Format(_T("%d:%02d:%02d.%1d"), 
		hours,minutes,seconds,millis/100);

	return str;
}