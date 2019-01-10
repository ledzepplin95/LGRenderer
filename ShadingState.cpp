#include "StdAfx.h"

#include "ShadingState.h"
#include "qmc.h"
#include "lgdef.h"
#include "Camera.h"
#include "Scene.h"
#include "MathUtil.h"
#include "TriangleMesh.h"
#include "geometry.h"
#include "Instance.h"
#include "LightServer.h"
#include "PhotonStore.h"

ShadingState* ShadingState::createPhotonState(const Ray&rr,IntersectionState*is,int ii,
									 PhotonStore*mm,LightServer*ls) 
{
	ShadingState *s=new ShadingState(ShadingState(),is,rr,ii,(int)4);
	s->server=ls;
	s->ps=mm;

	return s;
}

ShadingState* ShadingState::createState(IntersectionState*is,float rrx,float rry, 
								float ttime,const Ray&rr,int ii,int dd,LightServer*ls) 
{
	ShadingState* s=new ShadingState(ShadingState(),is,rr,ii,dd);
	s->server=ls;
	s->rx=rrx;
	s->ry=rry;
	s->time=ttime;

	return s;
}

ShadingState* ShadingState::createDiffuseBounceState(const ShadingState&pre,const Ray&rr,int ii) 
{
	ShadingState* s=new ShadingState(pre,pre.iState,rr,ii,2);
	s->diffuseDepth++;

	return s;
}

ShadingState* ShadingState::createGlossyBounceState(const ShadingState&pre,const Ray&rr,int ii)
{
	ShadingState* s=new ShadingState(pre,pre.iState,rr,ii,2);
	s->incluLights=FALSE;
	s->incluSpecular=FALSE;
	s->reflectionDepth++;

	return s;
}

ShadingState* ShadingState::createReflectionBounceState(const ShadingState&pre,const Ray&rr,int ii) 
{
	ShadingState* s=new ShadingState(pre,pre.iState,rr,ii,2);
	s->reflectionDepth++;

	return s;
}

ShadingState* ShadingState::createRefractionBounceState(const ShadingState&pre,const Ray&rr,int ii) 
{
	ShadingState* s=new ShadingState(pre,pre.iState,rr,ii,2);
	s->refractionDepth++;

	return s;
}

ShadingState* ShadingState::createFinalGatherState(const ShadingState&ss,const Ray&rr,int ii) 
{
	ShadingState* finalGatherState=new ShadingState(ss,ss.iState,rr,ii,2);
	finalGatherState->diffuseDepth++;
	finalGatherState->incluLights=FALSE;
	finalGatherState->incluSpecular=FALSE;

	return finalGatherState;
}

ShadingState::ShadingState()
{
	instance=NULL;
	shader=NULL;
	modifier=NULL;
	lightSample=NULL;
	server=NULL;
	ps=NULL;
	iState=NULL;
	rx=ry=time=0.0f;
	cosND=MathUtil::NotANumber;
	bias=0.0f;
	behind=FALSE;
	hitU=hitV=hitW=0.0f;
	primitiveID=0;
	d=0;
	i=0;
	qmcD0I=0.0;
	qmcD1I=0.0;
	diffuseDepth=0;	
	refractionDepth=0;
	incluLights=FALSE;
	incluSpecular=FALSE;
	reflectionDepth=-50001;
}

ShadingState::ShadingState(const ShadingState&pre,IntersectionState*is,
						   const Ray&rr,int ii,int dd)
{
	shader=NULL;
	modifier=NULL;
	lightSample=NULL;
	server=NULL;
	ps=NULL;
	iState=is;

	r=rr;	
	i=ii;
	d=dd;
	time=is->time;
	instance=is->instance;
	primitiveID=is->id;
	hitU=is->u;
	hitV=is->v;
	hitW=is->w;	
	o2w=instance->getObjectToWorld(time);
	w2o=instance->getWorldToObject(time);	

	if(pre.isNull())
	{
		diffuseDepth=0;
		reflectionDepth=0;
        refractionDepth=0;
	}
	else
	{
		diffuseDepth=pre.diffuseDepth;
		reflectionDepth=pre.reflectionDepth;
		refractionDepth=pre.refractionDepth;
		server=pre.server;
		ps=pre.ps;
		rx=pre.rx;
		ry=pre.ry;
		i+=pre.i;
		d+=pre.d;
	}	
	
	behind=FALSE;
	cosND=MathUtil::NotANumber;
	incluLights=incluSpecular=TRUE;
	qmcD0I=QMC::halton(d,i);
	qmcD1I=QMC::halton(d+1,i);
	result.set(Color::BLACK);
	bias=0.001f;
}

ShadingState::~ShadingState()
{
	LightSample*p=NULL;
	LightSample*q=NULL;
	if(lightSample!=NULL)
	{
		p=lightSample->next;
		while(p) 
		{
			q=p->next;
			delete p;
			if(q==NULL) break;
			p=q;
		}
		lightSample->next=NULL;
		delete lightSample;
		lightSample=NULL;
	}
}

BOOL ShadingState::isNull()const
{
	return reflectionDepth==-50001;
}

void ShadingState::setRay(const Ray&rr) 
{
	r=rr;
}

void ShadingState::init() 
{
	p.set(0,0,0);
	n.set(0,0,0);
	tex.set(0,0);
	ng.set(0,0,0);   
}

Color ShadingState::shade() 
{
	return server->shadeHit(*this);
}

void ShadingState::correctShadingNormal() 
{
	if(Vector3::dot(n,ng)<0.0f)
	{
		n.negate();
		basis.flipW();
	}
}

void ShadingState::faceforward()
{	
	if(r.dot(ng)<0.0f)
	{
	} 
	else
	{		
		ng.negate();
		n.negate();
		basis.flipW();
		behind=TRUE;
	}
	cosND=max(-r.dot(n),0.0f);	
	if( fabs(ng.x)>fabs(ng.y) 
		&& fabs(ng.x)>fabs(ng.z) )
		bias=max(bias,25*MathUtil::ulp(fabs(p.x)));
	else if( fabs(ng.y)>fabs(ng.z) )
		bias=max(bias,25*MathUtil::ulp(fabs(p.y)));
	else
		bias=max(bias,25*MathUtil::ulp(fabs(p.z)));
	p.x+=bias*ng.x;
	p.y+=bias*ng.y;
	p.z+=bias*ng.z;
}

float ShadingState::getRasterX() const
{
	return rx;
}

float ShadingState::getRasterY() const
{
	return ry;
}

float ShadingState::getCosND() const
{
	return cosND;
}

BOOL ShadingState::isBehind() const
{
	return behind;
}

IntersectionState* ShadingState::getIntersectionState()const
{
	return iState;
}

float ShadingState::getU() const
{
	return hitU;
}

float ShadingState::getV() const
{
	return hitV;
}

float ShadingState::getW() const
{
	return hitW;
}

Instance* ShadingState::getInstance() const
{
	return instance;
}

int ShadingState::getPrimitiveID() const
{
	return primitiveID;
}

Point3 ShadingState::transformObjectToWorld(const Point3&pp) const
{
	return  o2w==Matrix4()?pp:o2w.transformP(pp);
}

Point3 ShadingState::transformWorldToObject(const Point3&pp)const 
{
	return  w2o==Matrix4()?pp:w2o.transformP(pp);
}

Vector3 ShadingState::transformNormalObjectToWorld(const Vector3&nn)const 
{
	return  w2o==Matrix4()?nn:w2o.transformTransposeV(nn);
}

Vector3 ShadingState::transformNormalWorldToObject(const Vector3&nn)const
{
	return  o2w==Matrix4()?nn:o2w.transformTransposeV(nn);
}

Vector3 ShadingState::transformVectorObjectToWorld(const Vector3&v)const
{
	return  o2w==Matrix4()?v:o2w.transformV(v);
}

Vector3 ShadingState::transformVectorWorldToObject(const Vector3&v) const
{
	return  w2o==Matrix4()?v:w2o.transformV(v);
}

void ShadingState::setResult(const Color&c)
{
	result=c;
}

Color ShadingState::getResult() const
{
	return result;
}

LightServer* ShadingState::getLightServer() const
{
	return server;
}

void ShadingState::addSample(LightSample* sample) 
{	
	sample->next=lightSample;
	lightSample=sample;
}

double ShadingState::getRandom(int j,int dim)const
{
	switch(dim)
	{
	case 0:
		return QMC::mod1(qmcD0I+QMC::halton(0,j));
	case 1:
		return QMC::mod1(qmcD1I+QMC::halton(1,j));
	default:
		return QMC::mod1(QMC::halton(d+dim,i)+QMC::halton(dim,j));
	}
}

double ShadingState::getRandom(int j,int dim,int nn) const
{
	switch(dim)
	{
	case 0:
		return QMC::mod1(qmcD0I+(double)j/(double)nn);
	case 1:
		return QMC::mod1(qmcD1I+QMC::halton(0,j));
	default:
		return QMC::mod1(QMC::halton(d+dim,i)+QMC::halton(dim-1,j));
	}
}

BOOL ShadingState::includeLights() const
{
	return incluLights;
}

BOOL ShadingState::includeSpecular() const
{
	return incluSpecular;
}

Shader* ShadingState::getShader() const
{
	return shader;
}

void ShadingState::setShader(Shader*ss)
{
	shader=ss;
}

Modifier* ShadingState::getModifier() const
{
	return modifier;
}

void ShadingState::setModifier(Modifier*mm) 
{
	modifier=mm;
}

int ShadingState::getDepth() const
{
	return diffuseDepth+reflectionDepth+refractionDepth;
}

int ShadingState::getDiffuseDepth() const
{
	return diffuseDepth;
}

int ShadingState::getReflectionDepth() const
{
	return reflectionDepth;
}

int ShadingState::getRefractionDepth() const 
{
	return refractionDepth;
}

Point3 ShadingState::getPoint() const
{
	return p;
}

Vector3 ShadingState::getNormal()const
{
	return n;
}

Point2 ShadingState::getUV() const
{
	return tex;
}

Vector3 ShadingState::getGeoNormal()const
{
	return ng;
}

OrthoNormalBasis ShadingState::getBasis() const
{
	return basis;
}

void ShadingState::setBasis(const OrthoNormalBasis&onb) 
{
	basis=onb;
}

Ray ShadingState::getRay() const
{
	return r;
}

Matrix4 ShadingState::getCameraToWorld() const
{
	Camera* c=server->getScene()->getCamera();

	return c!=NULL?c->getCameraToWorld(time):Matrix4::IDENTITY;
}

Matrix4 ShadingState::getWorldToCamera() const 
{
	Camera* c=server->getScene()->getCamera();

	return c!=NULL?c->getWorldToCamera(time):Matrix4::IDENTITY;
}

BOOL ShadingState::getTrianglePoints(vector<Point3>&pp)const 
{
	PrimitiveList* prims=instance->getGeometry()->getPrimitiveList();

	if(prims->IsKindOf(TriangleMesh::Desc())) 
	{
		TriangleMesh* m=(TriangleMesh*)prims;
		pp.resize(3);
		pp[0]=m->getPoint(primitiveID,0);
		pp[1]=m->getPoint(primitiveID,1);
		pp[2]=m->getPoint(primitiveID,2);

		return TRUE;
	}

	return FALSE;
}

void ShadingState::initLightSamples() 
{
	server->initLightSamples(*this);
}

void ShadingState::initCausticSamples() 
{
	server->initCausticSamples(*this);
}

Color ShadingState::traceGlossy(Ray&rr,int ii)
{
	return server->traceGlossy(*this,rr,ii);
}

Color ShadingState::traceReflection(Ray&rr,int ii) 
{
	return server->traceReflection(*this,rr,ii);
}

Color ShadingState::traceRefraction(Ray&rr,int ii) 
{	
	rr.ox-=2.0*bias*ng.x;
	rr.oy-=2.0*bias*ng.y;
	rr.oz-=2.0*bias*ng.z;

	return server->traceRefraction(*this,rr,ii);
}

Color ShadingState::traceTransparency() 
{
	return traceRefraction(Ray(p.x,p.y,p.z,r.dx,r.dy,r.dz),0);
}

Color ShadingState::traceShadow(Ray&rr)
{
	return server->getScene()->traceShadow(rr,*iState);	
}

void ShadingState::storePhoton(const Vector3&dir,const Color&power,const Color&diff) 
{
	ps->store(*this,dir,power,diff);
}

void ShadingState::traceReflectionPhoton(Ray&rr,Color&power) 
{
	if(ps->allowReflectionBounced())
		server->traceReflectionPhoton(*this,rr,power);
}

void ShadingState::traceRefractionPhoton(Ray&rr,Color&power) 
{
	if(ps->allowRefractionBounced()) 
	{		
		rr.ox-=0.002f*ng.x;
		rr.oy-=0.002f*ng.y;
		rr.oz-=0.002f*ng.z;
		server->traceRefractionPhoton(*this,rr,power);
	}
}

void ShadingState::traceDiffusePhoton(Ray&rr,Color&power) 
{
	if(ps->allowDiffuseBounced())
		server->traceDiffusePhoton(*this,rr,power);
}

Color ShadingState::getGlobalRadiance()
{
	return server->getGlobalRadiance(*this);
}

Color ShadingState::getIrradiance(const Color&diffuseReflectance)
{
	return server->getIrradiance(*this,diffuseReflectance);
}

ShadingState* ShadingState::traceFinalGather(Ray&rr,int ii)
{
	return server->traceFinalGather(*this,rr,ii);
}

Color ShadingState::occlusion(int samples,float maxDist)
{
	return occlusion(samples,maxDist,Color::WHITE,Color::BLACK);
}

Color ShadingState::occlusion(int samples,float maxDist,const Color&bright,const Color&dark)
{
	if(n==Vector3()) return bright;
	faceforward();
	OrthoNormalBasis onb=getBasis();
	Vector3 w;
	Color result=Color::BLACK;
	for(int i=0; i<samples; i++) 
	{
		float xi=(float)getRandom(i,0,samples);
		float xj=(float)getRandom(i,1,samples);
		float phi=2.0f*LG_PI*xi;
		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);
		float sinTheta=(float)sqrt(xj);
		float cosTheta=(float)sqrt(1.0f-xj);
		w.x=cosPhi*sinTheta;
		w.y=sinPhi*sinTheta;
		w.z=cosTheta;
		onb.transform(w);
		Ray rr(p,w);
		r.setMax(maxDist);		
		result.add(Color::blend(bright,dark,traceShadow(rr)));
	}

	return result.mul(1.0f/samples);
}

Color ShadingState::diffuse(const Color&diff)
{	
	Color lr=Color::BLACK;
	if(diff.isBlack()) return lr;
	if(lightSample!=NULL)
	{
		LightSample*sample=lightSample;
		lr.madd(sample->dot(n),sample->getDiffuseRadiance());
		while(sample->next!=NULL) 
		{
			sample=sample->next;
			lr.madd(sample->dot(n),sample->getDiffuseRadiance());			 
		} 
	}
	lr.add(getIrradiance(diff));

	return lr.mul(diff).mul(1.0f/(float)LG_PI);
}

Color ShadingState::specularPhong(const Color&spec,float power,int numRays) 
{
	Color lr=Color::BLACK;
	if( !incluSpecular || spec.isBlack() ) return lr;	
	float dn=2.0f*cosND;
	Vector3 refDir;
	refDir.x=(dn*n.x)+r.dx;
	refDir.y=(dn*n.y)+r.dy;
	refDir.z=(dn*n.z)+r.dz;
	LightSample*sample=lightSample;	
	while(sample!=NULL)
	{		
		float cosNL=sample->dot(n);
		float cosLR=sample->dot(refDir);
		if(cosLR>0.0f)
			lr.madd(cosNL*(float)pow(cosLR,power),sample->getSpecularRadiance());
		sample=sample->next;
	} 	
	
	if(numRays>0) 
	{
		int numSamples=getDepth()==0?numRays:1;
		OrthoNormalBasis onb=OrthoNormalBasis::makeFromW(refDir);
		float mul=(2.0f*(float)LG_PI/(power+1))/numSamples;
		for(int i=0; i<numSamples; i++) 
		{			
			double r1=getRandom(i,0,numSamples);
			double r2=getRandom(i,1,numSamples);
			double u=2.0*LG_PI*r1;
			double s=(float)pow((float)r2,1.0f/(power+1));
			double s1=(float)sqrt(1.0-s*s);
			Vector3 w((float)(cos(u)*s1),(float)(sin(u)*s1),(float)s);
			w=onb.transform(w,Vector3());
			float wn=Vector3::dot(w,n);
			if(wn>0.0f)
				lr.madd(wn*mul,traceGlossy(Ray(p,w),i));
		}
	}
	lr.mul(spec).mul((power+2.0f)/(2.0f*(float)LG_PI));

	return lr;
}

BOOL ShadingState::operator !=(const ShadingState&ss)const
{
	const float eps=1.0e-6f;
    if(server!=ss.server) return TRUE;
	if(lightSample!=ss.lightSample) return TRUE;
    if(ps!=ss.ps) return TRUE;
    if(shader!=ss.shader) return TRUE;
	if(modifier!=ss.modifier) return TRUE;
	if(instance!=ss.instance) return TRUE;
	if(iState!=ss.iState) return TRUE;
	if(fabs(rx-ss.rx)>eps) return TRUE;
	if(fabs(ry-ss.ry)>eps) return TRUE;
    if(fabs(time-ss.time)>eps) return TRUE;
	if(result!=ss.result) return TRUE;
	if(p!=ss.p) return TRUE;
	if(n!=ss.n) return TRUE;
	if(tex!=ss.tex) return TRUE;
	if(ng!=ss.ng) return TRUE;
	if(basis!=ss.basis) return TRUE;
    if(fabs(cosND-ss.cosND)>eps) return TRUE;
	if(fabs(bias-ss.bias)>eps) return TRUE;
	if(behind!=ss.behind) return TRUE;
	if(fabs(hitU-ss.hitU)>eps) return TRUE;
	if(fabs(hitV-ss.hitV)>eps) return TRUE;
	if(fabs(hitW-ss.hitW)>eps) return TRUE;
	if(primitiveID!=ss.primitiveID) return TRUE;
	if(o2w!=ss.o2w) return TRUE;
	if(w2o!=ss.w2o) return TRUE;
	if(r!=ss.r) return TRUE;
	if(d!=ss.d) return TRUE;
	if(i!=ss.i) return TRUE;
	if(qmcD0I!=ss.qmcD0I) return TRUE;
	if(qmcD1I!=ss.qmcD1I) return TRUE;
	if(diffuseDepth!=ss.diffuseDepth) return TRUE;
	if(reflectionDepth!=ss.reflectionDepth) return TRUE;
	if(refractionDepth!=ss.refractionDepth) return TRUE;
	if(incluLights!=ss.incluLights) return TRUE;
	if(incluSpecular!=ss.incluSpecular) return TRUE;

	return FALSE;
}

BOOL ShadingState::operator==(const ShadingState&ss)const
{
	const float eps=1.0e-6f;
	if(server!=ss.server) return FALSE;
	if(lightSample!=ss.lightSample) return FALSE;
	if(ps!=ss.ps) return FALSE;
	if(shader!=ss.shader) return FALSE;
	if(modifier!=ss.modifier) return FALSE;
	if(instance!=ss.instance) return FALSE;
	if(iState!=ss.iState) return FALSE;
	if(fabs(rx-ss.rx)>eps) return FALSE;
	if(fabs(ry-ss.ry)>eps) return FALSE;
	if(fabs(time-ss.time)>eps) return FALSE;
	if(result!=ss.result) return FALSE;
	if(p!=ss.p) return FALSE;
	if(n!=ss.n) return FALSE;
	if(tex!=ss.tex) return FALSE;
	if(ng!=ss.ng) return FALSE;
	if(basis!=ss.basis) return FALSE;
	if(fabs(cosND-ss.cosND)>eps) return FALSE;
	if(fabs(bias-ss.bias)>eps) return FALSE;
	if(behind!=ss.behind) return FALSE;
	if(fabs(hitU-ss.hitU)>eps) return FALSE;
	if(fabs(hitV-ss.hitV)>eps) return FALSE;
	if(fabs(hitW-ss.hitW)>eps) return FALSE;
	if(primitiveID!=ss.primitiveID) return FALSE;
	if(o2w!=ss.o2w) return FALSE;
	if(w2o!=ss.w2o) return FALSE;
	if(r!=ss.r) return FALSE;
	if(d!=ss.d) return FALSE;
	if(i!=ss.i) return FALSE;
	if(qmcD0I!=ss.qmcD0I) return FALSE;
	if(qmcD1I!=ss.qmcD1I) return FALSE;
	if(diffuseDepth!=ss.diffuseDepth) return FALSE;
	if(reflectionDepth!=ss.reflectionDepth) return FALSE;
	if(refractionDepth!=ss.refractionDepth) return FALSE;
	if(incluLights!=ss.incluLights) return FALSE;
	if(incluSpecular!=ss.incluSpecular) return FALSE;

	return TRUE;
}

Point2& ShadingState::ss_tex()
{
	return tex;
}

LightSample* ShadingState::ss_lightSampleHead()
{
	return lightSample;
}

Point3& ShadingState::ss_point3()
{
	return p;
}

Vector3& ShadingState::ss_normal()
{
	return n;
}

Vector3& ShadingState::ss_geoNormal()
{
	return ng;
}

OrthoNormalBasis& ShadingState::ss_basis()
{
	return basis;
}

IntersectionState& ShadingState::ss_is()
{
    return *iState;
}

Ray& ShadingState::ss_ray()
{
	return r;
}