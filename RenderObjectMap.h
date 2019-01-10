#pragma once

#include <hash_map>
#include "CharLess.h"

class RenderObject;
class RenderObjectMap
{
	enum RenderObjectType
	{
		ROT_UNKNOWN,
		ROT_SHADER,
		ROT_MODIFIER, 
		ROT_GEOMETRY,
		ROT_INSTANCE,
		ROT_LIGHT,
		ROT_CAMERA, 
		ROT_OPTIONS,
	};

	class RenderObjectHandle
	{
	public:
		RenderObject* obj;
		RenderObjectType t;

		RenderObjectHandle();
		RenderObjectHandle(Shader*shader);
		RenderObjectHandle(Modifier*modifier);
        RenderObjectHandle(Tesselatable*tesselatable);
		RenderObjectHandle(PrimitiveList*prims);
		RenderObjectHandle(Instance*instance);
		RenderObjectHandle(LightSource*light);
		RenderObjectHandle(Camera*camera);
		RenderObjectHandle(Options*options);

		boolean update(ParameterList&pl,LGAPI&api);
		RenderObjectHandle& operator=(const RenderObjectHandle&h);

		CString typeName()const;       
		Shader* getShader()const;
		Modifier* getModifier()const;
		Geometry* getGeometry()const;
		Instance* getInstance()const;
		LightSource* getLight()const;
		Camera* getCamera()const;
		Options* getOptions()const;
		BOOL isNull()const;	
	
	private:
		CString GetTypeName(RenderObjectType rt)const;
	};

public:
	RenderObjectMap();
   
	void remove(CString name);
	BOOL update(CString name,ParameterList&pl,LGAPI&api);
	void updateScene(Scene*scene);
	void put(CString name,Shader*shader);
	void put(CString name,Modifier*modifier);
	void put(CString name,PrimitiveList*primitives);
	void put(CString name,Tesselatable*tesselatable);
	void put(CString name,Instance*instance);
	void put(CString name,LightSource*light);
	void put(CString name,Camera*camera);
	void put(CString name,Options*options);
	RenderObjectMap& operator=(const RenderObjectMap&rom);

	BOOL has(CString name)const;
	BOOL has(CString name,RenderObjectHandle&rh)const;
	Geometry* lookupGeometry(CString name)const;
    Instance* lookupInstance(CString name)const;
	Camera* lookupCamera(CString name)const;
	Options* lookupOptions(CString name)const;
    Shader* lookupShader(CString name)const;
	Modifier* lookupModifier(CString name)const;
	LightSource* lookupLight(CString name)const;   

private:
	BOOL rebuildInstanceList;
	BOOL rebuildLightList;
	hash_map<CString,RenderObjectHandle,
		hash_compare<const char*,CharLess>> renderObjects;
	typedef pair<CString,RenderObjectHandle> CR_PAIR;
};
