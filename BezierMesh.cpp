#include "StdAfx.h"

#include "BezierMesh.h"
#include "strutil.h"
#include "QuadMesh.h"
#include "TriangleMesh.h"
#include "BoundingBox.h"
#include "lgapi.h"
#include "Vector3.h"

LG_IMPLEMENT_DYNCREATE(BezierMesh,Tesselatable)

BezierMesh::BezierMesh()
{	
}

BezierMesh::BezierMesh(const vector<vector<float>>&p) 
{
	subdivs=8;
	smooth=TRUE;
	quads=FALSE;	
	patches=p;
}

BezierMesh::BezierMesh(float p[][49])
{
	subdivs=8;
	smooth=TRUE;
	quads=FALSE;
    for(int i=0; i<49; i++)
	{
		vector<float> ps;
		for(int j=0; j<48; j++)
		    ps.push_back(p[j][i]);
		patches.push_back(ps);
	}
}

BezierMesh::BezierMesh(float p[][48])
{
	subdivs=8;
	smooth=TRUE;
	quads=FALSE;
	for(int i=0; i<48; i++)
	{
		vector<float> ps;
		for(int j=0; j<48; j++)
			ps.push_back(p[j][i]);
		patches.push_back(ps);
	}
}

BoundingBox BezierMesh::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(0.0f);	
	for(int i=0; i<patches.size(); i++) 
	{
		vector<float> patch=patches[i];
		for(int j=0; j<patch.size(); j+=3) 
		{
			float x=patch[j];
			float y=patch[j+1];
			float z=patch[j+2];
			float wx=o2w.transformPX(x,y,z);
			float wy=o2w.transformPY(x,y,z);
			float wz=o2w.transformPZ(x,y,z);
			bounds.include(wx,wy,wz);
		}
	}

	return bounds;
}

vector<float> BezierMesh::bernstein(float u)const 
{
	vector<float> b;
	b.resize(4);
	float i=1.0f-u;
	b[0]=i*i*i;
	b[1]=3.0f*u*i*i;
	b[2]=3.0f*u*u*i;
	b[3]=u*u*u;

	return b;
}

vector<float> BezierMesh::bernsteinDeriv(float u)const
{
	if(!smooth)
		return vector<float>();
	vector<float> b;
	b.resize(4);
	float i=1.0f-u;
	b[0]=3.0f*(0-i*i);
	b[1]=3.0f*(i*i-2*u*i);
	b[2]=3.0f*(2*u*i-u*u);
	b[3]=3.0f*(u*u-0.0f);

	return b;
}

void BezierMesh::getPatchPoint(float u,float v,const vector<float>&ctrl,const vector<float>&bu, 
						   const vector<float>&bv,const vector<float>&bdu,const vector<float>&bdv,
						   Point3&p,Vector3&n)const
{
	float px=0.0f;
	float py=0.0f;
	float pz=0.0f;
	for(int i=0,index=0; i<4; i++) 
	{
		for(int j=0; j<4; j++, index+=3)
		{
			float scale=bu[j]*bv[i];
			px+=ctrl[index+0]*scale;
			py+=ctrl[index+1]*scale;
			pz+=ctrl[index+2]*scale;
		}
	}
	p.x=px;
	p.y=py;
	p.z=pz;

	float dpdux=0.0f;
	float dpduy=0.0f;
	float dpduz=0.0f;
	float dpdvx=0.0f;
	float dpdvy=0.0f;
	float dpdvz=0.0f;
	for(int i=0, index=0; i<4; i++) 
	{
		for(int j=0; j<4; j++, index+=3)
		{
			float scaleu=bdu[j]*bv[i];
			dpdux+=ctrl[index+0]*scaleu;
			dpduy+=ctrl[index+1]*scaleu;
			dpduz+=ctrl[index+2]*scaleu;
			float scalev=bu[j]*bdv[i];
			dpdvx+=ctrl[index+0]*scalev;
			dpdvy+=ctrl[index+1]*scalev;
			dpdvz+=ctrl[index+2]*scalev;
		}
	}		
	n.x=(dpduy*dpdvz-dpduz*dpdvy);
	n.y=(dpduz*dpdvx-dpdux*dpdvz);
	n.z=(dpdux*dpdvy-dpduy*dpdvx);
}

PrimitiveList* BezierMesh::tesselate() const
{
	vector<float> vertices;
	vertices.resize(patches.size()*(subdivs+1)*(subdivs+1)*3);
	vector<float> normals;
	normals.resize(smooth?(patches.size()*(subdivs+1)*(subdivs+1)*3):0);
	vector<float> uvs;
	uvs.resize(patches.size()*(subdivs+1)*(subdivs+1)*2);
	vector<int> indices;
	indices.resize(patches.size()*subdivs*subdivs*(quads?4:(2*3)));

	int vidx=0,pidx=0;
	float step=1.0f/subdivs;
	int vstride=subdivs+1;
	Point3 p;
	Vector3 n;
	for(int m=0; m<patches.size(); m++) 
	{	
		vector<float> patch=patches[m];
		for(int i=0, voff=0; i<=subdivs; i++) 
		{
			float u=i*step;
			vector<float> bu=bernstein(u);
			vector<float> bdu=bernsteinDeriv(u);
			for(int j=0; j<=subdivs; j++, voff+=3) 
			{
				float v=j*step;
				vector<float> bv=bernstein(v);
				vector<float> bdv=bernsteinDeriv(v);
				getPatchPoint(u,v,patch,bu,bv,bdu,bdv,p,n);
				vertices[vidx+voff+0]=p.x;
				vertices[vidx+voff+1]=p.y;
				vertices[vidx+voff+2]=p.z;
				if(smooth) 
				{
					normals[vidx+voff+0]=n.x;
					normals[vidx+voff+1]=n.y;
					normals[vidx+voff+2]=n.z;
				}
				uvs[(vidx+voff)/3*2+0]=u;
				uvs[(vidx+voff)/3*2+1]=v;
			}
		}	
		for(int i=0, vbase=vidx/3; i<subdivs; i++) 
		{
			for(int j=0; j<subdivs; j++) 
			{
				int v00=(i+0)*vstride+(j+0);
				int v10=(i+1)*vstride+(j+0);
				int v01=(i+0)*vstride+(j+1);
				int v11=(i+1)*vstride+(j+1);
				if(quads) 
				{
					indices[pidx+0]=vbase+v01;
					indices[pidx+1]=vbase+v00;
					indices[pidx+2]=vbase+v10;
					indices[pidx+3]=vbase+v11;
					pidx+=4;
				}
				else 
				{					
					indices[pidx+0]=vbase+v00;
					indices[pidx+1]=vbase+v10;
					indices[pidx+2]=vbase+v01;
					indices[pidx+3]=vbase+v10;
					indices[pidx+4]=vbase+v11;
					indices[pidx+5]=vbase+v01;
					pidx+=6;
				}
			}
		}
		vidx+=vstride*vstride*3;
	}
	ParameterList pl;
	pl.addPoints("points",ParameterList::IT_VERTEX,vertices);
	if(quads)
		pl.addIntegerArray("quads",indices);
	else
		pl.addIntegerArray("triangles",indices);
	pl.addTexCoords("uvs",ParameterList::IT_VERTEX,uvs);
	if(smooth)
		pl.addVectors("normals",ParameterList::IT_VERTEX,normals);
	PrimitiveList* m=NULL;
	if(quads)
	{		
		m=new QuadMesh;
	}	
	else
	{		
		m=new TriangleMesh;
	}	
	LGMemoryAllocator::lg_mem_pointer(m);
	m->update(pl,LGAPI());
	pl.clear(TRUE);

	return m;
}

BOOL BezierMesh::update(ParameterList&pl,LGAPI&api)
{
	subdivs=pl.getInt("subdivs",subdivs);
	smooth=pl.getBoolean("smooth",smooth);
	quads=pl.getBoolean("quads",quads);
	int nu=pl.getInt("nu",0);
	int nv=pl.getInt("nv",0);
	pl.setVertexCount(nu*nv);
	BOOL uwrap=pl.getBoolean("uwrap",FALSE);
    BOOL vwrap=pl.getBoolean("vwrap",FALSE);
	ParameterList::FloatParameter points=pl.getPointArray("points");
	if(points.interp==ParameterList::IT_VERTEX)
	{
		int numUPatches=uwrap?nu/3:(nu-4)/3+1;
		int numVPatches=vwrap?nv/3:(nv-4)/3+1;
		if( numUPatches<1 || numVPatches<1 )
		{
			StrUtil::PrintPrompt("bezier mesh的面片数无效 - 忽略");
			return FALSE;
		}		
		patches.resize(numUPatches*numVPatches);
		for(int v=0, p=0; v<numVPatches; v++) 
		{
			for(int u=0; u<numUPatches; u++, p++)
			{
				patches[p].resize(16*3);
				vector<float>& patch=patches[p];
				int up=u*3;
				int vp=v*3;
				for(int pv=0; pv<4; pv++) 
				{
					for(int pu=0; pu<4; pu++) 
					{
						int meshU=(up+pu)%nu;
						int meshV=(vp+pv)%nv;						
						patch[3*(pv*4+pu)+0]=points.data[3*(meshU+nu*meshV)+0];
						patch[3*(pv*4+pu)+1]=points.data[3*(meshU+nu*meshV)+1];
						patch[3*(pv*4+pu)+2]=points.data[3*(meshU+nu*meshV)+2];
					}
				}
			}
		}
	}
	if(subdivs<1) 
	{
		StrUtil::PrintPrompt( "bezier mesh的细分无效 - 忽略");
		return FALSE;
	}
	if(patches.empty()) 
	{
		StrUtil::PrintPrompt("bezier mesh中没有面片数据 - 忽略");
		return FALSE;
	}

	return TRUE;
}