#pragma once

class BenchmarkTest
{
public:
    virtual void kernelBegin();
	virtual void kernelMain();	
	virtual void kernelEnd();
};
