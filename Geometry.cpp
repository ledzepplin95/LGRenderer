#include "StdAfx.h"

#include "Geometry.h"
#include "NullAccelerator.h"
#include "strutil.h"
#include "AccelerationStructureFactory.h"
#include "primitivelist.h"
#include "tesselatable.h"
#include "BoundingBox.h"
#include "parameterlist.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(Geometry,RenderObject)

Geometry::Geometry(void)
{
	builtAccel=0;
	builtTess=0;
	acceltype="";

	tesselatable=NULL;
	primitives=NULL;
	accel=NULL;
}

Geometry::Geometry(Tesselatable*tess) 
{
	tesselatable=tess;	
	builtAccel=0;
	builtTess=0;
	acceltype="";
	accel=NULL;
	primitives=NULL;
}

Geometry::Geometry(PrimitiveList*prim) 
{	
	primitives=prim;	
	builtAccel=0;
	builtTess=1;
	accel=NULL;
	tesselatable=NULL;
}

void Geometry::set(Tesselatable*tess)
{
	tesselatable=tess;	
	builtAccel=0;
	builtTess=0;
	acceltype="";
	accel=NULL;
	primitives=NULL;
}

void Geometry::set(PrimitiveList*prim)
{
	primitives=prim;	
	builtAccel=0;
	builtTess=1;
	accel=NULL;
	tesselatable=NULL;
}

BOOL Geometry::update(ParameterList&pl,LGAPI&api) 
{
	acceltype=pl.getString("accel",acceltype);	
    if(tesselatable!=NULL)
	{
		primitives=NULL;
		builtTess=0;
	}

	accel=NULL;
	builtAccel=0;
	if(tesselatable!=NULL) 
		return tesselatable->update(pl,api);

	return primitives->update(pl,api);
}

int Geometry::getNumPrimitives() const
{
	return  primitives==NULL?0:primitives->getNumPrimitives();
}

BoundingBox Geometry::getWorldBounds(const Matrix4&o2w)
{
	if(primitives==NULL)
	{
		BoundingBox b=tesselatable->getWorldBounds(o2w);
		if(b!=BoundingBox(0.0f))
			return b;
		if(builtTess==0)
			tesselate();
		if(primitives==NULL)
			return BoundingBox(0.0f);
	}

	return primitives->getWorldBounds(o2w);
}

void Geometry::intersect(Ray&r,IntersectionState&state)
{
	if(builtTess==0) tesselate();
	if(builtAccel==0) build();
	accel->intersect(r,state);
}

void Geometry::tesselate()
{
	cs.Lock();
	if(builtTess!=0)
	{
		cs.Unlock();
		return;
	}
	
	if(tesselatable!=NULL && primitives==NULL)
	{
		StrUtil::PrintPrompt("几何体三角化...");
		primitives=tesselatable->tesselate();
		if(primitives==NULL)
			StrUtil::PrintPrompt("三角化未成功，丢弃几何体");
		else
			StrUtil::PrintPrompt("三角化产生%d图素",primitives->getNumPrimitives());
		builtTess=1;
	}
	cs.Unlock();
}

void Geometry::build() 
{	
	cs.Lock();
	if(builtAccel!=0) 
	{
		cs.Unlock();
		return;
	}
	if(primitives!=NULL)
	{
		int n=primitives->getNumPrimitives();
		if(n>=1000)
			StrUtil::PrintPrompt("为%d个图素创建加速结构...",n);
		accel=AccelerationStructureFactory::create(acceltype,n,TRUE);
		accel->build(primitives);
	}
	else
	{		
		void*lgp=LGMemoryAllocator::lg_malloc(sizeof(NullAccelerator));
		accel=new (lgp) NullAccelerator();		
	}
	
	builtAccel=1;
    cs.Unlock();
}

void Geometry::prepareShadingState(ShadingState&state) 
{
	primitives->prepareShadingState(state);
}

PrimitiveList* Geometry::getBakingPrimitives() 
{
	if(builtTess==0) tesselate();

	if(primitives==NULL)
		return NULL;

	return primitives->getBakingPrimitives();
}

PrimitiveList* Geometry::getPrimitiveList()const 
{
	return primitives;
}

BOOL Geometry::operator ==(const Geometry &geo) const
{
    return tesselatable==geo.tesselatable
		&& primitives==geo.primitives
		&& accel==geo.accel
		&& builtAccel==geo.builtAccel
		&& builtTess==geo.builtTess
		&& acceltype==geo.acceltype;
}