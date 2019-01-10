#include "StdAfx.h"

#include "ImageBasedLight.h"
#include "qmc.h"
#include "LGdef.h"
#include "ShadingState.h"
#include "float.h"
#include "TextureCache.h"
#include "Bitmap.h"
#include "lgapi.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(ImageBasedLight,LightSource)

ImageBasedLight::ImageBasedLight() 
{	
	updateBasis(Vector3(0,0,-1),Vector3(0,1,0));
	numSamples=64;
	numLowSamples=8;
}

void ImageBasedLight::updateBasis(const Vector3&center,const Vector3&up) 
{	
	basis=OrthoNormalBasis::makeFromWV(center,up);
	basis.swapWU();
	basis.flipV();	
}

BOOL ImageBasedLight::update(ParameterList&pl,LGAPI&api)
{
	PrimitiveList::update(pl,api);
	LightSource::update(pl,api);
	Shader::update(pl,api);
	updateBasis(pl.getVector("center",Vector3()),pl.getVector("up",Vector3()));
	numSamples=pl.getInt("samples",numSamples);
	numLowSamples=pl.getInt("lowsamples",numLowSamples);
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		texture=TextureCache::getTexture(api.resolveTextureFilename(fileName),FALSE);
	
	if(texture.getBitmap()==NULL) return FALSE;
	Bitmap* b=texture.getBitmap();
	if(b==NULL)	return FALSE;
	
	if(!fileName.IsEmpty())
	{
		imageHistogram.resize(b->getWidth());
		for(int i=0; i<imageHistogram.size(); i++)
			imageHistogram[i].resize(b->getHeight());				
		colHistogram.resize(b->getWidth());

		float du=1.0f/b->getWidth();
		float dv=1.0f/b->getHeight();
		for(int x=0;x<b->getWidth();x++)
		{
			for(int y=0; y<b->getHeight(); y++) 
			{
				float u=(x+0.5f)*du;
				float v=(y+0.5f)*dv;
				Color c=texture.getPixel(u,v);
				imageHistogram[x][y]=c.getLuminance()*(float)sin(LG_PI*v);
				if(y>0)
					imageHistogram[x][y]+=imageHistogram[x][y-1];
			}
			colHistogram[x]=imageHistogram[x][b->getHeight()-1];
			if(x>0)
				colHistogram[x]+=colHistogram[x-1];
			for(int y=0; y<b->getHeight(); y++)
				imageHistogram[x][y]/=imageHistogram[x][b->getHeight()-1];
		}
		for(int x=0; x<b->getWidth(); x++)
			colHistogram[x]/=colHistogram[b->getWidth()-1];
		jacobian=(2.0f*LG_PI*LG_PI)/(b->getWidth()*b->getHeight());
	}	
	if( pl.getBoolean("fixed",!samples.empty()) ) 
	{		
		samples.resize(numSamples);
		colors.resize(numSamples);
		generateFixedSamples(samples,colors);		
		lowSamples.resize(numLowSamples);
		lowColors.resize(numLowSamples);
		generateFixedSamples(lowSamples,lowColors);
	} 
	else 
	{		
		samples.clear();
		lowSamples.clear();
		colors.clear();
		lowColors.clear();
	}

	return TRUE;
}

void ImageBasedLight::generateFixedSamples(vector<Vector3>&samples,vector<Color>&colors) 
{
	for(int i=0; i<samples.size(); i++) 
	{
		double randX=(double)i/(double)samples.size();
		double randY=QMC::halton(0,i);
		int x=0;
		while( randX>=colHistogram[x] && x<colHistogram.size()-1 )
			x++;
		vector<float> rowHistogram=imageHistogram[x];
		int y=0;
		while( randY>=rowHistogram[y] && y<rowHistogram.size()-1 )
			y++;	
		float u=(float)( (x==0)?
			(randX/colHistogram[0]):((randX-colHistogram[x-1]) 
			/(colHistogram[x]-colHistogram[x-1])) );
		float v=(float)( (y==0)?
			(randY/rowHistogram[0]):((randY-rowHistogram[y-1]) 
			/(rowHistogram[y]-rowHistogram[y-1])) );

		float px=((x==0)?colHistogram[0] 
		: (colHistogram[x]-colHistogram[x-1]));
		float py=((y==0)?rowHistogram[0] 
		: (rowHistogram[y]-rowHistogram[y-1]));

		float su=(x+u)/colHistogram.size();
		float sv=(y+v)/rowHistogram.size();

		float invP=(float)sin(sv*LG_PI)*jacobian/(numSamples*px*py);
		samples[i]=getDirection(su,sv);
		basis.transform(samples[i]);
		colors[i]=texture.getPixel(su,sv).mul(invP);
	}
}

void ImageBasedLight::prepareShadingState(ShadingState&state)
{
	if(state.includeLights())
		state.setShader(this);
}

void ImageBasedLight::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	if(r.getMax()==FLT_MAX)
		state.setIntersection(0);
}

int ImageBasedLight::getNumPrimitives() const
{
	return 1;
}

float ImageBasedLight::getPrimitiveBound(int primID,int i) const
{
	return 0.0f;
}

BoundingBox ImageBasedLight::getWorldBounds(const Matrix4&o2w) const
{
	return BoundingBox(0.0f);
}

PrimitiveList* ImageBasedLight::getBakingPrimitives() const
{
	return NULL;
}

int ImageBasedLight::getNumSamples() const
{
	return numSamples;
}

void ImageBasedLight::getSamples(ShadingState&state)const 
{
	if(samples.empty())
	{
		int n=state.getDiffuseDepth()>0?1:numSamples;
		for(int i=0; i<n; i++) 
		{
			double randX=state.getRandom(i,0,n);
			double randY=state.getRandom(i,1,n);
			int x=0;
			while( randX>=colHistogram[x] && x<colHistogram.size()-1 )
				x++;
			vector<float> rowHistogram=imageHistogram[x];
			int y=0;
			while( randY>=rowHistogram[y] && y<rowHistogram.size()-1 )
				y++;		
			float u=(float)( (x==0)?(randX/colHistogram[0]) 
				:((randX-colHistogram[x-1])/(colHistogram[x]-colHistogram[x-1])) );
			float v=(float)( (y==0)?(randY/rowHistogram[0]) 
				:((randY-rowHistogram[y-1])/(rowHistogram[y]-rowHistogram[y-1])) );

			float px=( (x==0)?colHistogram[0]:(colHistogram[x]-colHistogram[x-1]) );
			float py=( (y==0)?rowHistogram[0]:(rowHistogram[y]-rowHistogram[y-1]) );

			float su=(x+u)/colHistogram.size();
			float sv=(y+v)/rowHistogram.size();
			float invP=(float)sin(sv*LG_PI)*jacobian/(n*px*py);
			Vector3 dir=getDirection(su,sv);
			basis.transform(dir);
			if( Vector3::dot(dir,state.getGeoNormal())>0.0f ) 
			{
				LightSample* dest=new LightSample();
				dest->setShadowRay(Ray(state.getPoint(),dir));
				dest->getShadowRay().setMax(FLT_MAX);
				Color radiance=texture.getPixel(su,sv);
				dest->setRadiance(radiance,radiance);
				dest->getDiffuseRadiance().mul(invP);
				dest->getSpecularRadiance().mul(invP);
				dest->traceShadow(state);
				state.addSample(dest);
			}
		}
	} 
	else
	{
		if( state.getDiffuseDepth()>0 )
		{
			for(int i=0; i<numLowSamples; i++) 
			{
				if( Vector3::dot(lowSamples[i],state.getGeoNormal())>0.0f 
					&& Vector3::dot(lowSamples[i],state.getNormal())>0.0f )
				{
					LightSample* dest=new LightSample();
					dest->setShadowRay(Ray(state.getPoint(),lowSamples[i]));
					dest->getShadowRay().setMax(FLT_MAX);
					dest->setRadiance(lowColors[i],lowColors[i]);
					dest->traceShadow(state);
					state.addSample(dest);
				}
			}
		} 
		else 
		{
			for(int i=0; i<numSamples; i++) 
			{
				if( Vector3::dot(samples[i],state.getGeoNormal())>0.0f
					&& Vector3::dot(samples[i],state.getNormal())>0.0f )
				{
					LightSample* dest=new LightSample();
					dest->setShadowRay(Ray(state.getPoint(),samples[i]));
					dest->getShadowRay().setMax(FLT_MAX);
					dest->setRadiance(colors[i],colors[i]);
					dest->traceShadow(state);
					state.addSample(dest);
				}
			}
		}
	}
}

void ImageBasedLight::getPhoton(double randX1,double randY1,double randX2,
			   double randY2,Point3&p,Vector3&dir,Color&power)const 
{
}

Color ImageBasedLight::getRadiance(ShadingState&state)const
{	
	return state.includeLights()?
		getColor(basis.untransform(state.getRay().getDirection(),
		Vector3())):Color::BLACK;
}

Color ImageBasedLight::getColor(const Vector3&dir) const
{
	float u,v;	
	double phi=0.0,theta=0.0;
	phi=acos(dir.y);
	theta=atan2(dir.z,dir.x);
	u=0.5f-0.5*theta/LG_PI;
	v=(float)(phi/LG_PI);

	return texture.getPixel(u,v);
}

Vector3 ImageBasedLight::getDirection(float u,float v) const
{
	Vector3 dest;
	double phi=0.0,theta=0.0;
	theta=u*2.0*LG_PI;
	phi=v*LG_PI;
	double sin_phi=sin(phi);
	dest.x=(float)(-sin_phi*cos(theta));
	dest.y=(float)cos(phi);
	dest.z=(float)(sin_phi*sin(theta));

	return dest;
}

void ImageBasedLight::scatterPhoton(ShadingState&state,Color&power)const
{
}

float ImageBasedLight::getPower() const
{
	return 0.0f;
}

Instance* ImageBasedLight::createInstance()
{
	return Instance::createTemporary(*this,Matrix4(),this);	
}
