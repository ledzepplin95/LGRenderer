#pragma once

#include "renderobject.h"
#include "BoundingBox.h"
#include "MovingMatrix4.h"

class Instance : public RenderObject
{
	LG_DECLARE_DYNCREATE(Instance);

public:
	Instance();

	static Instance* createTemporary(PrimitiveList&primitives,const Matrix4&transform, 
		Shader*shader);
	BOOL update(ParameterList&pl,LGAPI&api);
	void updateBounds();	
	void removeShader(Shader*s);
	void removeModifier(Modifier*m);	
	void prepareShadingState(ShadingState&state);
	Instance& operator=(const Instance&i);

	BoundingBox getBounds()const;
	int getNumPrimitives()const;
	void intersect(Ray&r,IntersectionState&state)const;
	BOOL hasGeometry(Geometry*g)const;
	Shader* getShader(int i)const;
	Modifier* getModifier(int i)const;
	Matrix4 getObjectToWorld(float time)const;
	Matrix4 getWorldToObject(float time)const;
	PrimitiveList* getBakingPrimitives()const;
	Geometry* getGeometry()const;
	BOOL operator !=(const Instance&vec) const;	

private:
	MovingMatrix4 o2w;
	MovingMatrix4 w2o;
	BoundingBox bounds;
	Geometry* geometry;
	vector<Shader*> shaders;
	vector<Modifier*> modifiers;
};
