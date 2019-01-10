#pragma once

#include "lightsource.h"
#include "IrregularSpectralCurve.h"
#include "RegularSpectralCurve.h"
#include "primitivelist.h"
#include "shader.h"
#include "OrthoNormalBasis.h"
#include "Color.h"

class SunSkyLight : public LightSource,
	public PrimitiveList,public Shader
{
	LG_DECLARE_DYNCREATE(SunSkyLight);

public:
    SunSkyLight();

	BOOL update(ParameterList&pl,LGAPI&api);

	int getNumSamples()const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&dir,Color&power)const;
	float getPower()const;
	void getSamples(ShadingState&state)const;
	PrimitiveList* getBakingPrimitives()const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	void prepareShadingState(ShadingState&state);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;
	Instance* createInstance();

private:
    SpectralCurve* computeAttenuatedSunlight(float theta,float turbidity);
	double perezFunction(const vector<double>&lam,double theta,
		double gamma,double lvz)const;
	void initSunSky();
	Color getSkyRGB(Vector3&dir)const;
	Vector3 getDirection(float u,float v)const;

private:
	int numSkySamples;
	OrthoNormalBasis basis;
	BOOL groundExtendSky;
	Color groundColor;
	Vector3 sunDirWorld;
	float turbidity;
	Vector3 sunDir;
	SpectralCurve* sunSpectralRadiance;
	Color sunColor;
	float sunTheta;
	double zenithY, zenithx, zenithy;
	vector<double> perezY;
	vector<double> perezx;
	vector<double> perezy;
	float jacobian;
	vector<float> colHistogram;
	vector<vector<float>> imageHistogram;

	static float solAmplitudes[];

	static RegularSpectralCurve solCurve;

	static float k_oWavelengths [];
	static float k_oAmplitudes[];
	static float k_gWavelengths[];
	static float k_gAmplitudes [];
	static float k_waWavelengths[];
	static float k_waAmplitudes[];

	static IrregularSpectralCurve k_oCurve;
	static IrregularSpectralCurve k_gCurve;
	static IrregularSpectralCurve k_waCurve;	
};
