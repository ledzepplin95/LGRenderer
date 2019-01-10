#include "StdAfx.h"

#include "BoundingIntervalHierarchy.h"
#include "strutil.h"
#include "memory.h"
#include "timer.h"
#include "mathutil.h"
#include "float.h"
#include "IntersectionState.h"
#include "PrimitiveList.h"
#include "ray.h"
#include "matrix4.h"
#include "IntArray.h"
#include <boost/math/special_functions/fpclassify.hpp>

LG_IMPLEMENT_DYNCREATE(BoundingIntervalHierarchy,AccelerationStructure)

BoundingIntervalHierarchy::BuildStats& BoundingIntervalHierarchy::BuildStats::operator=(
	const BuildStats&stat)
{
	if(this==&stat) return *this;

	numNodes=stat.numNodes;
	numLeaves=stat.numLeaves;
	sumObjects=stat.sumObjects;
	minObjects=stat.minObjects;
	maxObjects=stat.maxObjects;
	sumDepth=stat.sumDepth;
	minDepth=stat.minDepth;
	maxDepth=stat.maxDepth;
	numLeaves0=stat.numLeaves0;
	numLeaves1=stat.numLeaves1;
	numLeaves2=stat.numLeaves2;
	numLeaves3=stat.numLeaves3;
	numLeaves4=stat.numLeaves4;
	numLeaves4p=stat.numLeaves4p;
	numBVH2=stat.numBVH2;

	return *this;
}

BoundingIntervalHierarchy::BuildStats::BuildStats() 
{
	numNodes=numLeaves=0;
	sumObjects=0;
	minObjects=INT_MAX;
	maxObjects=INT_MIN;
	sumDepth=0;
	minDepth=INT_MAX;
	maxDepth=INT_MIN;
	numLeaves0=0;
	numLeaves1=0;
	numLeaves2=0;
	numLeaves3=0;
	numLeaves4=0;
	numLeaves4p=0;
	numBVH2=0;
}

void BoundingIntervalHierarchy::BuildStats::updateInner() 
{
	numNodes++;
}

void BoundingIntervalHierarchy::BuildStats::updateBVH2() 
{
	numBVH2++;
}

void BoundingIntervalHierarchy::BuildStats::updateLeaf(int depth,int n) 
{
	numLeaves++;
	minDepth=min(depth,minDepth);
	maxDepth=max(depth,maxDepth);
	sumDepth+=depth;
	minObjects=min(n,minObjects);
	maxObjects=max(n,maxObjects);
	sumObjects+=n;
	switch(n) 
	{
	case 0:
		numLeaves0++;
		break;
	case 1:
		numLeaves1++;
		break;
	case 2:
		numLeaves2++;
		break;
	case 3:
		numLeaves3++;
		break;
	case 4:
		numLeaves4++;
		break;
	default:
		numLeaves4p++;
		break;
	}
}

void BoundingIntervalHierarchy::BuildStats::printStats() const
{
	StrUtil::PrintPrompt("树统计:");
	StrUtil::PrintPrompt("  * 节点:             %d",numNodes);
	StrUtil::PrintPrompt("  * 叶子:             %d",numLeaves);
	StrUtil::PrintPrompt("  * 对象: 最小值      %d",minObjects);
	StrUtil::PrintPrompt("          平均        %.2f", 
		(float)sumObjects/numLeaves);
	StrUtil::PrintPrompt("          平均(n>0)   %.2f", 
		(float)sumObjects/(numLeaves-numLeaves0));
	StrUtil::PrintPrompt("          最大值      %d",maxObjects);
	StrUtil::PrintPrompt("  * 深度: 最小值      %d",minDepth);
	StrUtil::PrintPrompt("          平均        %.2f", 
		(float)sumDepth/numLeaves);
	StrUtil::PrintPrompt("          最大值      %d",maxDepth);
	StrUtil::PrintPrompt("  * 叶子 w/: N=0     %3d%%",100*numLeaves0/numLeaves);
	StrUtil::PrintPrompt("             N=1     %3d%%",100*numLeaves1/numLeaves);
	StrUtil::PrintPrompt("             N=2     %3d%%",100*numLeaves2/numLeaves);
	StrUtil::PrintPrompt("             N=3     %3d%%",100*numLeaves3/numLeaves);
	StrUtil::PrintPrompt("             N=4     %3d%%",100*numLeaves4/numLeaves);
	StrUtil::PrintPrompt("             N>4     %3d%%",100*numLeaves4p/numLeaves);
	StrUtil::PrintPrompt("  * BVH2结点:    %d(%d%%)",numBVH2,
		100*numBVH2/(numNodes+numLeaves-2*numBVH2));
}

BoundingIntervalHierarchy::BoundingIntervalHierarchy() 
{
	primitives=NULL;
	maxPrims=2;
}

void BoundingIntervalHierarchy::build(PrimitiveList*p) 
{
	primitives=p;
	int n=p->getNumPrimitives();
	StrUtil::PrintPrompt("得到包围盒...");
	bounds=p->getWorldBounds(Matrix4());
	objects.resize(n);
	for(int i=0; i<n; i++)
		objects[i]=i;
	StrUtil::PrintPrompt("创建树...");
	int initialSize=3*(2*6*n+1);
	IntArray tempTree((initialSize+3)/4);	
	BuildStats stats;
	Timer t;
	t.start();
	buildHierarchy(tempTree,objects,stats);
	t.end();
	tree=tempTree.trim();
	stats.printStats();
	StrUtil::PrintPrompt("  * 创建时间:    %s",t.toString());
	StrUtil::PrintPrompt("  * 初始化使用: %6.2f%%", 
		(100.0*tree.size())/initialSize);
	StrUtil::PrintPrompt("  * 树内存:      %s",Memory::m_sizeof(tree));
	StrUtil::PrintPrompt("  * 索引内存:    %s",Memory::m_sizeof(objects));
}

void BoundingIntervalHierarchy::buildHierarchy(IntArray&tempTree,vector<int>&indices,
											   BuildStats&stats)
{	
	tempTree.add(3<<30); 
	tempTree.add(0);
	tempTree.add(0);
	if(objects.empty())
		return;	
	vector<float> gridBox;
	gridBox.push_back(bounds.getMinimum().x);
	gridBox.push_back(bounds.getMaximum().x);
	gridBox.push_back(bounds.getMinimum().y);
	gridBox.push_back(bounds.getMaximum().y);
	gridBox.push_back(bounds.getMinimum().z);
	gridBox.push_back(bounds.getMaximum().z);
	vector<float> nodeBox;
	nodeBox.push_back(bounds.getMinimum().x);
	nodeBox.push_back(bounds.getMaximum().x);
	nodeBox.push_back(bounds.getMinimum().y);
	nodeBox.push_back(bounds.getMaximum().y);
	nodeBox.push_back(bounds.getMinimum().z);
	nodeBox.push_back(bounds.getMaximum().z);
	subdivide(0,objects.size()-1,tempTree,indices,gridBox,nodeBox,0,1,stats);
}

void BoundingIntervalHierarchy::createNode(IntArray&tempTree,int nodeIndex, 
										   int left,int right) 
{
	tempTree.set(nodeIndex+0,(3<<30)|left);
	tempTree.set(nodeIndex+1,right-left+1);
}

void BoundingIntervalHierarchy::subdivide(int left,int right,IntArray&tempTree,
	vector<int>&indices,vector<float>&gridBox,vector<float>&nodeBox,
	int nodeIndex,int depth,BuildStats&stats) 
{
	if( (right-left+1)<=maxPrims || depth>=64 ) 
	{		
		stats.updateLeaf(depth,right-left+1);
		createNode(tempTree,nodeIndex,left,right);
		return;
	}
	
	int axis=-1,prevAxis,rightOrig;
	float clipL=MathUtil::NotANumber,clipR=MathUtil::NotANumber,prevClip=MathUtil::NotANumber;
	float split=MathUtil::NotANumber,prevSplit;
	BOOL wasLeft=TRUE;
	while(TRUE) 
	{
		prevAxis=axis;
		prevSplit=split;		
		float d[]={gridBox[1]-gridBox[0],gridBox[3]-gridBox[2],
			gridBox[5]-gridBox[4] };
		if( d[0]<0.0f || d[1]<0.0f || d[2]<0.0f ) return;			
		for(int i=0; i<3; i++) 
		{
			if( nodeBox[2*i+1]<gridBox[2*i] || nodeBox[2*i]>gridBox[2*i+1] ) 
			{
				StrUtil::PrintPrompt("到达树中错误区域，放弃带有%d个对象的结点",right-left+1);				
			}
		}		
		if( d[0]>d[1] && d[0]>d[2] )
			axis=0;
		else if(d[1]>d[2])
			axis=1;
		else
			axis=2;
		split=0.5f*(gridBox[2*axis]+gridBox[2*axis+1]);		
		clipL=-FLT_MAX;
		clipR=FLT_MAX;
		rightOrig=right;
		float nodeL=FLT_MAX;
		float nodeR=-FLT_MAX;
		for(int i=left; i<=right;)
		{
			int obj=indices[i];
			float minb=primitives->getPrimitiveBound(obj,2*axis+0);
			float maxb=primitives->getPrimitiveBound(obj,2*axis+1);
			float center=(minb+maxb)*0.5f;
			if(center<=split) 
			{				
				i++;
				if(clipL<maxb)
					clipL=maxb;
			} 
			else 
			{				
				int t=indices[i];
				indices[i]=indices[right];
				indices[right]=t;
				right--;
				if(clipR>minb)
					clipR=minb;
			}
			if(nodeL>minb)
				nodeL=minb;
			if(nodeR<maxb)
				nodeR=maxb;
		}		
		if( nodeL>nodeBox[2*axis+0] && nodeR<nodeBox[2*axis+1] )
		{
			float nodeBoxW=nodeBox[2*axis+1]-nodeBox[2*axis+0];
			float nodeNewW=nodeR-nodeL;			
			if( (1.3f*nodeNewW)<nodeBoxW ) 
			{
				stats.updateBVH2();
				int nextIndex=tempTree.getSize();			
				tempTree.add(0);
				tempTree.add(0);
				tempTree.add(0);			
				stats.updateInner();
				tempTree.set(nodeIndex+0,(axis<<30)|(1<<29)|nextIndex);
				tempTree.set(nodeIndex+1,MathUtil::floatToRawIntBits(nodeL));
				tempTree.set(nodeIndex+2,MathUtil::floatToRawIntBits(nodeR));			
				nodeBox[2*axis+0]=nodeL;
				nodeBox[2*axis+1]=nodeR;
				subdivide(left,rightOrig,tempTree,indices, 
					gridBox,nodeBox,nextIndex,depth+1,stats);
				return;
			}
		}	
		if(right==rightOrig) 
		{		
			if(clipL<=split) 
			{			
				gridBox[2*axis+1]=split;
				prevClip=clipL;
				wasLeft=TRUE;
				continue;
			}
			if( prevAxis==axis && prevSplit==split ) 
			{			
				stats.updateLeaf(depth,right-left+1);
				createNode(tempTree,nodeIndex,left,right);
				return;
			}
			gridBox[2*axis+1]=split;
			prevClip=MathUtil::NotANumber;
		} 
		else if(left>right)
		{			
			right=rightOrig;
			if(clipR>=split) 
			{				
				gridBox[2*axis+0]=split;
				prevClip=clipR;
				wasLeft=FALSE;
				continue;
			}
			if( prevAxis==axis && prevSplit==split )
			{				
				stats.updateLeaf(depth,right-left+1);
				createNode(tempTree,nodeIndex,left,right);
				return;
			}
			gridBox[2*axis+0]=split;
			prevClip=MathUtil::NotANumber;
		} 
		else 
		{			
			if(prevAxis!=-1 && boost::math::isnan(prevClip)) 
			{				
				int nextIndex=tempTree.getSize();				
				tempTree.add(0);
				tempTree.add(0);
				tempTree.add(0);
				if(wasLeft)
				{				
					stats.updateInner();
					tempTree.set(nodeIndex+0,(prevAxis<<30)|nextIndex);
					tempTree.set(nodeIndex+1,MathUtil::floatToRawIntBits(prevClip));
					tempTree.set(nodeIndex+2,MathUtil::floatToRawIntBits(FLT_MAX));
				}
				else 
				{					
					stats.updateInner();
					tempTree.set(nodeIndex+0,(prevAxis<<30)|(nextIndex-3));
					tempTree.set(nodeIndex+1,MathUtil::floatToRawIntBits(-FLT_MAX));
					tempTree.set(nodeIndex+2,MathUtil::floatToRawIntBits(prevClip));
				}				
				depth++;
				stats.updateLeaf(depth,0);				
				nodeIndex=nextIndex;
			}
			break;
		}
	}	
	int nextIndex=tempTree.getSize();	
	int nl=right-left+1;
	int nr=rightOrig-(right+1)+1;
	if(nl>0)
	{
		tempTree.add(0);
		tempTree.add(0);
		tempTree.add(0);
	} 
	else
		nextIndex-=3;	
	if(nr>0) 
	{
		tempTree.add(0);
		tempTree.add(0);
		tempTree.add(0);
	}
	stats.updateInner();
	tempTree.set(nodeIndex+0,(axis<<30)|nextIndex);
	tempTree.set(nodeIndex+1,MathUtil::floatToRawIntBits(clipL));
	tempTree.set(nodeIndex+2,MathUtil::floatToRawIntBits(clipR));
	
	vector<float> gridBoxL,gridBoxR,nodeBoxL,nodeBoxR;
	gridBoxL.resize(6);
	gridBoxR.resize(6);
	nodeBoxL.resize(6);
	nodeBoxR.resize(6);
	for(int i=0; i<6; i++) 
	{
		gridBoxL[i]=gridBoxR[i]=gridBox[i];
		nodeBoxL[i]=nodeBoxR[i]=nodeBox[i];
	}
	gridBoxL[2*axis+1]=gridBoxR[2*axis]=split;
	nodeBoxL[2*axis+1]=clipL;
	nodeBoxR[2*axis+0]=clipR;
	
	gridBox=nodeBox;	
	if(nl>0)
		subdivide(left,right,tempTree,indices,gridBoxL,nodeBoxL,
		nextIndex,depth+1,stats);
	else
		stats.updateLeaf(depth+1,0);
	if(nr>0)
		subdivide(right+1,rightOrig,tempTree,indices,gridBoxR, 
		nodeBoxR,nextIndex+3,depth+1,stats);
	else
		stats.updateLeaf(depth+1,0);
}

void BoundingIntervalHierarchy::intersect(Ray&r,IntersectionState&state)const
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

	unsigned int aa;
	aa=MathUtil::floatToRawIntBits(dirX);
	int offsetXFront=aa>>31;
	aa=MathUtil::floatToRawIntBits(dirY);
	int offsetYFront=aa>>31;
	aa=MathUtil::floatToRawIntBits(dirZ);
	int offsetZFront=aa>>31;

	int offsetXBack=offsetXFront^1;
	int offsetYBack=offsetYFront^1;
	int offsetZBack=offsetZFront^1;

	int offsetXFront3=offsetXFront*3;
	int offsetYFront3=offsetYFront*3;
	int offsetZFront3=offsetZFront*3;

	int offsetXBack3=offsetXBack*3;
	int offsetYBack3=offsetYBack*3;
	int offsetZBack3=offsetZBack*3;
	
	offsetXFront++;
	offsetYFront++;
	offsetZFront++;
	offsetXBack++;
	offsetYBack++;
	offsetZBack++;

	vector<IntersectionState::StackNode>& stack=state.getStack();
	int stackPos=0;
	int node=0;
	while(TRUE)
	{
		int tn=tree[node];
		int axis=tn&(7<<29);
		int offset=tn&~(7<<29);
		switch(axis) 
		{
		case 0: 
			{						
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetXFront])-orgX)*invDirX;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetXBack])-orgX)*invDirX;

				if( tf<intervalMin && tb>intervalMax )
					break;
				int back=offset+offsetXBack3;
				node=back;

				if(tf<intervalMin) 
				{
					intervalMin=(tb>=intervalMin)?tb:intervalMin;
					continue;
				}
				node=offset+offsetXFront3;

				if(tb>intervalMax) 
				{
					intervalMax=(tf<=intervalMax)?tf:intervalMax;
					continue;
				}						
				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(tb>=intervalMin)?tb:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;						
				intervalMax=(tf<=intervalMax)?tf:intervalMax;
				continue;
			}
		case 1<<30: 
			{
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetYFront])-orgY)*invDirY;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetYBack])-orgY)*invDirY;						
				if( tf<intervalMin && tb>intervalMax )
					break;
				int back=offset+offsetYBack3;
				node=back;					
				if(tf<intervalMin) 
				{
					intervalMin=(tb>=intervalMin)?tb:intervalMin;
					continue;
				}
				node=offset+offsetYFront3;

				if(tb>intervalMax) 
				{
					intervalMax=(tf<=intervalMax)?tf:intervalMax;
					continue;
				}					
				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(tb>=intervalMin)?tb:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;

				intervalMax=(tf<=intervalMax)?tf:intervalMax;
				continue;
			}
		case 2<<30: 
			{					
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetZFront])-orgZ)*invDirZ;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetZBack])-orgZ)*invDirZ;

				if( tf<intervalMin && tb>intervalMax )
					break;
				int back=offset+offsetZBack3;
				node=back;						
				if(tf<intervalMin)
				{
					intervalMin=(tb>=intervalMin)?tb:intervalMin;
					continue;
				}
				node=offset+offsetZFront3;

				if(tb>intervalMax)
				{
					intervalMax=(tf<=intervalMax)?tf:intervalMax;
					continue;
				}						
				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(tb>=intervalMin)?tb:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;						
				intervalMax=(tf<=intervalMax)?tf:intervalMax;
				continue;
			}
		case 3<<30: 
			{						
				int n=tree[node+1];
				while(n>0)
				{
					primitives->intersectPrimitive(r,objects[offset],state);
					n--;
					offset++;
				}
				break;
			}
		case 1<<29: 
			{
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetXFront])-orgX)*invDirX;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetXBack])-orgX)*invDirX;
				node=offset;
				intervalMin=(tf>=intervalMin)?tf:intervalMin;
				intervalMax=(tb<=intervalMax)?tb:intervalMax;
				if(intervalMin>intervalMax)
					break;
				continue;
			}
		case 3<<29: 
			{
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetYFront])-orgY)*invDirY;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetYBack])-orgY)*invDirY;
				node=offset;
				intervalMin=(tf>=intervalMin)?tf:intervalMin;
				intervalMax=(tb<=intervalMax)?tb:intervalMax;
				if(intervalMin>intervalMax)
					break;
				continue;
			}
		case 5<<29: 
			{
				float tf=(MathUtil::intBitsToFloat(tree[node+offsetZFront])-orgZ)*invDirZ;
				float tb=(MathUtil::intBitsToFloat(tree[node+offsetZBack])-orgZ)*invDirZ;
				node=offset;
				intervalMin=(tf>=intervalMin)?tf:intervalMin;
				intervalMax=(tb<=intervalMax)?tb:intervalMax;
				if(intervalMin>intervalMax)
					break;
				continue;
			}
		default:
			return;
		} 
		do 
		{			 
			if(stackPos==0)
				return;			 
			stackPos--;
			intervalMin=stack[stackPos].m_near;
			if(r.getMax()<intervalMin)
				continue;
			node=stack[stackPos].m_node;
			intervalMax=stack[stackPos].m_far;
			break;
		} while(TRUE);
	}
}