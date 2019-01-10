#include "StdAfx.h"

#include "BenchmarkFramework.h"
#include "float.h"
#include "strutil.h"
#include "BenchmarkTest.h"

BenchmarkFramework::BenchmarkFramework(int iter,int tLimit)
{
	timeLimit=tLimit;
	timers.resize(iter);
}

void BenchmarkFramework::execute(BenchmarkTest&test)
{
	for(int i=0; i<timers.size(); i++)
		timers[i]=Timer();

	long startTime=GetTickCount();
	for(int i=0; i<timers.size()
		&& ((GetTickCount()-startTime)/1000000000)<timeLimit; i++)
	{
		StrUtil::PrintPrompt("执行迭代%d",(i+1));
		timers[i]=Timer();
		test.kernelBegin();
		timers[i].start();
		test.kernelMain();
		timers[i].end();
		test.kernelEnd();
	}

	double avg=0.0;
	double _min=DBL_MAX;
	double _max=DBL_MIN;
	int n=0;
	for(int i=0; i<timers.size(); i++) 
	{		
		Timer t=timers[i];
		double s=t.seconds();
		_min=min(_min,s);
		_max=max(_max,s);
		avg+=s;
		n++;
	}
	if(n==0)
		return;
	avg/=n;
	double stdDev=0.0;
	for(int i=0; i<timers.size(); i++) 
	{
		Timer t=timers[i];
		double s=t.seconds();
		stdDev+=(s-avg)*(s-avg);
	}

	stdDev=sqrt(stdDev/n);
	StrUtil::PrintPrompt("Benchmark结果:");
	StrUtil::PrintPrompt("  * 迭代数:  %d",n);
	StrUtil::PrintPrompt("  * 平均:    %s",Timer::toString(avg));
	StrUtil::PrintPrompt("  * 最快:    %s",Timer::toString(_min));
	StrUtil::PrintPrompt("  * 最长:    %s",Timer::toString(_max));
	StrUtil::PrintPrompt("  * 偏差:    %s",Timer::toString(stdDev));
	for(int i=0; i<timers.size(); i++)
		StrUtil::PrintPrompt("  * 迭代 %d: %s",i+1,timers[i]);
}