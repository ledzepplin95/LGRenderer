#include "StdAfx.h"

#include "UniformGrid.h"
#include "strutil.h"
#include "float.h"
#include "timer.h"
#include "MathUtil.h"
#include "IntersectionState.h"
#include "primitivelist.h"
#include "ray.h"
#include "BoundingBox.h"
#include "vector3.h"
#include "Matrix4.h"

LG_IMPLEMENT_DYNCREATE(UniformGrid,AccelerationStructure)

UniformGrid::UniformGrid() 
{
	primitives=NULL;
	nx=ny=nz=0;
	voxelwx=voxelwy=voxelwz=0.0f;
	invVoxelwx=invVoxelwy=invVoxelwz=0.0f;
}

void UniformGrid::build(PrimitiveList*p) 
{
	Timer t;
	t.start();
	primitives=p;
	int n=p->getNumPrimitives();	
	bounds=p->getWorldBounds(Matrix4());	
	bounds.enlargeUlps();
	Vector3 w=bounds.getExtents();
	double s=pow((float)((w.x*w.y*w.z)/n),1.0f/3.0f);
	nx=MathUtil::clamp((int)((w.x/s)+0.5),1,128);
	ny=MathUtil::clamp((int)((w.y/s)+0.5),1,128);
	nz=MathUtil::clamp((int)((w.z/s)+0.5),1,128);
	voxelwx=w.x/nx;
	voxelwy=w.y/ny;
	voxelwz=w.z/nz;
	invVoxelwx=1.0f/voxelwx;
	invVoxelwy=1.0f/voxelwy;
	invVoxelwz=1.0f/voxelwz;
	StrUtil::PrintPrompt("创建网格: %dx%dx%d...",nx,ny,nz);
	vector<IntArray> buildCells;
	buildCells.resize(nx*ny*nz);	
	vector<int> iMin;
	iMin.resize(3);
	vector<int> iMax;
	iMax.resize(3);
	int numCellsPerObject=0;
	for(int i=0; i<n; i++)
	{
		getGridIndex(primitives->getPrimitiveBound(i,0),primitives->getPrimitiveBound(i,2),
			primitives->getPrimitiveBound(i,4),iMin);
		getGridIndex(primitives->getPrimitiveBound(i,1),primitives->getPrimitiveBound(i,3),
			primitives->getPrimitiveBound(i,5),iMax);
		for(int ix=iMin[0]; ix<=iMax[0]; ix++) 
		{
			for(int iy=iMin[1]; iy<=iMax[1]; iy++) 
			{
				for(int iz=iMin[2]; iz<=iMax[2]; iz++) 
				{
					int idx=ix+(nx*iy)+(nx*ny*iz);					
					buildCells[idx].add(i);
					numCellsPerObject++;
				}
			}
		}
	}
	StrUtil::PrintPrompt("创建单元...");
	int numEmpty=0;
	int numInFull=0;
	cells.resize(nx*ny*nz);	
	for(int i=0; i<buildCells.size(); i++) 
	{	
		const IntArray& cell=buildCells[i];		
		if(cell.getSize()==0)
		{
			numEmpty++;				
		}
		else 
		{
			cells[i]=cell;
			numInFull+=cell.getSize();
		}			
	}
	t.end();
	StrUtil::PrintPrompt("均匀网格统计:");
	StrUtil::PrintPrompt("  * 网格单元:            %d",cells.size());
	StrUtil::PrintPrompt("  * 已使用单元:          %d",cells.size()-numEmpty);
	StrUtil::PrintPrompt("  * 空单元:              %d",numEmpty);
	StrUtil::PrintPrompt("  * 利用率:              %.2f%%",100.0*(cells.size()-numEmpty)/cells.size());
	StrUtil::PrintPrompt("  * 对象数/单元:         %.2f",(double)numInFull/(double)cells.size());
	StrUtil::PrintPrompt("  * 对象数/已使用单元:   %.2f",(double)numInFull/(double)(cells.size()-numEmpty));
	StrUtil::PrintPrompt("  * 单元数/对象:         %.2f",(double)numCellsPerObject/(double)n);
	StrUtil::PrintPrompt("  * 建造时间:            %s",t.toString());
}

void UniformGrid::intersect(Ray&r,IntersectionState&state)const 
{
	float intervalMin=r.getMin();
	float intervalMax=r.getMax();
	float orgX=r.ox;
	float dirX=r.dx,invDirX=1.0f/dirX;
	float t1,t2;
	t1=(bounds.getMinimum().x-orgX)*invDirX;
	t2=(bounds.getMaximum().x-orgX)*invDirX;
	if(invDirX>0.0f) 
	{
		if(t1>intervalMin)
			intervalMin=t1;
		if(t2<intervalMax)
			intervalMax=t2;
	} 
	else
	{
		if(t2>intervalMin)
			intervalMin=t2;
		if(t1<intervalMax)
			intervalMax=t1;
	}
	if(intervalMin>intervalMax)
		return;
	float orgY=r.oy;
	float dirY=r.dy,invDirY=1.0f/dirY;
	t1=(bounds.getMinimum().y-orgY)*invDirY;
	t2=(bounds.getMaximum().y-orgY)*invDirY;
	if(invDirY>0.0f) 
	{
		if(t1>intervalMin)
			intervalMin=t1;
		if(t2<intervalMax)
			intervalMax=t2;
	} 
	else 
	{
		if(t2>intervalMin)
			intervalMin=t2;
		if(t1<intervalMax)
			intervalMax=t1;
	}
	if(intervalMin>intervalMax)
		return;
	float orgZ=r.oz;
	float dirZ=r.dz,invDirZ=1.0f/dirZ;
	t1=(bounds.getMinimum().z-orgZ)*invDirZ;
	t2=(bounds.getMaximum().z-orgZ)*invDirZ;
	if(invDirZ>0.0f) 
	{
		if(t1>intervalMin)
			intervalMin=t1;
		if(t2<intervalMax)
			intervalMax=t2;
	} 
	else 
	{
		if(t2>intervalMin)
			intervalMin=t2;
		if(t1<intervalMax)
			intervalMax=t1;
	}
	if(intervalMin>intervalMax)
		return;
	orgX+=intervalMin*dirX;
	orgY+=intervalMin*dirY;
	orgZ+=intervalMin*dirZ;
	int indxX,indxY,indxZ;
	int stepX,stepY,stepZ;
	int stopX,stopY,stopZ;
	float deltaX,deltaY,deltaZ;
	float tnextX,tnextY,tnextZ;
	indxX=(int)((orgX-bounds.getMinimum().x)*invVoxelwx);
	if(indxX<0)
		indxX=0;
	else if(indxX>=nx)
		indxX=nx-1;
	if(fabs(dirX)<1e-6f) 
	{
		stepX=0;
		stopX=indxX;
		deltaX=0;
		tnextX=FLT_MAX;
	} 
	else if(dirX>0.0f) 
	{
		stepX=1;
		stopX=nx;
		deltaX=voxelwx*invDirX;
		tnextX=intervalMin+((indxX+1)*voxelwx
			+bounds.getMinimum().x-orgX)*invDirX;
	} 
	else 
	{
		stepX=-1;
		stopX=-1;
		deltaX=-voxelwx*invDirX;
		tnextX=intervalMin+(indxX*voxelwx 
			+bounds.getMinimum().x-orgX)*invDirX;
	}	
	indxY=(int)((orgY-bounds.getMinimum().y)*invVoxelwy);
	if(indxY<0)
		indxY=0;
	else if(indxY>=ny)
		indxY=ny-1;
	if(fabs(dirY)<1e-6f)
	{
		stepY=0;
		stopY=indxY;
		deltaY=0;
		tnextY=FLT_MAX;
	} 
	else if(dirY>0.0f)
	{
		stepY=1;
		stopY=ny;
		deltaY=voxelwy*invDirY;
		tnextY=intervalMin+((indxY+1)*voxelwy 
			+bounds.getMinimum().y-orgY)*invDirY;
	} 
	else
	{
		stepY=-1;
		stopY=-1;
		deltaY=-voxelwy*invDirY;
		tnextY=intervalMin+(indxY*voxelwy 
			+bounds.getMinimum().y-orgY)*invDirY;
	}
	indxZ=(int)((orgZ-bounds.getMinimum().z)*invVoxelwz);
	if(indxZ<0)
		indxZ=0;
	else if(indxZ>=nz)
		indxZ=nz-1;
	if(fabs(dirZ)<1e-6f) 
	{
		stepZ=0;
		stopZ=indxZ;
		deltaZ=0;
		tnextZ=FLT_MAX;
	}
	else if(dirZ>0.0f) 
	{
		stepZ=1;
		stopZ=nz;
		deltaZ=voxelwz*invDirZ;
		tnextZ=intervalMin+((indxZ+1)*voxelwz
			+bounds.getMinimum().z-orgZ)*invDirZ;
	}
	else
	{
		stepZ=-1;
		stopZ=-1;
		deltaZ=-voxelwz*invDirZ;
		tnextZ=intervalMin+(indxZ*voxelwz+bounds.getMinimum().z-orgZ)*invDirZ;
	}
	int cellstepX=stepX;
	int cellstepY=stepY*nx;
	int cellstepZ=stepZ*ny*nx;
	int cell=indxX+indxY*nx+indxZ*ny*nx;	
	for(;;)
	{
		if( tnextX<tnextY && tnextX<tnextZ ) 
		{			
			for(int i=0; i<cells[cell].getSize(); i++)
				primitives->intersectPrimitive(r,cells[cell].get(i),state);
			if( state.hit()&&( r.getMax()<tnextX && r.getMax()<intervalMax ) )
				return;
			
			intervalMin=tnextX;
			if(intervalMin>intervalMax)
				return;
			indxX+=stepX;
			if(indxX==stopX)
				return;
			tnextX+=deltaX;
			cell+=cellstepX;
		} 
		else if(tnextY<tnextZ)
		{			
			for(int i=0; i<cells[cell].getSize(); i++)
				primitives->intersectPrimitive(r,cells[cell].get(i),state);
			if( state.hit() && (r.getMax()<tnextY && r.getMax()<intervalMax) )
				return;
			
			intervalMin=tnextY;
			if(intervalMin>intervalMax)
				return;
			indxY+=stepY;
			if(indxY==stopY)
				return;
			tnextY+=deltaY;
			cell+=cellstepY;
		} 
		else
		{
			for(int i=0; i<cells[cell].getSize(); i++)
				primitives->intersectPrimitive(r,cells[cell].get(i),state);
			if( state.hit() && (r.getMax()<tnextZ && r.getMax()<intervalMax) )
				return;
			
			intervalMin=tnextZ;
			if(intervalMin>intervalMax)
				return;
			indxZ+=stepZ;
			if(indxZ==stopZ)
				return;
			tnextZ+=deltaZ;
			cell+=cellstepZ;
		}
	}
}

void UniformGrid::getGridIndex(float x,float y,float z,vector<int>&i)const 
{
	i[0]=MathUtil::clamp((int)((x-bounds.getMinimum().x)*invVoxelwx),0,nx-1);
	i[1]=MathUtil::clamp((int)((y-bounds.getMinimum().y)*invVoxelwy),0,ny-1);
	i[2]=MathUtil::clamp((int)((z-bounds.getMinimum().z)*invVoxelwz),0,nz-1);
}
