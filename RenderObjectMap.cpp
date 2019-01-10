#include "StdAfx.h"

#include "RenderObjectMap.h"
#include "Shader.h"
#include "modifier.h"
#include "Instance.h"
#include "LightSource.h"
#include "Camera.h"
#include "options.h"
#include "strutil.h"
#include "lgapi.h"
#include "Scene.h"
#include "Geometry.h"

RenderObjectMap::RenderObjectHandle::RenderObjectHandle()
{
	obj=NULL;
	t=ROT_UNKNOWN;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Shader*shader)
{
	obj=shader;
	t=ROT_SHADER;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Modifier*modifier)
{
	obj=modifier;
	t=ROT_MODIFIER;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Tesselatable*tesselatable) 
{
	void*lgp=LGMemoryAllocator::lg_malloc(sizeof(Geometry));
	obj=new (lgp) Geometry(tesselatable);
	t=ROT_GEOMETRY;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(PrimitiveList*prims) 
{	
	void*lgp=LGMemoryAllocator::lg_malloc(sizeof(Geometry));
	obj=new (lgp) Geometry(prims);
	t=ROT_GEOMETRY;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Instance*instance)
{
	obj=instance;
	t=ROT_INSTANCE;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(LightSource*light)
{
	obj=light;
	t=ROT_LIGHT;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Camera*camera) 
{
	obj=camera;
	t=ROT_CAMERA;
}

RenderObjectMap::RenderObjectHandle::RenderObjectHandle(Options*options) 
{
	obj=options;
	t=ROT_OPTIONS;
}

RenderObjectMap::RenderObjectHandle& RenderObjectMap::RenderObjectHandle::operator=(
	const RenderObjectHandle&h)
{
	if(this==&h) return *this;

	obj=h.obj;
	t=h.t;

	return *this;
}

boolean RenderObjectMap::RenderObjectHandle::update(ParameterList&pl,LGAPI&api)
{
	return obj->update(pl,api);
}

BOOL RenderObjectMap::RenderObjectHandle::isNull()const
{
	return obj==NULL && t==ROT_UNKNOWN;
}

CString RenderObjectMap::RenderObjectHandle::typeName() const
{
	return GetTypeName(t);
}

CString RenderObjectMap::RenderObjectHandle::GetTypeName(RenderObjectType rt)const
{
	switch(rt)
	{
	case ROT_UNKNOWN:
		return "unknown";		
	case ROT_SHADER:
		return "shader";
	case ROT_MODIFIER:
		return "modifier";
	case ROT_GEOMETRY:
		return "geometry";
	case ROT_INSTANCE:
		return "instance";
	case ROT_LIGHT:
		return "light";
	case ROT_CAMERA:
		return "camera";
	case ROT_OPTIONS:
		return "options";		
	default:
		return CString();
	}
}

Shader* RenderObjectMap::RenderObjectHandle::getShader() const
{
	return (t==ROT_SHADER)?(Shader*)obj:NULL;
}

Modifier* RenderObjectMap::RenderObjectHandle::getModifier() const
{
	return (t==ROT_MODIFIER)?(Modifier*)obj:NULL;
}

Geometry* RenderObjectMap::RenderObjectHandle::getGeometry() const
{
	return (t==ROT_GEOMETRY)?(Geometry*)obj:NULL;
}

Instance* RenderObjectMap::RenderObjectHandle::getInstance() const
{
	return (t==ROT_INSTANCE)?(Instance*)obj:NULL;
}

LightSource* RenderObjectMap::RenderObjectHandle::getLight() const
{
	return (t==ROT_LIGHT)?(LightSource*)obj:NULL;
}

Camera* RenderObjectMap::RenderObjectHandle::getCamera() const
{
	return (t==ROT_CAMERA)?(Camera*)obj:NULL;
}

Options* RenderObjectMap::RenderObjectHandle::getOptions() const
{
	return (t==ROT_OPTIONS)?(Options*)obj:NULL;
}

RenderObjectMap::RenderObjectMap() 
{	
	rebuildInstanceList=rebuildLightList=FALSE;
}

void RenderObjectMap::remove(CString name)
{
	RenderObjectHandle obj=renderObjects[name];
    if(obj.isNull()) 
	{
		StrUtil::PrintPrompt("不能移除 \"%s\"，对象未定义");
		return;
	}
	StrUtil::PrintPrompt("移除对象 \"%s\"",name);
	renderObjects.erase(name);
	hash_map<CString,RenderObjectHandle,hash_compare<const char*,CharLess>>::iterator 
		iter=renderObjects.begin();
	
	switch(obj.t)
	{
	case ROT_SHADER:
		{	
			Shader*s=obj.getShader();			
            while(iter!=renderObjects.end())
			{
				RenderObjectHandle rh=iter->second;
				Instance*i=rh.getInstance();
				if(i!=NULL)
				{
					StrUtil::PrintPrompt("从instance \"%s\"中移除shader \"%s\"", 
						iter->first,name);
					i->removeShader(s);
				}
				iter++;
			}		
		}
		break;
	case ROT_MODIFIER:
		{	
			Modifier* m=obj.getModifier();
			while(iter!=renderObjects.end())
			{
				RenderObjectHandle rh=iter->second;
				Instance*i=rh.getInstance();
				if(i!=NULL)
				{
					StrUtil::PrintPrompt("从instance \"%s\"中移除modifier \"%s\"", 
						iter->first,name);
					i->removeModifier(m);
				}
				iter++;
			}	
		}
		break;
	case ROT_GEOMETRY: 
		{	
			Geometry* g=obj.getGeometry();
			while(iter!=renderObjects.end())
			{
				RenderObjectHandle rh=iter->second;
				Instance*i=rh.getInstance();
				if( i!=NULL && i->hasGeometry(g) )
				{
					StrUtil::PrintPrompt("移除instance \"%s\"， 因其已参照几何体 \"%s\"",
						iter->first,name);
					remove(iter->first);
				}
				iter++;
			}		
		}
		break;
	case ROT_INSTANCE:
		rebuildInstanceList=TRUE;
		break;
	case ROT_LIGHT:
		rebuildLightList=TRUE;
		break;
	default:				
		break;
	}
}

BOOL RenderObjectMap::update(CString name,ParameterList&pl,LGAPI&api) 
{
	RenderObjectHandle obj=renderObjects[name];
	BOOL success;	
	if(obj.isNull())
	{
		StrUtil::PrintPrompt("不能更新\"%s\" -对象未定义",name);
		success=FALSE;
	}
	else
	{
		StrUtil::PrintPrompt("更新%s对象\"%s\"",obj.typeName(),name);
		success=obj.update(pl,api);
		if(!success)
		{
			StrUtil::PrintPrompt("不能更新\"%s\" -移除",name);
			remove(name);
		}
		else 
		{
			switch(obj.t) 
			{
			case ROT_GEOMETRY:
			case ROT_INSTANCE:
				rebuildInstanceList=TRUE;
				break;
			case ROT_LIGHT:
				rebuildLightList=TRUE;
				break;
			default:
				break;
			}
		}
	}	

	return success;
}

void RenderObjectMap::updateScene(Scene*scene) 
{
	hash_map<CString,RenderObjectHandle,hash_compare<const char*,CharLess>>::iterator 
		iter=renderObjects.begin();

	if(rebuildInstanceList)
	{
		StrUtil::PrintPrompt("为渲染创建场景Instance列表...");
		int numInfinite=0,numInstance=0;
		while(iter!=renderObjects.end())
		{
			RenderObjectHandle rh=iter->second;
			Instance*i=NULL;
			i=rh.getInstance();
			if(i!=NULL)
			{
				i->updateBounds();
				if(i->getBounds()==BoundingBox())
					numInfinite++;
				else if(!i->getBounds().isEmpty())
					numInstance++;
				else
					StrUtil::PrintPrompt("忽略空Instance: \"%s\"",iter->first);				
			}
			iter++;
		}	

		vector<Instance*> infinites;
	    if(numInfinite>0)
			infinites.resize(numInfinite);
		vector<Instance*> instances;
		if(numInstance>0)
	        instances.resize(numInstance);
		numInfinite=numInstance=0;
		iter=renderObjects.begin();
		while(iter!=renderObjects.end())
		{
			RenderObjectHandle rh=iter->second;
			Instance*i=NULL;
			i=rh.getInstance();
			if(i!=NULL)
			{		
				if(i->getBounds()==BoundingBox())
				{
					infinites[numInfinite]=i;
					numInfinite++;
				}
				else if(!i->getBounds().isEmpty()) 
				{
					instances[numInstance]=i;
					numInstance++;
				}			
			}
			iter++;
		}	
		scene->setInstanceLists(instances,infinites);
		rebuildInstanceList=FALSE;
	}
	if(rebuildLightList) 
	{
		StrUtil::PrintPrompt("为渲染创建场景光源列表...");
		vector<LightSource*> lightList;

		iter=renderObjects.begin();
		while(iter!=renderObjects.end())
		{
			LightSource* light=iter->second.getLight();
			if(light!=NULL)
				lightList.push_back(light);
			iter++;
		}	
		scene->setLightList(lightList);
		rebuildLightList=FALSE;
	}
}

RenderObjectMap& RenderObjectMap::operator=(const RenderObjectMap&rom)
{
	if(this==&rom) return *this;

	rebuildInstanceList=rom.rebuildInstanceList;
	rebuildLightList=rom.rebuildLightList;
	renderObjects=rom.renderObjects;	

	return *this;
}

void RenderObjectMap::put(CString name,Shader* shader) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(shader)));
}

void RenderObjectMap::put(CString name,Modifier* modifier)
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(modifier)));
}

void RenderObjectMap::put(CString name,PrimitiveList* primitives) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(primitives)));
}

void RenderObjectMap::put(CString name,Tesselatable* tesselatable) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(tesselatable)));
}

void RenderObjectMap::put(CString name,Instance* instance) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(instance)));
}

void RenderObjectMap::put(CString name,LightSource* light) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(light)));
}

void RenderObjectMap::put(CString name,Camera* camera) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(camera)));
}

void RenderObjectMap::put(CString name,Options* options) 
{
	renderObjects.insert(CR_PAIR(name,RenderObjectHandle(options)));
}

BOOL RenderObjectMap::has(CString name)const
{
	hash_map<CString,RenderObjectHandle,
		hash_compare<const char*,CharLess>>::const_iterator iter;
	iter=renderObjects.find(name);	

	return iter!=renderObjects.end();
}

BOOL RenderObjectMap::has(CString name,RenderObjectHandle&rh) const
{
	hash_map<CString,RenderObjectHandle,
		hash_compare<const char*,CharLess>>::const_iterator iter;
	iter=renderObjects.find(name);

	if(iter!=renderObjects.end())
	{
        rh=(*iter).second;
		return TRUE;
	}

	return FALSE;
}

Geometry* RenderObjectMap::lookupGeometry(CString name) const
{
	if(name.IsEmpty())
		return NULL;

	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getGeometry();
	else return NULL;
}

Instance* RenderObjectMap::lookupInstance(CString name)const
{
	if(name.IsEmpty())
		return NULL;
	
	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getInstance();
	else return NULL;
}

Camera* RenderObjectMap::lookupCamera(CString name) const
{
	if(name.IsEmpty())
		return NULL;
	
	RenderObjectHandle rh;
	if(has(name,rh))
	   return rh.isNull()?NULL:rh.getCamera();
	else return NULL;
}

Options* RenderObjectMap::lookupOptions(CString name) const
{
	if(name.IsEmpty())
		return NULL;
	
	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getOptions();
	else return NULL;
}

Shader* RenderObjectMap::lookupShader(CString name) const
{
	if(name.IsEmpty())
		return NULL;

	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getShader();
	else return NULL;
}

Modifier* RenderObjectMap::lookupModifier(CString name) const
{
	if(name.IsEmpty())
		return NULL;
	
	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getModifier();
	else return NULL;
}

LightSource* RenderObjectMap::lookupLight(CString name) const
{
	if(name.IsEmpty())
		return NULL;

	RenderObjectHandle rh;
	if(has(name,rh))
	    return rh.isNull()?NULL:rh.getLight();
	else return NULL;
}

