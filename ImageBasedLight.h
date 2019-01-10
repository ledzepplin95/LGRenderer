#pragma once

#include "vector3.h"
#include "color.h"
#include "texture.h"
#include "lightsource.h"
#include "PrimitiveList.h"
#include "shader.h"
#include "orthonormalbasis.h"

class ImageBasedLight : 
	public LightSource, public PrimitiveList, public Shader
{
	LG_DECLARE_DYNCREATE(ImageBasedLight);

public:
	ImageBasedLight(void);

    void updateBasis(const Vector3&center,const Vector3&up);
	BOOL update(ParameterList&pl,LGAPI&api);
    void generateFixedSamples(vector<Vector3>&samples,vector<Color>&colors);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;
	int getNumSamples()const;
	void getSamples(ShadingState&state)const;
	void getPhoton(double randX1,double randY1,double randX2,
		double randY2,Point3&p,Vector3&dir,Color&power)const;
	float getPower()const;
	Instance* createInstance();
	Color getColor(const Vector3&dir)const;
	Vector3 getDirection(float u,float v)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	

private:
	Texture texture;
	OrthoNormalBasis basis;
	int numSamples;
	int numLowSamples;
	float jacobian;
	vector<float> colHistogram;
	vector<vector<float>> imageHistogram;
	vector<Vector3> samples;
	vector<Vector3> lowSamples;
	vector<Color> colors;
	vector<Color> lowColors;	
};
