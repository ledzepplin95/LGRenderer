#include "StdAfx.h"

#include "WireframeShader.h"
#include "ShadingState.h"
#include "LGDef.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(WireframeShader,Shader)

WireframeShader::WireframeShader()
{
	lineColor=Color::BLACK;
	fillColor=Color::WHITE;	
	width=(float)(LG_PI*0.5/4096);
	cosWidth=(float)cos(width);
}

BOOL WireframeShader::update(ParameterList&pl,LGAPI&api) 
{
	lineColor=pl.getColor("line",lineColor);
	fillColor=pl.getColor("fill",fillColor);
	width=pl.getFloat("width",width);
	cosWidth=(float)cos(width);

	return TRUE;
}

Color WireframeShader::getFillColor(ShadingState&state)const
{
	return fillColor;
}

Color WireframeShader::getLineColor(ShadingState&state)const
{
	return lineColor;
}

Color WireframeShader::getRadiance(ShadingState&state)const 
{
	vector<Point3> p;
	p.resize(3);
	if( !state.getTrianglePoints(p) )
		return getFillColor(state);
	Point3 center=state.getPoint();
	Matrix4 w2c=state.getWorldToCamera();
	center=w2c.transformP(center);
	for(int i=0; i<3; i++)
		p[i]=w2c.transformP(state.transformObjectToWorld(p[i]));
	float cn=1.0f/(float)sqrt(center.x*center.x 
		+center.y*center.y+center.z*center.z);
	for(int i=0, i2=2; i<3; i2=i, i++)
	{	
		float t=(center.x-p[i].x)*(p[i2].x-p[i].x);
		t+=(center.y-p[i].y)*(p[i2].y-p[i].y);
		t+=(center.z-p[i].z)*(p[i2].z-p[i].z);
		t/=p[i].distanceToSquared(p[i2]);
		float projx=(1.0f-t)*p[i].x+t*p[i2].x;
		float projy=(1.0f-t)*p[i].y+t*p[i2].y;
		float projz=(1.0f-t)*p[i].z+t*p[i2].z;
		float n=1.0f/(float)sqrt(projx*projx+projy*projy+projz*projz);	
		float dot=projx*center.x+projy*center.y+projz*center.z;
		if(dot*n*cn>=cosWidth)
			return getLineColor(state);
	}

	return getFillColor(state);
}

void WireframeShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}