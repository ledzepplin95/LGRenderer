#include "StdAfx.h"

#include "InstantGI.h"
#include "ShadingState.h"
#include "LGdef.h"
#include "strutil.h"
#include "Scene.h"
#include "shader.h"
#include "options.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(InstantGI,GIEngine)

LG_IMPLEMENT_DYNCREATE(PointLightStore,PhotonStore)

IGIPointLight::IGIPointLight()
{
}

IGIPointLight::IGIPointLight(const IGIPointLight &pl)
{
	if(this==&pl) return;

	p=pl.p;
	n=pl.n;
	power=pl.power;  
}

IGIPointLight& IGIPointLight::operator=(const IGIPointLight&pl)
{
	if(this==&pl) return *this;

    p=pl.p;
	n=pl.n;
	power=pl.power;    

	return *this;
}

PointLightStore::PointLightStore()
{
	nPhotons=0;
}

PointLightStore::PointLightStore(int n)
{
	nPhotons=n;
}

void PointLightStore::prepare(Options*options,const BoundingBox&sceneBounds) 
{
}

void PointLightStore::store(ShadingState&state,const Vector3&dir,
							const Color&power,const Color&diffuse)
{
	state.faceforward();
	IGIPointLight vpl;
	vpl.p=state.getPoint();
	vpl.n=state.getNormal();
	vpl.power=power;
	cs.Lock();
	virtualLights.push_back(vpl);
	cs.Unlock();
}

void PointLightStore::init() 
{
}

int PointLightStore::numEmit()const
{
	return nPhotons;
}

BOOL PointLightStore::allowDiffuseBounced() const
{
	return TRUE;
}

BOOL PointLightStore::allowReflectionBounced() const
{
	return TRUE;
}

BOOL PointLightStore::allowRefractionBounced()const
{
	return TRUE;
}

Color InstantGI::getGlobalRadiance(ShadingState&state)const 
{
	Point3 p=state.getPoint();
	Vector3 n=state.getNormal();
	int s=(int)(state.getRandom(0,1,1)*numSets);
	float maxAvgPow=0.0f;
	float minDist=1.0f;
	Color pow;
	for(int i=0; i<virtualLights[s].size(); i++) 
	{
		maxAvgPow=max(maxAvgPow,virtualLights[s].at(i).power.getAverage());
		if( Vector3::dot(n,virtualLights[s].at(i).n)>0.9f )
		{
			float d=virtualLights[s].at(i).p.distanceToSquared(p);
			if(d<minDist) 
			{
				pow=virtualLights[s].at(i).power;
				minDist=d;
			}
		}
	}

	return  pow==Color()?Color::BLACK:pow.copy().mul(1.0f/maxAvgPow);
}

BOOL InstantGI::init(Options*options,Scene*scene) 
{
	numPhotons=options->getInt("gi.igi.samples",64);
	numSets=options->getInt("gi.igi.sets",1);
	c=options->getFloat("gi.igi.c",0.00003f);
	numBias=options->getInt("gi.igi.bias_samples",0);
	virtualLights.clear();
	if(numSets<1) numSets=1;
	StrUtil::PrintPrompt("IGI设置:");
	StrUtil::PrintPrompt("  * 采样:       %d",numPhotons);
	StrUtil::PrintPrompt("  * 集合:       %d",numSets);
	StrUtil::PrintPrompt("  * 偏置界限:   %f",c);
	StrUtil::PrintPrompt("  * 偏置光线数: %d",numBias);
	virtualLights.resize(numSets);
	if(numPhotons>0) 
	{
		for(int i=0,seed=0; i<virtualLights.size(); i++,seed+=numPhotons)
		{			
			PointLightStore*m=new PointLightStore(numPhotons);
			LGMemoryAllocator::lg_mem_pointer(m);
			if(!scene->calculatePhotons(m,"虚拟",seed,options))
				return FALSE;		
			virtualLights[i]=m->virtualLights;
			StrUtil::PrintPrompt("为%d个集合之%d已存储%d虚拟点光源",
				numSets,i+1,virtualLights[i].size());			
		}
	}

	return TRUE;
}

Color InstantGI::getIrradiance(ShadingState&state,const Color&diffuseReflectance)const 
{
	float b=(float)LG_PI*c/diffuseReflectance.getMax();
	Color irr=Color::BLACK;
	Point3 p=state.getPoint();
	Vector3 n=state.getNormal();
	int s=(int)(state.getRandom(0,1,1)*numSets);
	for(int i=0; i<virtualLights[s].size(); i++)
	{
		const IGIPointLight& vpl=virtualLights[s].at(i);
		Ray r(p,vpl.p);
		float dotNlD=-(r.dx*vpl.n.x+r.dy*vpl.n.y+r.dz*vpl.n.z);
		float dotND=r.dx*n.x+r.dy*n.y+r.dz*n.z;
		if( dotNlD>0.0f && dotND>0.0f )
		{
			float r2=r.getMax()*r.getMax();
			Color opacity=state.traceShadow(r);
			Color power=Color::blend(vpl.power,Color::BLACK,opacity);
			float g=(dotND*dotNlD)/r2;
			irr.madd(0.25f*min(g,b),power);
		}
	}	
	int nb=(state.getDiffuseDepth()==0 || numBias<=0)?numBias:1;
	if(nb<=0) return irr;
	OrthoNormalBasis onb=state.getBasis();
	Vector3 w;
	float scale=(float)LG_PI/nb;
	for(int i=0; i<nb; i++) 
	{
		float xi=(float)state.getRandom(i,0,nb);
		float xj=(float)state.getRandom(i,1,nb);
		float phi=2.0f*xi*LG_PI;
		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);
		float sinTheta=(float)sqrt(xj);
		float cosTheta=(float)sqrt(1.0f-xj);
		w.x=cosPhi*sinTheta;
		w.y=sinPhi*sinTheta;
		w.z=cosTheta;
		onb.transform(w);
		Ray r(state.getPoint(),w);
		r.setMax((float)sqrt(cosTheta/b));
		ShadingState* temp=state.traceFinalGather(r,i);	
        if(!temp->isNull())
		{
			temp->getInstance()->prepareShadingState(*temp);	
			if(temp->getShader()!=NULL)
			{
				float dist=temp->getRay().getMax();
				float r2=dist*dist;
				float cosThetaY=-Vector3::dot(w,temp->getNormal());
				if(cosThetaY>0.0f) 
				{
					float g=(cosTheta*cosThetaY)/r2;					
					if(g>b)
						irr.madd(scale*(g-b)/g,temp->getShader()->getRadiance(*temp));
				}	
			}	
		}
		delete temp;
	}

	return irr;
}



