#include "StdAfx.h"

#include "InstanceList.h"
#include "ShadingState.h"
#include "instance.h"

InstanceList::InstanceList() 
{
	instances.clear();
	clearLightSources();
}

InstanceList::InstanceList(const vector<Instance*>&ins) 
{
	instances=ins;
	clearLightSources();
}

void InstanceList::set(const vector<Instance*>&ins)
{	
	instances=ins;
	clearLightSources();
}

void InstanceList::addLightSourceInstances(const vector<Instance*>&ls)
{
	lights=ls;
}

void InstanceList::clearLightSources() 
{
	lights.clear();
}

float InstanceList::getPrimitiveBound(int primID,int i) const
{
	if(primID<instances.size())		
		return instances[primID]->getBounds().getBound(i);
	else
		return lights[primID-instances.size()]->getBounds().getBound(i);
}

BoundingBox InstanceList::getWorldBounds(const Matrix4&o2w) const
{
	BoundingBox bounds;
	for(int i=0; i<instances.size(); i++)
		bounds.include(instances[i]->getBounds());
	for(int i=0; i<lights.size(); i++)
		bounds.include(lights[i]->getBounds());

	return bounds;
}

void InstanceList::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	if(primID<instances.size())
		instances[primID]->intersect(r,state);
	else
		lights[primID-instances.size()]->intersect(r,state);
}

int InstanceList::getNumPrimitives()const 
{
	return instances.size()+lights.size();
}

int InstanceList::getNumPrimitives(int primID) const
{
	return primID<instances.size()?instances[primID]->getNumPrimitives()
		:lights[primID-instances.size()]->getNumPrimitives();
}

void InstanceList::prepareShadingState(ShadingState&state) 
{
	state.getInstance()->prepareShadingState(state);
}

BOOL InstanceList::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

PrimitiveList* InstanceList::getBakingPrimitives() const
{
	return NULL;
}

InstanceList& InstanceList::operator=(const InstanceList&il)
{
	if(this==&il) return *this;

    instances=il.instances;
	lights=il.lights;

	return *this;
}


