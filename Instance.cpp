#include "StdAfx.h"

#include "Instance.h"
#include "IntersectionState.h"
#include "ShadingState.h"
#include "geometry.h"
#include "lgapi.h"
#include "modifier.h"
#include "shader.h"
#include "StrUtil.h"

LG_IMPLEMENT_DYNCREATE(Instance,RenderObject)

Instance::Instance() 
{
	geometry=NULL;
	bounds.set(0.0f);
}

Instance* Instance::createTemporary(PrimitiveList&primitives,const Matrix4&transform, 
								Shader*shader) 
{		
	Instance* i=new Instance();	
	LGMemoryAllocator::lg_mem_pointer(i);
	i->o2w.set(transform);
	i->w2o=i->o2w.inverse();
	if(i->geometry==NULL)
	{	
		void*lgp=LGMemoryAllocator::lg_malloc(sizeof(Geometry));
		i->geometry=new (lgp) Geometry();		
	}
	i->geometry->set(&primitives);
	i->shaders.push_back(shader);
	i->updateBounds();

	return i;
}

BOOL Instance::update(ParameterList&pl,LGAPI&api) 
{
	CString geometryName=pl.getString("geometry",CString());
	if( geometry==NULL || !geometryName.IsEmpty() ) 
	{
		if(geometryName.IsEmpty())
		{
			StrUtil::PrintPrompt("几何参数丢失，不能创建instance");
			return FALSE;
		}
		geometry=api.lookupGeometry(geometryName);
		if(geometry==NULL) 
		{
			StrUtil::PrintPrompt("几何体\"%s\"未声明，instance无效",geometryName);
			return FALSE;
		}
	}
	
	vector<CString> shaderNames=pl.getStringArray("shaders",vector<CString>());
	if( !shaderNames.empty() )
	{		
		shaders.resize(shaderNames.size());
		for(int i=0; i<shaders.size(); i++)
		{
			shaders[i]=api.lookupShader(shaderNames[i]);
		    if(shaders[i]==NULL)
			    StrUtil::PrintPrompt("Shader\"%s\"未声明，忽略",shaderNames[i]);
		}
	}

	vector<CString> modifierNames=pl.getStringArray("modifiers",vector<CString>());
	if( !modifierNames.empty() ) 
	{		
		modifiers.resize(modifierNames.size());
		for(int i=0; i<modifiers.size(); i++) 		
		{
			modifiers[i]=api.lookupModifier(modifierNames[i]);
			if(modifiers[i]==NULL)
				StrUtil::PrintPrompt("Modifier\"%s\"未声明，忽略",modifierNames[i]);
		}
	}
	o2w=pl.getMovingMatrix("transform",o2w);
	w2o=o2w.inverse();
	if(w2o.isNull())
	{
		StrUtil::PrintPrompt("不能计算变换矩阵的逆");
        return FALSE;
	}

	return TRUE;
}

void Instance::updateBounds()
{
	bounds=geometry->getWorldBounds(o2w.getData(0));
	for(int i=1; i<o2w.numSegments(); i++)
		bounds.include(geometry->getWorldBounds(o2w.getData(i)));
}

BOOL Instance::hasGeometry(Geometry*g) const
{
	return geometry==g;
}

void Instance::removeShader(Shader*s)
{
	if(!shaders.empty())
	{
		for(int i=0; i<shaders.size(); i++)
			if(shaders[i]==s)
				shaders.erase(shaders.begin()+i);
	}
}

void Instance::removeModifier(Modifier*m) 
{
	if(!modifiers.empty())
	{
		for(int i=0; i<modifiers.size(); i++)
			if(modifiers[i]==m)
				modifiers.erase(modifiers.begin()+i);
	}
}

BoundingBox Instance::getBounds() const
{
	return bounds;
}

int Instance::getNumPrimitives() const
{
	return geometry->getNumPrimitives();
}

void Instance::intersect(Ray&r,IntersectionState&state)const
{
	Ray localRay=r.transform(w2o.sample(state.time));
	state.current=(Instance*)this;
	geometry->intersect(localRay,state);	
	r.setMax(localRay.getMax());
}

void Instance::prepareShadingState(ShadingState&state) 
{
	geometry->prepareShadingState(state);
	if(state.getNormal()!=Vector3() && state.getGeoNormal()!=Vector3())
		state.correctShadingNormal();	
	if(state.getModifier()!=NULL)
		state.getModifier()->modify(state);
}

Shader* Instance::getShader(int i) const
{
	if( shaders.empty() || i<0 || i>=shaders.size() )
		return NULL;

	return shaders[i];
}

Modifier* Instance::getModifier(int i) const
{
	if( modifiers.empty() || i<0 || i>=modifiers.size() )
		return NULL;

	return modifiers[i];
}

Matrix4 Instance::getObjectToWorld(float time)const
{
	return o2w.sample(time);
}

Matrix4 Instance::getWorldToObject(float time) const
{
	return w2o.sample(time);
}

PrimitiveList* Instance::getBakingPrimitives() const
{
	return geometry->getBakingPrimitives();
}

Geometry* Instance::getGeometry()const
{
	return geometry;
}

BOOL Instance::operator !=(const Instance&ins) const
{
	return TRUE;
}

Instance& Instance::operator =(const Instance&i)
{
	if(this==&i) return *this;

    o2w=i.o2w;
    w2o=i.w2o;
	bounds=i.bounds;
	geometry=i.geometry;
	shaders=i.shaders;
	modifiers=i.modifiers;

	return *this;
}

