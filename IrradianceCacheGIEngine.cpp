#include "StdAfx.h"

#include "IrradianceCacheGIEngine.h"
#include "MathUtil.h"
#include "strutil.h"
#include "Scene.h"
#include "float.h"
#include "PluginRegistry.h"
#include "shadingstate.h"
#include "color.h"
#include "options.h"
#include "BoundingBox.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(IrradianceCacheGIEngine,GIEngine)

IrradianceCacheGIEngine::Sample::Sample()
{
    pix=piy=piz=0.0f;
	nix=niy=niz=0.0f;
	invR0=0.0f;
	irr.set(0,0,0);
	next=NULL;
}

IrradianceCacheGIEngine::Sample::Sample(const Point3&p,const Vector3&n) 
{
	pix=p.x;
	piy=p.y;
	piz=p.z;
	Vector3 ni(n);
	ni.normalize();
	nix=ni.x;
	niy=ni.y;
	niz=ni.z;			
}

IrradianceCacheGIEngine::Sample::Sample(const Point3&p,
										const Vector3&n,float r0,const Color&i)
{
	pix=p.x;
	piy=p.y;
	piz=p.z;
	Vector3 ni(n);
	ni.normalize();
	nix=ni.x;
	niy=ni.y;
	niz=ni.z;
	invR0=1.0f/r0;
	irr=i;			
}

IrradianceCacheGIEngine::Node::Node(const Point3&c,float sLength)
{
	children.resize(8);
	for(int i=0;i<8;i++)
		children[i]=NULL;
	center.set(c);
	sideLength=sLength;
	halfSideLength=0.5f*sLength;
	quadSideLength=0.5f*halfSideLength;			
}

IrradianceCacheGIEngine::Node::~Node()
{
	Sample*p=NULL;
	Sample*q=NULL;
	if(first!=NULL)
	{
		p=first->next;
		while(p) 
		{
			q=p->next;
			delete p;
			if(q==NULL) break;
			p=q;
		}
		first->next=NULL;
		delete first;
		first=NULL;
	}

	for(int i=0; i<children.size(); i++)
	{
		if(children[i]) 
			delete children[i];
	}
}

BOOL IrradianceCacheGIEngine::Node::isInside(const Point3&p) const
{
	return (fabs(p.x-center.x)<halfSideLength) 
		&& (fabs(p.y-center.y)<halfSideLength)
		&& (fabs(p.z-center.z)<halfSideLength);
}

float IrradianceCacheGIEngine::Node::find(Sample&x) const
{
	float weight=0.0f;
	for(Sample* s=first; s!=NULL; s=s->next)
	{
		float c2=1.0f-(x.nix*s->nix+x.niy*s->niy+x.niz*s->niz);
		float d2=(x.pix-s->pix)*(x.pix-s->pix)+ 
			(x.piy-s->piy)*(x.piy-s->piy)+(x.piz-s->piz)*(x.piz-s->piz);
		if( c2>(tolerance*tolerance) || d2>(maxSpacing*maxSpacing) )
			continue;
		float invWi=(float)(sqrt(d2)*s->invR0+sqrt(max(c2,0)));
		if( invWi<tolerance || d2<(minSpacing*minSpacing) )
		{
			float wi=min(1e10f,1.0f/invWi);					
			x.irr.madd(wi,s->irr);					
			weight+=wi;
		}
	}
	for(int i=0; i<8; i++)
		if( (children[i]!=NULL)
			&& (fabs(children[i]->center.x-x.pix)<=halfSideLength) 
			&& (fabs(children[i]->center.y-x.piy)<=halfSideLength)
			&& (fabs(children[i]->center.z-x.piz)<=halfSideLength) )
			weight+=children[i]->find(x);

	return weight;
}

IrradianceCacheGIEngine::IrradianceCacheGIEngine()
{
    root=NULL;
	globalPhotonMap=NULL;
}

IrradianceCacheGIEngine::~IrradianceCacheGIEngine()
{
    if(root) delete root;
}

BOOL IrradianceCacheGIEngine::init(Options*options,Scene*scene)
{
	samples=options->getInt("gi.irr-cache.samples",256);
	tolerance=options->getFloat("gi.irr-cache.tolerance",0.05f);
	invTolerance=1.0f/tolerance;
	minSpacing=options->getFloat("gi.irr-cache.min_spacing",0.05f);
	maxSpacing=options->getFloat("gi.irr-cache.max_spacing",5.00f);
	root=NULL;	
	globalPhotonMap=PluginRegistry::globalPhotonMapPlugins.createObject(
		options->getString("gi.irr-cache.gmap",CString()),2);	
	samples=max(0,samples);
	minSpacing=max(0.001f,minSpacing);
	maxSpacing=max(0.001f,maxSpacing);	
	StrUtil::PrintPrompt("Irradiance cache设置:");
	StrUtil::PrintPrompt("  * 采样: %d",samples);
	if(tolerance<=0.0f)
		StrUtil::PrintPrompt("  * 容差关闭");
	else
		StrUtil::PrintPrompt("  * 容差: %.3f",tolerance);
	StrUtil::PrintPrompt("  * 间隔: %.3f至%.3f",minSpacing,maxSpacing);	
	Vector3 ext=scene->getBounds().getExtents();
	root=new Node(scene->getBounds().getCenter(), 
		1.0001f*MathUtil::mu_max(ext.x,ext.y,ext.z));
	
	return scene->calculatePhotons(globalPhotonMap,"global",0,options);
}

Color IrradianceCacheGIEngine::getGlobalRadiance(ShadingState&state)const
{
	if(globalPhotonMap==NULL) 	
		return state.getShader()->getRadiance(state);		
    else
		return globalPhotonMap->getRadiance(state.getPoint(),state.getNormal());	
}

Color IrradianceCacheGIEngine::getIrradiance(ShadingState&state,
											 const Color&diffuseReflectance) const
{
	if(samples<=0) return Color::BLACK;

	if(state.getDiffuseDepth()>0) 
	{	
		float xi=(float)state.getRandom(0,0,1);
		float xj=(float)state.getRandom(0,1,1);
		float phi=2.0f*xi*LG_PI;
		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);
		float sinTheta=(float)sqrt(xj);
		float cosTheta=(float)sqrt(1.0f-xj);
		Vector3 w;
		w.x=cosPhi*sinTheta;
		w.y=sinPhi*sinTheta;
		w.z=cosTheta;
		OrthoNormalBasis onb=state.getBasis();
		onb.transform(w);
		Ray r(state.getPoint(),w);
		ShadingState* temp=state.traceFinalGather(r,0);
		Color c=getGlobalRadiance(*temp).copy().mul((float)LG_PI);
		delete temp;

		return c;
	}
	
	Color irr=getIrradiance(state.getPoint(),state.getNormal());		
	irr=Color::BLACK;
	OrthoNormalBasis onb=state.getBasis();
	float invR=0.0f;
	float minR=FLT_MAX;
	Vector3 w;
	for(int i=0; i<samples; i++) 
	{
		float xi=(float)state.getRandom(i,0,samples);
		float xj=(float)state.getRandom(i,1,samples);
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
		ShadingState* temp=state.traceFinalGather(r,i);
		minR=min(r.getMax(),minR);
		invR+=1.0f/r.getMax();
		temp->getInstance()->prepareShadingState(*temp);
		irr.add(getGlobalRadiance(*temp));
		delete temp;
	}
	irr.mul((float)LG_PI/samples);
	invR=samples/invR;	
	insert(state.getPoint(),state.getNormal(),invR,irr);				

	return irr;
}

void IrradianceCacheGIEngine::insert(const Point3&p,const Vector3&n,float r0,const Color&i)const 
{
	if(tolerance<=0.0f) return;
	Node* node=root;
	r0=MathUtil::clamp(r0*tolerance,minSpacing,maxSpacing)*invTolerance;
	if(root->isInside(p)) 
	{
		while( node->sideLength>=(4.0*r0*tolerance) ) 
		{
			int k=0;
			k |= ( p.x>node->center.x )?1:0;
			k |= ( p.y>node->center.y )?2:0;
			k |= ( p.z>node->center.z )?4:0;
			if(node->children[k]==NULL) 
			{
				Point3 c(node->center);
				c.x+=( (k&1)==0 )?-node->quadSideLength:node->quadSideLength;
				c.y+=( (k&2)==0 )?-node->quadSideLength:node->quadSideLength;
				c.z+=( (k&4)==0 )?-node->quadSideLength:node->quadSideLength;
				node->children[k]=new Node(c,node->halfSideLength);
			}
			node=node->children[k];
		}
	}
	Sample* s=new Sample(p,n,r0,i);
	s->next=node->first;
	node->first=s;
}

Color IrradianceCacheGIEngine::getIrradiance(const Point3&p,const Vector3&n)const 
{
	if(tolerance<=0.0f) return Color();
	Sample x(p,n);
	float w=root->find(x);

	return x.irr.mul(1.0f/w);
}