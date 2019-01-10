#pragma once

#include "Timer.h"

class BenchmarkFramework
{
public:
	BenchmarkFramework(int iter,int tLimit);
    void execute(BenchmarkTest&test);

private:
    int timeLimit;
    vector<Timer> timers;	 
};
