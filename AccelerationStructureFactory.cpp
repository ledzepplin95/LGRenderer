#include "StdAfx.h"

#include "AccelerationStructureFactory.h"
#include "PluginRegistry.h"

AccelerationStructure* AccelerationStructureFactory::create(CString name,
	int n,BOOL primitives)
{
	if( name.IsEmpty() || name=="auto" )
	{
		if(primitives) 
		{
			//if(n>20000000)
			//	name="uniformgrid";
			//else if(n>2000000)
			//	name="bih";
			//else if(n>2)
			//	name="kdtree";
			//else
				name="null";
		} 
		else 
		{
			/*if(n>2)
				name="bih";
			else*/
				name="null";
		}
	}

	AccelerationStructure* accel=NULL;
	accel=PluginRegistry::accelPlugins.createObject(name,2);	

	return accel;
}