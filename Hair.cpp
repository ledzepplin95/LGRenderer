#include "StdAfx.h"

#include "Hair.h"
#include "strutil.h"
#include "ShadingState.h"
#include "BoundingBox.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(Hair,PrimitiveList)

Hair::Hair() 
{
	numSegments=1;	
	widths=ParameterList::FloatParameter(1.0f);
}

int Hair::getNumPrimitives()const
{
	return numSegments*(points.size()/(3*(numSegments+1)));
}

float Hair::getPrimitiveBound(int primID,int i)const
{
	int hair=primID/numSegments;
	int line=primID%numSegments;
	int vn=hair*(numSegments+1)+line;
	int vRoot=hair*3*(numSegments+1);
	int v0=vRoot+line*3;
	int v1=v0+3;
	int axis=i>>1;
	if( (i&1)==0 ) 
	{
		return min(points[v0+axis]-0.5f*getWidth(vn),
			points[v1+axis]-0.5f*getWidth(vn+1));
	}
	else 
	{
		return max(points[v0+axis]+0.5f*getWidth(vn), 
			points[v1+axis]+0.5f*getWidth(vn+1));
	}
}

BoundingBox Hair::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(0.0f);
	for(int i=0, j=0; i<points.size(); i+=3, j++)
	{
		float w=0.5f*getWidth(j);
		bounds.include(points[i]-w,points[i+1]-w,points[i+2]-w);
		bounds.include(points[i]+w,points[i+1]+w,points[i+2]+w);
	}	
	bounds=o2w.transform(bounds);

	return bounds;
}

float Hair::getWidth(int i) const
{
	switch(widths.interp)
	{
	case ParameterList::IT_NONE:
		return widths.data[0];
	case ParameterList::IT_VERTEX:
		return widths.data[i];
	default:
		return 0.0f;
	}
}

Vector3 Hair::getTangent(int line,int v0,float v)const 
{
	Vector3 vcurr(points[v0+3]-points[v0+0], 
		points[v0+4]-points[v0+1],points[v0+5]-points[v0+2]);
	vcurr.normalize();
	if( line==0 || line==numSegments-1 )
		return vcurr;
	if(v<=0.5f) 
	{	
		Vector3 vprev(points[v0+0]-points[v0-3],
			points[v0+1]-points[v0-2],points[v0+2]-points[v0-1]);
		vprev.normalize();
		float t=v+0.5f;
		float s=1.0f-t;
		float vx=vprev.x*s+vcurr.x*t;
		float vy=vprev.y*s+vcurr.y*t;
		float vz=vprev.z*s+vcurr.z*t;

		return Vector3(vx,vy,vz);
	} 
	else 
	{		
		v0+=3;
		Vector3 vnext(points[v0+3]-points[v0+0], 
			points[v0+4]-points[v0+1],points[v0+5]-points[v0+2]);
		vnext.normalize();
		float t=1.5f-v;
		float s=1.0-t;
		float vx=vnext.x*s+vcurr.x*t;
		float vy=vnext.y*s+vcurr.y*t;
		float vz=vnext.z*s+vcurr.z*t;

		return Vector3(vx,vy,vz);
	}
}

void Hair::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	int hair=primID/numSegments;
	int line=primID%numSegments;
	int vRoot=hair*3*(numSegments+1);
	int v0=vRoot+line*3;
	int v1=v0+3;
	float vx=points[v1+0]-points[v0+0];
	float vy=points[v1+1]-points[v0+1];
	float vz=points[v1+2]-points[v0+2];
	float ux=r.dy*vz-r.dz*vy;
	float uy=r.dz*vx-r.dx*vz;
	float uz=r.dx*vy-r.dy*vx;
	float nx=uy*vz-uz*vy;
	float ny=uz*vx-ux*vz;
	float nz=ux*vy-uy*vx;
	float tden=1.0f/(nx*r.dx+ny*r.dy+nz*r.dz);
	float tnum=nx*(points[v0+0]-r.ox) 
		+ny*(points[v0+1]-r.oy)+nz*(points[v0+2]-r.oz);
	float t=tnum*tden;
	if( r.isInside(t) )
	{
		int vn=hair*(numSegments+1)+line;
		float px=r.ox+t*r.dx;
		float py=r.oy+t*r.dy;
		float pz=r.oz+t*r.dz;
		float qx=px-points[v0+0];
		float qy=py-points[v0+1];
		float qz=pz-points[v0+2];
		float q=(vx*qx+vy*qy+vz*qz)/(vx*vx+vy*vy+vz*vz);
		if( q<=0.0f ) 
		{			
			if(line==0)
				return;
			float dx=points[v0+0]-px;
			float dy=points[v0+1]-py;
			float dz=points[v0+2]-pz;
			float d2=dx*dx+dy*dy+dz*dz;
			float width=getWidth(vn);
			if( d2<(width*width*0.25f) )
			{
				r.setMax(t);
				state.setIntersection(primID,0,0);
			}
		}
		else if( q>=1.0f )
		{
			float dx=points[v1+0]-px;
			float dy=points[v1+1]-py;
			float dz=points[v1+2]-pz;
			float d2=dx*dx+dy*dy+dz*dz;
			float width=getWidth(vn+1);
			if( d2<(width*width*0.25f) ) 
			{
				r.setMax(t);
				state.setIntersection(primID,0.0,1.0);
			}
		} 
		else 
		{
			float dx=points[v0+0]+q*vx-px;
			float dy=points[v0+1]+q*vy-py;
			float dz=points[v0+2]+q*vz-pz;
			float d2=dx*dx+dy*dy+dz*dz;
			float width=(1.0-q)*getWidth(vn)+q*getWidth(vn+1);
			if( d2<(width*width*0.25f) ) 
			{
				r.setMax(t);
				state.setIntersection(primID,0.0,q);
			}
		}
	}
}

void Hair::prepareShadingState(ShadingState&state)
{
	state.init();
	Instance* i=state.getInstance();
	state.getRay().getPoint(state.ss_point3());
	Ray r=state.getRay();
	Shader* s=i->getShader(0);
	state.setShader(s);
	int primID=state.getPrimitiveID();
	int hair=primID/numSegments;
	int line=primID%numSegments;
	int vRoot=hair*3*(numSegments+1);
	int v0=vRoot+line*3;

	Vector3 v=getTangent(line,v0,state.getV());
	v=state.transformVectorObjectToWorld(v);
	state.setBasis(OrthoNormalBasis::makeFromWV(v,Vector3(-r.dx,-r.dy,-r.dz)));
	state.ss_basis().swapVW();	
	state.ss_normal().set(0,0,1);
	state.ss_basis().transform(state.getNormal());
	state.ss_geoNormal().set(state.getNormal());
	state.ss_tex().set(0,(line+state.getV())/numSegments);
}

BOOL Hair::update(ParameterList&pl,LGAPI&api) 
{
	numSegments=pl.getInt("segments",numSegments);
	if(numSegments<1) 
	{
		StrUtil::PrintPrompt("无效的段数: %d",numSegments);
		return FALSE;
	}
	ParameterList::FloatParameter pointsP=pl.getPointArray("points");	
	if(pointsP.interp!=ParameterList::IT_VERTEX)
		StrUtil::PrintPrompt("点插值类型必须设定为 \"vertex\" - was \"%s\"",
		ParameterList::GetInterpolationTypeName(pointsP.interp).MakeUpper());
	else 
	{
		points=pointsP.data;
	}
	
	if(points.empty()) 
	{
		StrUtil::PrintPrompt("不能更新hair - 顶点缺失");
		return FALSE;
	}

	pl.setVertexCount(points.size()/3);
	ParameterList::FloatParameter widthsP=pl.getFloatArray("widths");	
	if( widthsP.interp==ParameterList::IT_NONE 
		|| widthsP.interp==ParameterList::IT_VERTEX )
		widths=widthsP;
	else
		StrUtil::PrintPrompt("不支持宽度插值类型 %s -- 忽略", 
		ParameterList::GetInterpolationTypeName(widthsP.interp).MakeUpper());

	return TRUE;
}

Color Hair::getRadiance(ShadingState&state) const
{
	state.initLightSamples();
	state.initCausticSamples();
	Vector3 v=state.getRay().getDirection();
	v.negate();
	Vector3 h;
	Vector3 t=state.getBasis().transform(Vector3(0,1,0));
	Color diff=Color::BLACK;
	Color spec=Color::BLACK;
	for(LightSample*sample=state.ss_lightSampleHead(); sample!=NULL; sample=sample->next) 
	{		
		Vector3 l=sample->getShadowRay().getDirection();
		float dotTL=Vector3::dot(t,l);
		float sinTL=(float)sqrt(1.0-dotTL*dotTL);	
		diff.madd(sinTL,sample->getDiffuseRadiance());
		Vector3::add(v,l,h);
		h.normalize();
		float dotTH=Vector3::dot(t,h);
		float sinTH=(float)sqrt(1.0-dotTH*dotTH);
		float s=(float)pow(sinTH,10.0f);
		spec.madd(s,sample->getSpecularRadiance());
	}
	Color c=Color::add(diff,spec,Color());	

	return Color::blend(c,state.traceTransparency(),state.getV(),Color());
}

void Hair::scatterPhoton(ShadingState&state,Color&power) 
{
}

PrimitiveList* Hair::getBakingPrimitives() const
{
	return NULL;
}