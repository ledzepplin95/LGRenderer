#pragma once
#include "primitivelist.h"
#include "Shader.h"
#include "LightSource.h"
#include "color.h"
#include "BoundingBox.h"

class CornellBox : public LightSource,
	public PrimitiveList,public Shader
{	
	LG_DECLARE_DYNCREATE(CornellBox);
public:
	CornellBox();

	BOOL update(ParameterList&pl,LGAPI&api);
	BoundingBox getBounds()const ;
	float getBound(int i)const ;
	BOOL intersects(const BoundingBox&box);
	void prepareShadingState(ShadingState&state);
    void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;
	int getNumSamples()const ;
	void getSamples(ShadingState&state)const ;
	void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&dir,Color&power)const;
	float getPower()const;
	Instance* createInstance();
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives() const;	

private:
	float minX,minY,minZ;
	float maxX,maxY,maxZ;
	Color left,right,top,bottom,back;
	Color radiance;
	int samples;
	float lxmin,lymin,lxmax,lymax;
	float area;
	BoundingBox lightBounds;

	void updateGeometry(const Point3&c0,const Point3&c1);
};
