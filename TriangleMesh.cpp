#include "StdAfx.h"

#include "TriangleMesh.h"
#include "IntersectionState.h"
#include "ShadingState.h"
#include "strutil.h"
#include "mathutil.h"
#include "BoundingBox.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(BakingSurface,PrimitiveList)
LG_IMPLEMENT_DYNCREATE(TriangleMesh,PrimitiveList)

BOOL TriangleMesh::smallTriangles=FALSE;

WaldTriangle::WaldTriangle()
{
	k=0;
	nu=nv=nd=0.0f;
	bnu=bnv=bnd=0.0f;
	cnu=cnv=cnd=0.0f;
}

WaldTriangle::WaldTriangle(const TriangleMesh&mesh,int tri)
{
    set(mesh,tri);
}

void WaldTriangle::set(const TriangleMesh&mesh,int t)
{
	k=0;
	int tri=t*3;
	int index0=mesh.triangles[tri+0];
	int index1=mesh.triangles[tri+1];
	int index2=mesh.triangles[tri+2];
	Point3 v0p=mesh.getPoint(index0);
	Point3 v1p=mesh.getPoint(index1);
	Point3 v2p=mesh.getPoint(index2);
	Vector3 ng=Point3::normal(v0p,v1p,v2p);
	if( fabs(ng.x)>fabs(ng.y)
		&& fabs(ng.x)>fabs(ng.z) )
		k=0;
	else if( fabs(ng.y)>fabs(ng.z) )
		k=1;
	else
		k=2;
	float ax,ay,bx,by,cx,cy;
	switch(k)
	{
	case 0: 
		{
			nu=ng.y/ng.x;
			nv=ng.z/ng.x;
			nd=v0p.x+(nu*v0p.y)+(nv*v0p.z);
			ax=v0p.y;
			ay=v0p.z;
			bx=v2p.y-ax;
			by=v2p.z-ay;
			cx=v1p.y-ax;
			cy=v1p.z-ay;
			break;
		}
	case 1: 
		{
			nu=ng.z/ng.y;
			nv=ng.x/ng.y;
			nd=(nv*v0p.x)+v0p.y+(nu*v0p.z);
			ax=v0p.z;
			ay=v0p.x;
			bx=v2p.z-ax;
			by=v2p.x-ay;
			cx=v1p.z-ax;
			cy=v1p.x-ay;
			break;
		}
	case 2:
	default: 
		{
			nu=ng.x/ng.z;
			nv=ng.y/ng.z;
			nd=(nu*v0p.x)+(nv*v0p.y)+v0p.z;
			ax=v0p.x;
			ay=v0p.y;
			bx=v2p.x-ax;
			by=v2p.y-ay;
			cx=v1p.x-ax;
			cy=v1p.y-ay;
		}
	}
	float det=bx*cy-by*cx;
	bnu=-by/det;
	bnv=bx/det;
	bnd=(by*ax-bx*ay)/det;
	cnu=cy/det;
	cnv=-cx/det;
	cnd=(cx*ay-cy*ax)/det;
}

WaldTriangle& WaldTriangle::operator=(const WaldTriangle&wt)
{
	if(this==&wt) return *this;

    k=wt.k;
	nu=wt.nu;
	nv=wt.nv;
	nd=wt.nd;
	bnu=wt.bnu;
	bnv=wt.bnv;
	bnd=wt.bnd;
	cnu=wt.cnu;
	cnv=wt.cnv;
	cnd=wt.cnd;

	return *this;
}

void WaldTriangle::intersect(Ray&r,int primID,IntersectionState&state)const 
{
	switch(k)
	{
	case 0: 
		{
		float det=1.0f/(r.dx+nu*r.dy+nv*r.dz);
		float t=(nd-r.ox-nu*r.oy-nv*r.oz)*det;
		if(!r.isInside(t)) return;
		float hu=r.oy+t*r.dy;
		float hv=r.oz+t*r.dz;
		float u=hu*bnu+hv*bnv+bnd;
		if(u<0.0f) return;
		float v=hu*cnu+hv*cnv+cnd;
		if(v<0.0f) return;
		if((u+v)>1.0f) return;
		r.setMax(t);
		state.setIntersection(primID,u,v);
		return;
		}
	case 1: 
		{
		float det=1.0f/(r.dy+nu*r.dz+nv*r.dx);
		float t=(nd-r.oy-nu*r.oz-nv*r.ox)*det;
		if(!r.isInside(t)) return;
		float hu=r.oz+t*r.dz;
		float hv=r.ox+t*r.dx;
		float u=hu*bnu+hv*bnv+bnd;
		if(u<0.0f) return;
		float v=hu*cnu+hv*cnv+cnd;
		if(v<0.0f) return;
		if((u+v)>1.0f) return;
		r.setMax(t);
		state.setIntersection(primID,u,v);
		return;
		}
	case 2: 
		{
		float det=1.0f/(r.dz+nu*r.dx+nv*r.dy);
		float t=(nd-r.oz-nu*r.ox-nv*r.oy)*det;
		if(!r.isInside(t)) return;
		float hu=r.ox+t*r.dx;
		float hv=r.oy+t*r.dy;
		float u=hu*bnu+hv*bnv+bnd;
		if(u<0.0f) return;
		float v=hu*cnu+hv*cnv+cnd;
		if(v<0.0f) return;
		if((u+v)>1.0f) return;
		r.setMax(t);
		state.setIntersection(primID,u,v);
		return;
		}
	}
}

BakingSurface::BakingSurface()
{
	t_mesh=NULL;
}

BakingSurface::BakingSurface(TriangleMesh *m)
{
	t_mesh=m;
}

PrimitiveList* BakingSurface::getBakingPrimitives() const
{
	return NULL;
}

int BakingSurface::getNumPrimitives() const
{
	return t_mesh->getNumPrimitives();
}

float BakingSurface::getPrimitiveBound(int primID,int i)const 
{
	if(i>3)	return 0.0f;
	switch(t_mesh->uvs.interp) 
	{
	case ParameterList::IT_NONE:
	case ParameterList::IT_FACE:
	default:
		{
			return 0.0f;
		}
	case ParameterList::IT_VERTEX:
		{
			int tri=3*primID;
			int index0=t_mesh->triangles[tri+0];
			int index1=t_mesh->triangles[tri+1];
			int index2=t_mesh->triangles[tri+2];
			int i20=2*index0;
			int i21=2*index1;
			int i22=2*index2;
			vector<float> uvs=t_mesh->uvs.data;
			switch(i) 
			{
			case 0:
				return MathUtil::mu_min(uvs[i20+0],uvs[i21+0],uvs[i22+0]);
			case 1:
				return MathUtil::mu_max(uvs[i20+0],uvs[i21+0],uvs[i22+0]);
			case 2:
				return MathUtil::mu_min(uvs[i20+1],uvs[i21+1],uvs[i22+1]);
			case 3:
				return MathUtil::mu_max(uvs[i20+1],uvs[i21+1],uvs[i22+1]);
			default:
				return 0.0f;
			}
		}
	case ParameterList::IT_FACEVARYING: 
		{
			int idx=6*primID;
			vector<float> uvs=t_mesh->uvs.data;
			switch(i) 
			{
			case 0:
				return MathUtil::mu_min(uvs[idx+0],uvs[idx+2],uvs[idx+4]);
			case 1:
				return MathUtil::mu_max(uvs[idx+0],uvs[idx+2],uvs[idx+4]);
			case 2:
				return MathUtil::mu_min(uvs[idx+1],uvs[idx+3],uvs[idx+5]);
			case 3:
				return MathUtil::mu_max(uvs[idx+1],uvs[idx+3],uvs[idx+5]);
			default:
				return 0.0f;
			}
		}
	}

	return 0.0f;
}

BoundingBox BakingSurface::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds;
	if(o2w==Matrix4())
	{
		for(int i=0; i<t_mesh->uvs.data.size(); i+=2)
			bounds.include(t_mesh->uvs.data[i],t_mesh->uvs.data[i+1],0);
	}
	else 
	{
		for(int i=0; i<t_mesh->uvs.data.size(); i+=2) 
		{
			float x=t_mesh->uvs.data[i];
			float y=t_mesh->uvs.data[i+1];
			float wx=o2w.transformPX(x,y,0);
			float wy=o2w.transformPY(x,y,0);
			float wz=o2w.transformPZ(x,y,0);
			bounds.include(wx,wy,wz);
		}
	}

	return bounds;
}

void BakingSurface::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	float uv00=0.0f,uv01=0.0f,uv10=0.0f,uv11=0.0f,uv20=0.0f,uv21=0.0f;
	switch(t_mesh->uvs.interp)
	{
	case ParameterList::IT_NONE:
	case ParameterList::IT_FACE:
	default:
		return;
	case ParameterList::IT_VERTEX: 
		{
			int tri=3*primID;
			int index0=t_mesh->triangles[tri+0];
			int index1=t_mesh->triangles[tri+1];
			int index2=t_mesh->triangles[tri+2];
			int i20=2*index0;
			int i21=2*index1;
			int i22=2*index2;
			vector<float> uvs=t_mesh->uvs.data;
			uv00=uvs[i20+0];
			uv01=uvs[i20+1];
			uv10=uvs[i21+0];
			uv11=uvs[i21+1];
			uv20=uvs[i22+0];
			uv21=uvs[i22+1];
			break;
		}
	case ParameterList::IT_FACEVARYING:
		{
			int idx=(3*primID)<<1;
			vector<float> uvs=t_mesh->uvs.data;
			uv00=uvs[idx+0];
			uv01=uvs[idx+1];
			uv10=uvs[idx+2];
			uv11=uvs[idx+3];
			uv20=uvs[idx+4];
			uv21=uvs[idx+5];
			break;
		}
	}

	double edge1x=uv10-uv00;
	double edge1y=uv11-uv01;
	double edge2x=uv20-uv00;
	double edge2y=uv21-uv01;
	double pvecx=r.dy*0.0-r.dz*edge2y;
	double pvecy=r.dz*edge2x-r.dx*0.0;
	double pvecz=r.dx*edge2y-r.dy*edge2x;
	double qvecx,qvecy,qvecz;
	double u,v;
	double det=edge1x*pvecx+edge1y*pvecy+0.0*pvecz;
	if(det>0.0) 
	{
		double tvecx=r.ox-uv00;
		double tvecy=r.oy-uv01;
		double tvecz=r.oz;
		u=tvecx*pvecx+tvecy*pvecy+tvecz*pvecz;
		if( u<0.0 || u>det ) return;
		qvecx=tvecy*0.0-tvecz*edge1y;
		qvecy=tvecz*edge1x-tvecx*0.0;
		qvecz=tvecx*edge1y-tvecy*edge1x;
		v=r.dx*qvecx+r.dy*qvecy+r.dz*qvecz;
		if( v<0.0 || u+v>det )	return;
	} 
	else if(det<0.0)
	{
		double tvecx=r.ox-uv00;
		double tvecy=r.oy-uv01;
		double tvecz=r.oz;
		u=tvecx*pvecx+tvecy*pvecy+tvecz*pvecz;
		if(u>0.0 || u<det)	return;
		qvecx=tvecy*0.0-tvecz*edge1y;
		qvecy=tvecz*edge1x-tvecx*0.0;
		qvecz=tvecx*edge1y-tvecy*edge1x;
		v=r.dx*qvecx+r.dy*qvecy+ r.dz*qvecz;
		if( v>0.0 || u+v<det ) return;
	} 
	else return;

	double inv_det=1.0/det;
	float t=(float)((edge2x*qvecx+edge2y*qvecy+0*qvecz)*inv_det);
	if(r.isInside(t)) 
	{
		r.setMax(t);
		state.setIntersection(primID,(float)(u*inv_det),(float)(v*inv_det));
	}
}

void BakingSurface::prepareShadingState(ShadingState&state)
{
	state.init();
	Instance* parent=state.getInstance();
	int primID=state.getPrimitiveID();
	float u=state.getU();
	float v=state.getV();
	float w=1.0f-u-v;	
	int tri=3*primID;
	int index0=t_mesh->triangles[tri+0];
	int index1=t_mesh->triangles[tri+1];
	int index2=t_mesh->triangles[tri+2];
	Point3 v0p=t_mesh->getPoint(index0);
	Point3 v1p=t_mesh->getPoint(index1);
	Point3 v2p=t_mesh->getPoint(index2);
	
	state.ss_point3().x=w*v0p.x+u*v1p.x+v*v2p.x;
	state.ss_point3().y=w*v0p.y+u*v1p.y+v*v2p.y;
	state.ss_point3().z=w*v0p.z+u*v1p.z+v*v2p.z;	
	state.ss_point3().set(state.transformObjectToWorld(state.getPoint()));

	Vector3 ng=Point3::normal(v0p,v1p,v2p);
	ng=state.transformNormalObjectToWorld(ng);
	ng.normalize();
	state.ss_normal().set(ng);

	switch(t_mesh->normals.interp)
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
			vector<float> normals=t_mesh->normals.data;
			state.ss_normal().x=w*normals[i30+0]+u*normals[i31+0]+v*normals[i32+0];
			state.ss_normal().y=w*normals[i30+1]+u*normals[i31+1]+v*normals[i32+1];
			state.ss_normal().z=w*normals[i30+2]+u*normals[i31+2]+v*normals[i32+2];
			state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
			state.ss_normal().normalize();
			break;
		}
		case ParameterList::IT_FACEVARYING: 
		{
			int idx=3*tri;
			vector<float> normals=t_mesh->normals.data;
			state.ss_normal().x=w*normals[idx+0]+u*normals[idx+3]+v*normals[idx+6];
			state.ss_normal().y=w*normals[idx+1]+u*normals[idx+4]+v*normals[idx+7];
			state.ss_normal().z=w*normals[idx+2]+u*normals[idx+5]+v*normals[idx+8];
			state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
			state.ss_normal().normalize();
			break;
		}
	}

	float uv00=0.0f,uv01=0.0f,uv10=0.0f,uv11=0.0f,uv20=0.0f,uv21=0.0f;
	switch(t_mesh->uvs.interp) 
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
			vector<float> uvs=t_mesh->uvs.data;
			uv00=uvs[i20+0];
			uv01=uvs[i20+1];
			uv10=uvs[i21+0];
			uv11=uvs[i21+1];
			uv20=uvs[i22+0];
			uv21=uvs[i22+1];
			break;
		}
	case ParameterList::IT_FACEVARYING: 
		{
			int idx=tri<<1;
			vector<float> uvs=t_mesh->uvs.data;
			uv00=uvs[idx+0];
			uv01=uvs[idx+1];
			uv10=uvs[idx+2];
			uv11=uvs[idx+3];
			uv20=uvs[idx+4];
			uv21=uvs[idx+5];
			break;
		}
	}
	if(t_mesh->uvs.interp!=ParameterList::IT_NONE) 
	{		
		state.ss_tex().x=w*uv00+u*uv10+v*uv20;
		state.ss_tex().y=w*uv01+u*uv11+v*uv21;
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

	int shaderIndex;
	shaderIndex=t_mesh->faceShaders.empty()?0:(t_mesh->faceShaders[primID]&0xFF);
	state.setShader(parent->getShader(shaderIndex));
}

BOOL BakingSurface::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

void TriangleMesh::setSmallTriangles(BOOL sTriangles)
{
	if(sTriangles)
		StrUtil::PrintPrompt("小trimesh模式: 开");
	else
		StrUtil::PrintPrompt("小trimeshmode: 关");
	smallTriangles=sTriangles;
}

TriangleMesh::TriangleMesh() 
{			
}

void TriangleMesh::writeObj(CString fileName)
{	
	CFile triFile;	
	if( !triFile.Open(fileName,CFile::modeWrite | CFile::typeText) )
		return;

    CString str;
	str.Format("o object\n");
	triFile.Write(str,str.GetLength());
	for(int i=0; i<points.size(); i+=3)
	{
        str.Format(_T("v %g %g %g\n"),points[i],points[i+1],points[i+2]);
		triFile.Write(str,str.GetLength());
	}
	str.Format("s off\n");
	triFile.Write(str,str.GetLength());
	for(int i=0; i<triangles.size(); i+=3)
	{
		str.Format(_T("f %d %d %d\n"),triangles[i]+1,triangles[i+1]+1,triangles[i+2]+1);
		triFile.Write(str,str.GetLength());
	}
	triFile.Close();	
}

BOOL TriangleMesh::update(ParameterList&pl,LGAPI&api) 
{
	BOOL updatedTopology=FALSE;
	{
		vector<int> tries=pl.getIntArray("triangles");
		if(!tries.empty()) 
		{
			triangles=tries;
			updatedTopology=FALSE;
		}
	}
	if(triangles.empty())
	{
		StrUtil::PrintPrompt("不能更新mesh -三角形索引缺失");
		return FALSE;
	}
	if(triangles.size()%3!=0)
		StrUtil::PrintPrompt("三角形索引数据不是3的乘数 -三角形缺失");
	pl.setFaceCount(triangles.size()/3);
	{
		ParameterList::FloatParameter pointsP=pl.getPointArray("points");		
		if(pointsP.interp!=ParameterList::IT_VERTEX)
			StrUtil::PrintPrompt("点插值类型必须设定为\"vertex\" -原来是\"%s\"", 
			ParameterList::GetInterpolationTypeName(pointsP.interp).MakeLower());
		else 
		{
			points=pointsP.data;
			updatedTopology=TRUE;
		}
	}
	if(points.empty()) 
	{
		StrUtil::PrintPrompt("不能更新mesh -顶点缺失");
		return FALSE;
	}
	pl.setVertexCount(points.size()/3);
	pl.setFaceVertexCount(3*(triangles.size()/3));
	ParameterList::FloatParameter nors=pl.getVectorArray("normals");
    if(!nors.isNull())
	    normals=nors;
	ParameterList::FloatParameter s=pl.getTexCoordArray("uvs");
	if(!s.isNull())
        uvs=s;	
	vector<int> fShaders=pl.getIntArray("faceshaders");
	if( !fShaders.empty() && fShaders.size()==triangles.size()/3 )
	{
		faceShaders.resize(fShaders.size());
		for(int i=0; i<faceShaders.size(); i++) 
		{
			int v=fShaders[i];
			if(v>255)
				StrUtil::PrintPrompt("三角形上的Shader索引%d太大",i);
			faceShaders[i]=(byte)(v&0xFF);
		}
	}
	if(updatedTopology)	init();	

	return TRUE;
}

float TriangleMesh::getPrimitiveBound(int primID,int i) const
{
	int tri=3*primID;
	int a=3*triangles[tri+0];
	int b=3*triangles[tri+1];
	int c=3*triangles[tri+2];
	unsigned int axis=(unsigned int)i>>1;
	if( (i&1)==0 )
		return MathUtil::mu_min(points[a+axis],points[b+axis],points[c+axis]);
	else
		return MathUtil::mu_max(points[a+axis],points[b+axis],points[c+axis]);
}

BoundingBox TriangleMesh::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds;
	if(o2w==Matrix4())
	{
		for (int i=0; i<points.size(); i+=3)
			bounds.include(points[i],points[i+1],points[i+2]);
	}
	else
	{
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
	}

	return bounds;
}

void TriangleMesh::intersectTriangleKensler(Ray&r,int primID,IntersectionState&state) const
{
	int tri=3*primID;
	int a=3*triangles[tri+0];
	int b=3*triangles[tri+1];
	int c=3*triangles[tri+2];
	float edge0x=points[b+0]-points[a+0];
	float edge0y=points[b+1]-points[a+1];
	float edge0z=points[b+2]-points[a+2];
	float edge1x=points[a+0]-points[c+0];
	float edge1y=points[a+1]-points[c+1];
	float edge1z=points[a+2]-points[c+2];
	float nx=edge0y*edge1z-edge0z*edge1y;
	float ny=edge0z*edge1x-edge0x*edge1z;
	float nz=edge0x*edge1y-edge0y*edge1x;
	float v=r.dot(nx,ny,nz);
	float iv=1.0f/v;
	float edge2x=points[a+0]-r.ox;
	float edge2y=points[a+1]-r.oy;
	float edge2z=points[a+2]-r.oz;
	float vva=nx*edge2x+ny*edge2y+nz*edge2z;
	float t=iv*vva;
	if(!r.isInside(t)) return;
	float ix=edge2y*r.dz-edge2z*r.dy;
	float iy=edge2z*r.dx-edge2x*r.dz;
	float iz=edge2x*r.dy-edge2y*r.dx;
	float v1=ix*edge1x+iy*edge1y+iz*edge1z;
	float beta=iv*v1;
	if(beta<0.0f) return;
	float v2=ix*edge0x+iy*edge0y+iz*edge0z;
	if((v1+v2)*v>v*v) return;
	float gamma=iv*v2;
	if(gamma<0.0f) return;
	r.setMax(t);
	state.setIntersection(primID,beta,gamma);
}

void TriangleMesh::intersectPrimitive(Ray&r,int primID,IntersectionState&state) const
{
	if(!triAccel.empty()) 
	{		
		triAccel[primID].intersect(r,primID,state);
		return;
	}
	intersectTriangleKensler(r,primID,state);
}

int TriangleMesh::getNumPrimitives() const
{
	return triangles.size()/3;
}

void TriangleMesh::prepareShadingState(ShadingState&state) 
{
	state.init();
	Instance* parent=state.getInstance();
	int primID=state.getPrimitiveID();
	float u=state.getU();
	float v=state.getV();
	float w=1.0f-u-v;
	state.getRay().getPoint(state.ss_point3());
	int tri=3*primID;
	int index0=triangles[tri+0];
	int index1=triangles[tri+1];
	int index2=triangles[tri+2];
	Point3 v0p=getPoint(index0);
	Point3 v1p=getPoint(index1);
	Point3 v2p=getPoint(index2);
	Vector3 ng=Point3::normal(v0p,v1p,v2p);
	if(!parent)
	    ng=state.transformNormalObjectToWorld(ng);
	ng.normalize();
	state.ss_geoNormal().set(ng);

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
			vector<float> nors=normals.data;
			state.ss_normal().x=w*nors[i30+0]+u*nors[i31+0]+v*nors[i32+0];
			state.ss_normal().y=w*nors[i30+1]+u*nors[i31+1]+v*nors[i32+1];
			state.ss_normal().z=w*nors[i30+2]+u*nors[i31+2]+v*nors[i32+2];
			if(!parent)
			    state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
			state.ss_normal().normalize();
			break;
		}
	case ParameterList::IT_FACEVARYING: 
		{
			int idx=3*tri;
			vector<float> nors=normals.data;
			state.ss_normal().x=w*nors[idx+0]+u*nors[idx+3]+v*nors[idx+6];
			state.ss_normal().y=w*nors[idx+1]+u*nors[idx+4]+v*nors[idx+7];
			state.ss_normal().z=w*nors[idx+2]+u*nors[idx+5]+v*nors[idx+8];
			if(!parent)
			    state.ss_normal().set(state.transformNormalObjectToWorld(state.getNormal()));
			state.ss_normal().normalize();
			break;
		}
	}
	float uv00=0.0f,uv01=0.0f,uv10=0.0f,uv11=0.0f,uv20=0.0f,uv21=0.0f;
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
			vector<float> s=uvs.data;
			uv00=s[i20+0];
			uv01=s[i20+1];
			uv10=s[i21+0];
			uv11=s[i21+1];
			uv20=s[i22+0];
			uv21=s[i22+1];
			break;
		}
	case ParameterList::IT_FACEVARYING:
		{
			int idx=tri<<1;
			vector<float> s=uvs.data;
			uv00=s[idx+0];
			uv01=s[idx+1];
			uv10=s[idx+2];
			uv11=s[idx+3];
			uv20=s[idx+4];
			uv21=s[idx+5];
			break;
		}
	}
	if(uvs.interp!=ParameterList::IT_NONE)
	{		
		state.ss_tex().x=w*uv00+u*uv10+v*uv20;
		state.ss_tex().y=w*uv01+u*uv11+v*uv21;
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
			if(!parent)
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

void TriangleMesh::init() 
{
	triAccel.clear();
	int nt=getNumPrimitives();
	if(!smallTriangles)
	{		
		if(nt>2000000)
		{
			StrUtil::PrintPrompt("TRI -太多三角形--跳过加速三角形的生成");
			return;
		}
		triAccel.resize(nt);
		for(int i=0; i<nt; i++)
			triAccel[i].set(*this,i);
	}
}

Point3 TriangleMesh::getPoint(int i)const
{
	i*=3;

	return Point3(points[i],points[i+1],points[i+2]);
}

Point3 TriangleMesh::getPoint(int tri,int i) const
{
	Point3 p;
	int index=3*triangles[3*tri+i];
	p.set(points[index],points[index+1],points[index+2]);

	return p;
}

PrimitiveList* TriangleMesh::getBakingPrimitives() const
{
	switch(uvs.interp) 
	{
	case ParameterList::IT_NONE:
	case ParameterList::IT_FACE:
		StrUtil::PrintPrompt("没有纹理坐标数据不能生成烘焙面");
		return NULL;
	default:
		{
			PrimitiveList *p=new BakingSurface((TriangleMesh*)this);
            return p;
		}		
	}
}
