#include "StdAfx.h"

#include "Background.h"
#include "ShadingState.h"
#include "float.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(Background,PrimitiveList)

Background::Background() 
{
}

BOOL Background::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

void Background::prepareShadingState(ShadingState&state)
{
	if(state.getDepth()==0)
		state.setShader(state.getInstance()->getShader(0));
}

int Background::getNumPrimitives()const
{
	return 1;
}

float Background::getPrimitiveBound(int primID,int i)const 
{
	return 0.0f;
}

BoundingBox Background::getWorldBounds(const Matrix4&o2w)const 
{
	return BoundingBox();
}

void Background::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	if(r.getMax()==FLT_MAX)
		state.setIntersection(0);
}

PrimitiveList* Background::getBakingPrimitives() const
{
	return NULL;
}