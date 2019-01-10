#pragma once
#include "lgapi.h"

class RealtimeBenchmark :
	public LGAPI
{
public:
	RealtimeBenchmark(unsigned int threads);

private:
	void createGeometry();	
};
