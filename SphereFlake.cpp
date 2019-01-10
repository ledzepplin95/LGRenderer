#include "StdAfx.h"

#include "SphereFlake.h"
#include "LGdef.h"
#include "MathUtil.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "BoundingBox.h"
#include "instance.h"

class vinitSphereFlake
{
public:
	vinitSphereFlake(void);		
};

int SphereFlake::MAX_LEVEL=20;
vector<float> SphereFlake::boundingRadiusOffset;
vector<float> SphereFlake::recursivePattern;
static vinitSphereFlake ini;

LG_IMPLEMENT_DYNCREATE(SphereFlake,PrimitiveList)

vinitSphereFlake::vinitSphereFlake()
{
	SphereFlake::initial();
}

void SphereFlake::initial()
{
	boundingRadiusOffset.resize(MAX_LEVEL+1);
	recursivePattern.resize(9*3);

	for(int i=0, r=3; i<boundingRadiusOffset.size(); i++, r*=3)
		boundingRadiusOffset[i]=(r-3.0f)/r;
	
	double a=0.0,daL=2.0*LG_PI/6.0,daU=2.0*LG_PI/3.0;
	for(int i=0; i<6; i++)
	{
		recursivePattern[3*i+0]=-0.3f;
		recursivePattern[3*i+1]=(float)sin(a);
		recursivePattern[3*i+2]=(float)cos(a);
		a+=daL;
	}
	a-=daL/2; 
	for(int i=6; i<9; i++)
	{
		recursivePattern[3*i+0]=+0.7f;
		recursivePattern[3*i+1]=(float)sin(a);
		recursivePattern[3*i+2]=(float)cos(a);
		a+=daU;
	}

	for(int i=0; i<recursivePattern.size(); i+=3) 
	{
		float x=recursivePattern[i+0];
		float y=recursivePattern[i+1];
		float z=recursivePattern[i+2];
		float n=1.0f/(float)sqrt(x*x+y*y+z*z);
		recursivePattern[i+0]=x*n;
		recursivePattern[i+1]=y*n;
		recursivePattern[i+2]=z*n;
	}
}

SphereFlake::SphereFlake()
{
	level=2;
	axis.set(0,0,1);
	baseRadius=1.0f;
}

BOOL SphereFlake::update(ParameterList&pl,LGAPI&api) 
{
	level=MathUtil::clamp(pl.getInt("level",level),0,20);
	axis=pl.getVector("axis",axis);
	axis.normalize();
	baseRadius=fabs(pl.getFloat("radius",baseRadius));

	return TRUE;
}

BoundingBox SphereFlake::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(getPrimitiveBound(0,1));
    bounds=o2w.transform(bounds);

	return bounds;
}

float SphereFlake::getPrimitiveBound(int primID,int i)const
{
	float br=1.0f+boundingRadiusOffset[level];

	return (i&1)==0?-br:br;
}

int SphereFlake::getNumPrimitives() const
{
	return 1;
}

void SphereFlake::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Point3 localPoint=state.transformWorldToObject(state.getPoint());

	float cx=state.getU();
	float cy=state.getV();
	float cz=state.getW();

	state.ss_normal().set(localPoint.x-cx,localPoint.y-cy,localPoint.z-cz);
	state.ss_normal().normalize();

	float phi=(float)atan2(state.getNormal().y,state.getNormal().x);
	if(phi<0.0f)
		phi+=2.0f*LG_PI;
	float theta=(float)acos(state.getNormal().z);
	state.ss_tex().y=theta/(float)LG_PI;
	state.ss_tex().x=phi/(2.0f*LG_PI);
	Vector3 v;
	v.x=-2.0f*LG_PI*state.getNormal().y;
	v.y=2.0f*LG_PI*state.getNormal().x;
	v.z=0.0f;
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));
	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	v=state.transformVectorObjectToWorld(v);
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());	
	state.setBasis(OrthoNormalBasis::makeFromWV(state.getNormal(),v));
}

void SphereFlake::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{	
	float qa=r.dx*r.dx+r.dy*r.dy+r.dz*r.dz;
	intersectFlake(r,state,level,qa,1.0/qa,0,0,0,axis.x,axis.y,axis.z,baseRadius);
}

void SphereFlake::intersectFlake(Ray&r,IntersectionState&state,int level, 
					float qa,float qaInv,float cx,float cy,
					float cz,float dx,float dy,float dz,float radius) const
{
	if(level<=0) 
	{		
		float vcx=cx-r.ox;
		float vcy=cy-r.oy;
		float vcz=cz-r.oz;
		float b=r.dx*vcx+r.dy*vcy+r.dz*vcz;
		float disc=b*b-qa*((vcx*vcx+vcy*vcy+vcz*vcz)-radius*radius);
		if(disc>0.0f)
		{			
			float d=(float)sqrt(disc);
			float t1=(b-d)*qaInv;
			float t2=(b+d)*qaInv;
			if( t1>=r.getMax() || t2<=r.getMin() )
				return;
			if(t1>r.getMin())
				r.setMax(t1);
			else
				r.setMax(t2);
			state.setIntersection(0,cx,cy,cz);
		}
	} 
	else 
	{
		float boundRadius=radius*(1.0f+boundingRadiusOffset[level]);
		float vcx=cx-r.ox;
		float vcy=cy-r.oy;
		float vcz=cz-r.oz;
		float b=r.dx*vcx+r.dy*vcy+r.dz*vcz;
		float vcd=(vcx*vcx+vcy*vcy+vcz*vcz);
		float disc=b*b-qa*(vcd-boundRadius*boundRadius);
		if(disc>0.0f) 
		{			
			float d=(float)sqrt(disc);
			float t1=(b-d)*qaInv;
			float t2=(b+d)*qaInv;
			if( t1>=r.getMax() || t2<=r.getMin() )
				return;
		
			disc=b*b-qa*(vcd-radius*radius);
			if(disc>0.0f)
			{
				d=(float)sqrt(disc);
				t1=(b-d)*qaInv;
				t2=(b+d)*qaInv;
				if( t1>= r.getMax() || t2<=r.getMin() ) 
				{				
				} 
				else 
				{
					if(t1>r.getMin())
						r.setMax(t1);
					else
						r.setMax(t2);
					state.setIntersection(0,cx,cy,cz);
				}
			}
		
			float b1x,b1y,b1z;
			if( dx*dx<dy*dy && dx*dx<dz*dz ) 
			{
				b1x=0.0f;
				b1y=dz;
				b1z=-dy;
			}
			else if(dy*dy<dz*dz) 
			{
				b1x=dz;
				b1y=0.0f;
				b1z=-dx;
			} 
			else 
			{
				b1x=dy;
				b1y=-dx;
				b1z=0.0f;
			}
			float n=1.0f/(float)sqrt(b1x*b1x+b1y*b1y+b1z*b1z);
			b1x*=n;
			b1y*=n;
			b1z*=n;
			float b2x=dy*b1z-dz*b1y;
			float b2y=dz*b1x-dx*b1z;
			float b2z=dx*b1y-dy*b1x;
			b1x=dy*b2z-dz*b2y;
			b1y=dz*b2x-dx*b2z;
			b1z=dx*b2y-dy*b2x;			
			float nr=radius*(1.0f/3.0f),scale=radius+nr;

			for(int i=0; i<9*3; i+=3) 
			{				
				float ndx=recursivePattern[i]*dx+recursivePattern[i+1]*b1x 
					+recursivePattern[i+2]*b2x;
				float ndy=recursivePattern[i]*dy+recursivePattern[i+1]*b1y
					+recursivePattern[i+2]*b2y;
				float ndz=recursivePattern[i]*dz+recursivePattern[i+1]*b1z 
					+recursivePattern[i+2]*b2z;
			
				intersectFlake(r,state,level-1,qa,qaInv,cx+scale*ndx,cy 
					+scale*ndy,cz+scale*ndz,ndx,ndy,ndz,nr);
			}
		}
	}
}

PrimitiveList* SphereFlake::getBakingPrimitives()const 
{
	return NULL;
}