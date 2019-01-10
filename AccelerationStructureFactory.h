#pragma once

class AccelerationStructureFactory
{
public:
	static AccelerationStructure* create(CString name,int n,BOOL primitives);
};
