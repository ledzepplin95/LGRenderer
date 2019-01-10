#pragma once

#include "primitivelist.h"

class InstanceList :
	public PrimitiveList
{
public:
	InstanceList(void);
	InstanceList(const vector<Instance*>&ins);	

	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	int getNumPrimitives(int primID)const;
	PrimitiveList* getBakingPrimitives()const;
	int getNumPrimitives()const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;

	BOOL update(ParameterList&pl,LGAPI&api);
	void set(const vector<Instance*>&ins);
	void addLightSourceInstances(const vector<Instance*>&ls);
	void clearLightSources();		
	void prepareShadingState(ShadingState&state);
    InstanceList& operator=(const InstanceList&il);

private:
	vector<Instance*> instances;
	vector<Instance*> lights;	
};
