#include "StdAfx.h"

#include "QuadMesh.h"
#include "ShadingState.h"
#include "MathUtil.h"
#include "strutil.h"
#include "BoundingBox.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(QuadMesh,PrimitiveList)

QuadMesh::QuadMesh()
{	
	normals=uvs=ParameterList::FloatParameter();	
}

void QuadMesh::writeObj(CString fileName) 
{
	try
	{
		CFile qmFile;		
		if( !qmFile.Open(fileName,CFile::modeWrite | CFile::typeText) )
			return;	
		CString str;
        str.Format(_T("o object\n"));
		qmFile.Write(str,str.GetLength());
		for(int i=0; i<points.size(); i+=3)
		{
			str.Format(_T("v %g %g %g\n"),points[i],points[i+1],points[i+2]);
			qmFile.Write(str,str.GetLength());
		}
		str.Format("s off\n");
		qmFile.Write(str,str.GetLength());
		for(int i=0; i<quads.size(); i+=4)
		{
			str.Format("f %d %d %d %d\n",quads[i]+1,quads[i+1]+1, 
				quads[i+2]+1,quads[i+3]+1);
			qmFile.Write(str,str.GetLength());
		}
		qmFile.Close();
	} 
	catch(...)
	{		
	}
}

BOOL QuadMesh::update(ParameterList&pl,LGAPI&api) 
{	
	vector<int> _quads=pl.getIntArray("quads");
	if(!_quads.empty())		
		quads=_quads;	
	if(quads.empty())
	{
		StrUtil::PrintPrompt("不能更新mesh - quad索引缺失");
		return FALSE;
	}
	if(quads.size()%4!=0)
		StrUtil::PrintPrompt("Quad索引数据不是4的乘数 - 一些quad可能缺失");
	pl.setFaceCount(quads.size()/4);
	{
		ParameterList::FloatParameter pointsP=pl.getPointArray("points");
		if(pointsP.interp!=ParameterList::IT_VERTEX)
			StrUtil::PrintPrompt("点插值类型必须设定为 \"vertex\" - was \"%s\"", 
			ParameterList::GetInterpolationTypeName(pointsP.interp).MakeLower());
		else 
		{
			points=pointsP.data;
		}
	}
	if(points.empty()) 
	{
		StrUtil::PrintPrompt("不能更新mesh - 顶点缺失");
		return FALSE;
	}
	pl.setVertexCount(points.size()/3);
	pl.setFaceVertexCount(4*(quads.size()/4));
	ParameterList::FloatParameter _normals=pl.getVectorArray("normals");
    normals=_normals;
	ParameterList::FloatParameter _uvs=pl.getTexCoordArray("uvs");
	uvs=_uvs;
	vector<int> _faceShaders=pl.getIntArray("faceshaders");
	if( !_faceShaders.empty() && _faceShaders.size()==quads.size()/4 ) 
	{
		faceShaders.resize(faceShaders.size());
		for(int i=0; i<faceShaders.size(); i++) 
		{
			int v=faceShaders[i];
			if(v>255)
				StrUtil::PrintPrompt("quad上的Shader索引值%d太大",i);
			faceShaders[i]=(byte)(v&0xFF);
		}
	}

	return TRUE;
}

float QuadMesh::getPrimitiveBound(int primID,int i) const
{
	int quad=4*primID;
	int a=3*quads[quad+0];
	int b=3*quads[quad+1];
	int c=3*quads[quad+2];
	int d=3*quads[quad+3];
	int axis=i>>1;
	if( (i&1)==0 )
		return MathUtil::mu_min(points[a+axis],points[b+axis],points[c+axis],points[d+axis]);
	else
		return MathUtil::mu_max(points[a+axis],points[b+axis],points[c+axis],points[d+axis]);
}

BoundingBox QuadMesh::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(0.0f);		
	for(int i=0; i<points.size(); i+=3) 
	{
		float x=points[i];
		float y=points[i+1];
		float z=points[i+2];
		float wx=o2w.transformPX(x,y,z);
		float wy=o2w.transformPY(x,y,z);
		float wz=o2w.transformPZ(x,y,z);
		bounds.include(wx,wy,wz);
	}

	return bounds;
}

void QuadMesh::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	int quad=4*primID;
	int p0=3*quads[quad+0];
	int p1=3*quads[quad+1];
	int p2=3*quads[quad+2];
	int p3=3*quads[quad+3];

	vector<float> A;
	A.push_back(points[p2+0]-points[p3+0]-points[p1+0]+points[p0+0]);
	A.push_back(points[p2+1]-points[p3+1]-points[p1+1]+points[p0+1]);
	A.push_back(points[p2+2]-points[p3+2]-points[p1+2]+points[p0+2]);
	vector<float> B;
	B.push_back(points[p1+0]-points[p0+0]);
	B.push_back(points[p1+1]-points[p0+1]);
	B.push_back(points[p1+2]-points[p0+2]);
    vector<float> C;
	C.push_back(points[p3+0]-points[p0+0]);
	C.push_back(points[p3+1]-points[p0+1]);
	C.push_back(points[p3+2]-points[p0+2]);
    vector<float> R;
	R.push_back(r.ox-points[p0+0]);
	R.push_back(r.oy-points[p0+1]);
	R.push_back(r.oz-points[p0+2]);
	vector<float> Q;
	Q.push_back(r.dx);
	Q.push_back(r.dy);
	Q.push_back(r.dz);

	float absqx=fabs(r.dx);
	float absqy=fabs(r.dy);
	float absqz=fabs(r.dz);

	int X=0,Y=1,Z=2;
	if( absqx>absqy && absqx>absqz ) 
	{			
	} 
	else if(absqy>absqz)
	{		
		X=1;
		Y=0;
	} 
	else 
	{			
		X=2;
		Z=0;
	}

	float Cxz=C[X]*Q[Z]-C[Z]*Q[X];
	float Cyx=C[Y]*Q[X]-C[X]*Q[Y];
	float Czy=C[Z]*Q[Y]-C[Y]*Q[Z];
	float Rxz=R[X]*Q[Z]-R[Z]*Q[X];
	float Ryx=R[Y]*Q[X]-R[X]*Q[Y];
	float Rzy=R[Z]*Q[Y]-R[Y]*Q[Z];
	float Bxy=B[X]*Q[Y]-B[Y]*Q[X];
	float Byz=B[Y]*Q[Z]-B[Z]*Q[Y];
	float Bzx=B[Z]*Q[X]-B[X]*Q[Z];
	float a=A[X]*Byz+A[Y]*Bzx+A[Z]*Bxy;
	if(a==0.0f)
	{			
		float b=B[X]*Czy+B[Y]*Cxz+B[Z]*Cyx;
		float c=C[X]*Rzy+C[Y]*Rxz+C[Z]*Ryx;
		float u=-c/b;
		if( u>=0.0f && u<=1.0f )
		{
			float v=(u*Bxy+Ryx)/Cyx;
			if( v>=0.0f && v<=1.0f ) 
			{
				float t=(B[X]*u+C[X]*v-R[X])/Q[X];
				if (r.isInside(t)) 
				{
					r.setMax(t);
					state.setIntersection(primID,u,v);
				}
			}
		}
	} 
	else 
	{			
		float b=A[X]*Rzy+A[Y]*Rxz+A[Z]*Ryx+B[X]*Czy+B[Y]*Cxz+B[Z]*Cyx;
		float c=C[X]*Rzy+C[Y]*Rxz+C[Z]*Ryx;
		float discrim=b*b-4*a*c;		
		if( c*(a+b+c)>0.0f && (discrim<0.0f || a*c<0.0f || b/a>0.0f || b/a<-2.0f) )
			return;			
		float q=b>0.0f?-0.5f*(b+(float)sqrt(discrim)):-0.5f*(b-(float)sqrt(discrim));			
		float Axy=A[X]*Q[Y]-A[Y]*Q[X];
		float u=q/a;
		if( u>=0.0f && u<=1.0f ) 
		{
			float d=u*Axy-Cyx;
			float v=-(u*Bxy+Ryx)/d;
			if( v>=0.0f && v<=1.0f ) 
			{
				float t=(A[X]*u*v+B[X]*u+C[X]*v-R[X])/Q[X];
				if(r.isInside(t)) 
				{
					r.setMax(t);
					state.setIntersection(primID,u,v);
				}
			}
		}
		u=c/q;
		if( u>=0.0f && u<=1.0f ) 
		{
			float d=u*Axy-Cyx;
			float v=-(u*Bxy+Ryx)/d;
			if( v>=0.0f && v<=1.0f )
			{
				float t=(A[X]*u*v+B[X]*u+C[X]*v-R[X])/Q[X];
				if (r.isInside(t)) 
				{
					r.setMax(t);
					state.setIntersection(primID,u,v);
				}
			}
		}
	}
}

int QuadMesh::getNumPrimitives() const
{
	return quads.size()/4;
}

void QuadMesh::prepareShadingState(ShadingState&state)
{
	state.init();
	Instance* parent=state.getInstance();
	int primID=state.getPrimitiveID();
	float u=state.getU();
	float v=state.getV();
	state.getRay().getPoint(state.ss_point3());
	int quad=4*primID;
	int index0=quads[quad+0];
	int index1=quads[quad+1];
	int index2=quads[quad+2];
	int index3=quads[quad+3];
	Point3 v0p=getPoint(index0);
	Point3 v1p=getPoint(index1);
	Point3 v2p=getPoint(index2);
	Point3 v3p=getPoint(index2);
	float tanux=(1.0f-v)*(v1p.x-v0p.x)+v*(v2p.x-v3p.x);
	float tanuy=(1.0f-v)*(v1p.y-v0p.y)+v*(v2p.y-v3p.y);
	float tanuz=(1.0f-v)*(v1p.z-v0p.z)+v*(v2p.z-v3p.z);

	float tanvx=(1.0f-u)*(v3p.x-v0p.x)+u*(v2p.x-v1p.x);
	float tanvy=(1.0f-u)*(v3p.y-v0p.y)+u*(v2p.y-v1p.y);
	float tanvz=(1.0f-u)*(v3p.z-v0p.z)+u*(v2p.z-v1p.z);

	float nx=tanuy*tanvz-tanuz*tanvy;
	float ny=tanuz*tanvx-tanux*tanvz;
	float nz=tanux*tanvy-tanuy*tanvx;

	Vector3 ng(nx,ny,nz);
	ng=state.transformNormalObjectToWorld(ng);
	ng.normalize();
	state.ss_geoNormal().set(ng);

	float k00=(1.0f-u)*(1.0f-v);
	float k10=u*(1.0f-v);
	float k01=(1.0f-u)*v;
	float k11=u*v;

	switch(normals.interp) 
	{
	case ParameterList::IT_NONE:
	case ParameterList::IT_FACE: 
		{
		state.ss_normal().set(ng);
		break;
		}
	case ParameterList::IT_VERTEX: 
		{
		int i30=3*index0;
		int i31=3*index1;
		int i32=3*index2;
		int i33=3*index3;
		vector<float> _normals=normals.data;
		state.ss_normal().x=k00*_normals[i30+0]+k10*_normals[i31+0]+k11*_normals[i32+0]+k01*_normals[i33+0];
		state.ss_normal().y=k00*_normals[i30+1]+k10*_normals[i31+1]+k11*_normals[i32+1]+k01*_normals[i33+1];
		state.ss_normal().z=k00*_normals[i30+2]+k10*_normals[i31+2]+k11*_normals[i32+2]+k01*_normals[i33+2];
		state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
		state.ss_normal().normalize();
		break;
		}
	case ParameterList::IT_FACEVARYING:
		{
			int idx=3*quad;
			vector<float> _normals=normals.data;
			state.ss_normal().x=k00*_normals[idx+0]+k10*_normals[idx+3]+k11*_normals[idx+6]+k01*_normals[idx+9];
			state.ss_normal().y=k00*_normals[idx+1]+k10*_normals[idx+4]+k11*_normals[idx+7]+k01*_normals[idx+10];
			state.ss_normal().z=k00*_normals[idx+2]+k10*_normals[idx+5]+k11*_normals[idx+8]+k01*_normals[idx+11];
			state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
			state.ss_normal().normalize();
			break;
		}
	}
	float uv00=0.0f,uv01=0.0f,uv10=0.0f,uv11=0.0f,uv20=0.0f,uv21=0.0f,uv30=0.0f,uv31=0.0f;
	switch(uvs.interp) 
	{
	case ParameterList::IT_NONE:
	case ParameterList::IT_FACE: 
		{
		state.ss_tex().x=0.0f;
		state.ss_tex().y=0.0f;
		break;
		}
	case ParameterList::IT_VERTEX: 
		{
		int i20=2*index0;
		int i21=2*index1;
		int i22=2*index2;
		int i23=2*index3;
		vector<float> _uvs=uvs.data;
		uv00=_uvs[i20+0];
		uv01=_uvs[i20+1];
		uv10=_uvs[i21+0];
		uv11=_uvs[i21+1];
		uv20=_uvs[i22+0];
		uv21=_uvs[i22+1];
		uv20=_uvs[i23+0];
		uv21=_uvs[i23+1];
		break;
		}
	case ParameterList::IT_FACEVARYING:
		{
			int idx=quad<<1;
			vector<float> _uvs=uvs.data;
			uv00=_uvs[idx+0];
			uv01=_uvs[idx+1];
			uv10=_uvs[idx+2];
			uv11=_uvs[idx+3];
			uv20=_uvs[idx+4];
			uv21=_uvs[idx+5];
			uv30=_uvs[idx+6];
			uv31=_uvs[idx+7];
			break;
		}
	}
	if(uvs.interp!=ParameterList::IT_NONE)
	{
		state.ss_tex().x=k00*uv00+k10*uv10+k11*uv20+k01*uv30;
		state.ss_tex().y=k00*uv01+k10*uv11+k11*uv21+k01*uv31;
		float du1=uv00-uv20;
		float du2=uv10-uv20;
		float dv1=uv01-uv21;
		float dv2=uv11-uv21;
		Vector3 dp1=Point3::sub(v0p,v2p,Vector3()),dp2=Point3::sub(v1p,v2p,Vector3());
		float determinant=du1*dv2-dv1*du2;
		if(determinant==0.0f) 
		{		
			state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
		}
		else 
		{
			float invdet=1.0f/determinant;		
			Vector3 dpdv;
			dpdv.x=(-du2*dp1.x+du1*dp2.x)*invdet;
			dpdv.y=(-du2*dp1.y+du1*dp2.y)*invdet;
			dpdv.z=(-du2*dp1.z+du1*dp2.z)*invdet;
			dpdv=state.transformVectorObjectToWorld(dpdv);			
			state.setBasis(OrthoNormalBasis::makeFromWV(state.getNormal(),dpdv));
		}
	}
	else
		state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));

	int shaderIndex=faceShaders.empty()?0:(faceShaders[primID]&0xFF);
	state.setShader(parent->getShader(shaderIndex));
	state.setModifier(parent->getModifier(shaderIndex));
}

Point3 QuadMesh::getPoint(int i)const
{
	i*=3;

	return Point3(points[i],points[i+1],points[i+2]);
}

PrimitiveList* QuadMesh::getBakingPrimitives()const 
{
	return NULL;
}