#pragma once
#include "trianglemesh.h"
#include "shader.h"
#include "LightSource.h"
#include "color.h"

class TriangleMeshLight :
	public LightSource, public TriangleMesh, public Shader
{
	LG_DECLARE_DYNCREATE(TriangleMeshLight);

public:
	TriangleMeshLight();

	BOOL update(ParameterList&pl,LGAPI&api);

	Color getRadiance(ShadingState&state)const;
    void scatterPhoton(ShadingState&state,Color&power)const;
	Instance* createInstance();
	int getNumSamples() const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&dir,Color&power)const;
	float getPower()const;
	void getSamples(ShadingState&state)const;

private:
	Color radiance;
	int numSamples;
	vector<float> areas;
	float totalArea;
	vector<Vector3> ngs;

	BOOL intersectTriangleKensler(int tri3,Ray&r)const;	
};
