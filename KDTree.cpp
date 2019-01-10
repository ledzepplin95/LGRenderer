#include "StdAfx.h"

#include "KDTree.h"
#include "strutil.h"
#include "mathutil.h"
#include "memory.h"
#include "timer.h"
#include "IntersectionState.h"
#include "primitivelist.h"
#include "color.h"
#include "ray.h"
#include "matrix4.h"

float KDTree::INTERSECT_COST=0.5f;
float KDTree::TRAVERSAL_COST=1.0f;
float KDTree::EMPTY_BONUS=0.2f;
int KDTree::MAX_DEPTH=64;
BOOL KDTree::dump=FALSE;
CString KDTree::dumpPrefix="kdtree";

static unsigned __int64 CLOSED=(unsigned __int64)0L<<30;
static unsigned __int64 PLANAR=(unsigned __int64)1L<<30;
static unsigned __int64 OPENED=(unsigned __int64)2L<<30;
static unsigned __int64 TYPE_MASK=(unsigned __int64)3L<<30;

LG_IMPLEMENT_DYNCREATE(KDTree,AccelerationStructure)

KDTree::BuildStats& KDTree::BuildStats::operator =(const KDTree::BuildStats &stat)
{	
	if(this==&stat) return *this;

	numNodes=stat.numNodes;
	numLeaves=stat.numLeaves;
	sumObjects=stat.sumObjects;
	minObjects=stat.minObjects;
	maxObjects=stat.maxObjects;
	numLeaves0=stat.numLeaves0;
	numLeaves1=stat.numLeaves1;
	numLeaves2=stat.numLeaves2;
	numLeaves3=stat.numLeaves3;
	numLeaves4=stat.numLeaves4;
    numLeaves4p=stat.numLeaves4p;

	return *this;
}

KDTree::BuildStats::BuildStats() 
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
}

void KDTree::BuildStats::updateInner() 
{
	numNodes++;
}

void KDTree::BuildStats::updateLeaf(int depth,int n) 
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

void KDTree::BuildStats::printStats() const
{
	StrUtil::PrintPrompt("KDTree统计:");
	StrUtil::PrintPrompt("  * 结点:            %d",numNodes);
	StrUtil::PrintPrompt("  * 叶子:            %d",numLeaves);
	StrUtil::PrintPrompt("  * 对象: 最小值     %d",minObjects);
	StrUtil::PrintPrompt("             平均    %.2f",(float)sumObjects/numLeaves);
	StrUtil::PrintPrompt("           平均(n>0) %.2f",(float)sumObjects/(numLeaves-numLeaves0));
	StrUtil::PrintPrompt("             最大值  %d",maxObjects);
	StrUtil::PrintPrompt("  * 深度:   最小值   %d",minDepth);
	StrUtil::PrintPrompt("             平均    %.2f",(float)sumDepth/numLeaves);
	StrUtil::PrintPrompt("             最大值  %d", maxDepth);
	StrUtil::PrintPrompt("  * 叶子 w/:   N=0   %3d%%",100*numLeaves0/numLeaves);
	StrUtil::PrintPrompt("               N=1   %3d%%",100*numLeaves1/numLeaves);
	StrUtil::PrintPrompt("               N=2   %3d%%",100*numLeaves2/numLeaves);
	StrUtil::PrintPrompt("               N=3   %3d%%",100*numLeaves3/numLeaves);
	StrUtil::PrintPrompt("               N=4   %3d%%",100*numLeaves4/numLeaves);
	StrUtil::PrintPrompt("               N>4   %3d%%",100*numLeaves4p/numLeaves);
}

KDTree::BuildTask::BuildTask(int num) 
{
	splits.resize(6*num);
	numObjects=num;
	n=0;			
	leftRightTable.resize((num+3)/4);
}

KDTree::BuildTask::BuildTask(int num,const BuildTask&parent) 
{
	splits.resize(6*num);
	numObjects=num;
	n=0;
	leftRightTable=parent.leftRightTable;
}

KDTree::BuildTask& KDTree::BuildTask::operator=(const BuildTask&task)
{
    if(this==&task) return *this;

    splits=task.splits;
	numObjects=task.numObjects;
	n=task.n;
	leftRightTable=task.leftRightTable;

	return *this;
}

KDTree::KDTree(void)
{
	maxPrims=0;
	primitiveList=NULL;
}

KDTree::KDTree(int mp) 
{
	maxPrims=mp;
}

void KDTree::setDumpMode(BOOL d,CString prefix) 
{
	dump=d;
	dumpPrefix=prefix;
}

void KDTree::build(PrimitiveList*p) 
{
	StrUtil::PrintPrompt("KDTree设置");
	StrUtil::PrintPrompt("  * 最大叶子大小:  %d",maxPrims);
	StrUtil::PrintPrompt("  * 最大深度:      %d",MAX_DEPTH);
	StrUtil::PrintPrompt("  * 遍历成本:      %.2f",TRAVERSAL_COST);
	StrUtil::PrintPrompt("  * 相交成本:      %.2f",INTERSECT_COST);
	StrUtil::PrintPrompt("  * 零红利:        %.2f",EMPTY_BONUS);
	StrUtil::PrintPrompt("  * 叶转储:        %s",dump?"允许":"不允许");
	Timer total;
	total.start();
	primitiveList=p;	
	bounds=p->getWorldBounds(Matrix4());
	int nPrim=primitiveList->getNumPrimitives(),nSplits=0;
	BuildTask task(nPrim);
	Timer prepare;
	prepare.start(); 
	for(int i=0; i<nPrim; i++) 
	{
		for(int axis=0; axis<3; axis++) 
		{
			float ls=primitiveList->getPrimitiveBound(i,2*axis+0);
			float rs=primitiveList->getPrimitiveBound(i,2*axis+1);
			if(ls==rs)
			{				
				task.splits[nSplits]=pack(ls,PLANAR,axis,i);
				nSplits++;
			} 
			else
			{
				task.splits[nSplits+0]=pack(ls,OPENED,axis,i);
				task.splits[nSplits+1]=pack(rs,CLOSED,axis,i);
				nSplits+=2;
			}
		}
	}
	task.n=nSplits;
	prepare.end();
	Timer t;
	vector<int> tempTree;
	vector<int> tempList;
	tempTree.push_back(0);
	tempTree.push_back(1);
	t.start();	
	Timer sorting;
	sorting.start();
	radix12(task.splits,task.n);
	sorting.end();	
	BuildStats stats;
	buildTree(bounds.getMinimum().x,bounds.getMaximum().x,bounds.getMinimum().y, 
		bounds.getMaximum().y,bounds.getMinimum().z,bounds.getMaximum().z,task,1, 
		tempTree,0,tempList,stats);
	t.end();
	tree=tempTree;	
	primitives=tempList;	
	total.end();	
	stats.printStats();
	StrUtil::PrintPrompt("  * 结点内存:    %s",Memory::m_sizeof(tree));
	StrUtil::PrintPrompt("  * 对象内存:    %s",Memory::m_sizeof(primitives));
	StrUtil::PrintPrompt("  * 预备时间:    %s",prepare.toString() );
	StrUtil::PrintPrompt("  * 排序时间:    %s",sorting.toString());
	StrUtil::PrintPrompt("  * 创建树:      %s",t.toString());
	StrUtil::PrintPrompt("  * 建造时间:    %s",total.toString());
	if(dump) 
	{
		try 
		{
			StrUtil::PrintPrompt("转储材质为%s.mtl...",dumpPrefix);
			CFile mtlFile;
			CString fileName=dumpPrefix+".mtl";
			if( !mtlFile.Open(fileName,CFile::modeWrite | CFile::typeText) )
				return;
			int maxN=stats.maxObjects;
			for(int n=0; n<=maxN; n++)
			{
				float blend=(float)n/(float)maxN;
				Color nc;
				if(blend<0.25f)
					nc=Color::blend(Color::BLUE,Color::GREEN,blend/0.25f);
				else if(blend<0.5f)
					nc=Color::blend(Color::GREEN,Color::YELLOW,(blend-0.25f)/0.25f);
				else if(blend<0.75f)
					nc=Color::blend(Color::YELLOW,Color::RED,(blend-0.50f)/0.25f);
				else
					nc=Color::MAGENTA;
				CString str;
				str.Format(_T("newmtl mtl%d\n"),n);
				mtlFile.Write(str,str.GetLength());
				vector<float> rgb=nc.getRGB();
				str="Ka 0.1 0.1 0.1\n";
				mtlFile.Write(str,str.GetLength());
                str.Format("Kd %.12g %.12g %.12g\n",rgb[0],rgb[1],rgb[2]);
				mtlFile.Write(str,str.GetLength());
				str="illum 1\n\n";
				mtlFile.Write(str,str.GetLength());
			}
			CFile objFile;
			fileName=dumpPrefix+".obj";
			if( !objFile.Open(fileName,CFile::modeWrite | CFile::typeText) )
				return;
			StrUtil::PrintPrompt("转储树为%s.obj...",dumpPrefix);
			dumpObj(0,0,maxN,BoundingBox(bounds),objFile,mtlFile);
			objFile.Close();
			mtlFile.Close();
		} 
		catch(...) 
		{			
		}
	}
}

int KDTree::dumpObj(int offset,int vertOffset,int maxN,BoundingBox&bounds, 
					CFile&file,CFile&mtlFile) 
{
	CString str;
	if(offset==0)
	{
		str.Format(_T("mtllib %s.mtl\n"),dumpPrefix);
		file.Write(str,str.GetLength());
	}
	int nextOffset=tree[offset];
	if( (nextOffset&(3<<30))==(3<<30) ) 
	{	
		int n=tree[offset+1];
		if(n>0) 
		{			
			Point3 min=bounds.getMinimum();
			Point3 max=bounds.getMaximum();
			str.Format(_T("o node%d\n"),offset);
			file.Write(str,str.GetLength());
            str.Format(_T("v %g %g %g\n"),max.x,max.y,min.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),max.x,min.y,min.z);
		    file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),min.x,min.y,min.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),min.x,max.y,min.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),max.x,max.y,max.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),max.x,min.y,max.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),min.x,min.y,max.z);
			file.Write(str,str.GetLength());
			str.Format(_T("v %g %g %g\n"),min.x,max.y,max.z);
			file.Write(str,str.GetLength());
			int v0=vertOffset;
			str.Format(_T("usemtl mtl%d\n"),n);
			file.Write(str,str.GetLength());
			str="s off\n";
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+1,v0+2,v0+3,v0+4);
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+5,v0+8,v0+7,v0+6);
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+1,v0+5,v0+6,v0+2);
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+2,v0+6,v0+7,v0+3);
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+3,v0+7,v0+8,v0+4);
			file.Write(str,str.GetLength());
			str.Format(_T("f %d %d %d %d\n"),v0+5,v0+1,v0+4,v0+8);
			file.Write(str,str.GetLength());
			vertOffset+=8;
		}

		return vertOffset;
	}
	else 
	{		
		int axis=nextOffset&(3<<30),v0;
		float split=MathUtil::intBitsToFloat(tree[offset+1]),min,max;
		nextOffset&=~(3<<30);
		switch(axis) 
		{
		case 0:
			max=bounds.getMaximum().x;
			bounds.Maximum().x=split;
			v0=dumpObj(nextOffset,vertOffset,maxN,bounds,file,mtlFile);					
			bounds.Maximum().x=max;
			min=bounds.getMinimum().x;
			bounds.Minimum().x=split;
			v0=dumpObj(nextOffset+2,v0,maxN,bounds,file,mtlFile);
			bounds.Minimum().x=min;
			break;
		case 1<<30:
			max=bounds.getMaximum().y;
			bounds.Maximum().y=split;
			v0=dumpObj(nextOffset,vertOffset,maxN,bounds,file,mtlFile);					
			bounds.Maximum().y=max;
			min=bounds.getMinimum().y;
			bounds.Minimum().y=split;
			v0=dumpObj(nextOffset+2,v0,maxN,bounds,file,mtlFile);
			bounds.Minimum().y=min;
			break;
		case 2<<30:
			max=bounds.getMaximum().z;
			bounds.Maximum().z=split;
			v0=dumpObj(nextOffset,vertOffset,maxN,bounds,file,mtlFile);				
			bounds.Maximum().z=max;
			min=bounds.getMinimum().z;
			bounds.Minimum().z=split;
			v0=dumpObj(nextOffset+2,v0,maxN,bounds,file,mtlFile);				
			bounds.Minimum().z=min;
			break;
		default:
			v0=vertOffset;
			break;
		}

		return v0;
	}
}

LONG64 KDTree::pack(float split,unsigned long t,int axis,int object) 
{	
	int f=MathUtil::floatToRawIntBits(split);
	int top=f^((f>>31)|0x80000000);
	LONG64 p=((LONG64)top&0xFFFFFFFFL)<<32;
	p|=(LONG64)t;
	p|=((LONG64)axis)<<28;
	p|=((LONG64)object&0xFFFFFFFL);

	return p;
}

int KDTree::unpackObject(LONG64 p) 
{
	return (int)((unsigned __int64)p&0xFFFFFFFL);
}

int KDTree::unpackAxis(LONG64 p) 
{
	return (int)((unsigned __int64)p>>28)&3;
}

long KDTree::unpackSplitType(LONG64 p)
{
	return (unsigned __int64)p&TYPE_MASK;
}

float KDTree::unpackSplit(LONG64 p) 
{
	int f=(int)(((unsigned __int64)p>>32)&0xFFFFFFFFL);
	int m=(( (unsigned int)f>>31 )-1)|0x80000000;

	return MathUtil::intBitsToFloat(f^m);
}

void KDTree::radix12(vector<LONG64>&splits,int n) 
{	
	int hist[2048];
	for(int i=0; i<2048; i++)
		hist[i]=0;
	vector<LONG64> sorted;
	sorted.resize(n);
	unsigned __int64 aa;

	for(int i=0; i<n; i++) 
	{
		LONG64 pi=splits[i];
		aa=((unsigned __int64)(pi>>28)&0x1FF);
		hist[0x000+aa]++;
		aa=((unsigned __int64)(pi>>37)&0x1FF);
		hist[0x200+aa]++;
		aa=((unsigned __int64)(pi>>46)&0x1FF);
		hist[0x400+aa]++;
		aa=(unsigned __int64)pi>>55;
		hist[0x600+aa]++;
	}
	{
		int sum0=0,sum1=0,sum2=0,sum3=0;
		int tsum;
		for(int i=0; i<512; i++)
		{
			tsum=hist[0x000+i]+sum0;
			hist[0x000+i]=sum0-1;
			sum0=tsum;
			tsum=hist[0x200+i]+sum1;
			hist[0x200+i]=sum1-1;
			sum1=tsum;
			tsum=hist[0x400+i]+sum2;
			hist[0x400+i]=sum2-1;
			sum2=tsum;
			tsum=hist[0x600+i]+sum3;
			hist[0x600+i]=sum3-1;
			sum3=tsum;
		}
	}
	unsigned __int64 pos;
	for(int i=0; i<n; i++) 
	{
		LONG64 pi=splits[i];
		pos=(unsigned __int64)(pi>>28)&0x1FF;
		sorted[++hist[0x000+pos]]=pi;
	}
	for(int i=0; i<n; i++) 
	{
		LONG64 pi=sorted[i];
		pos=(unsigned __int64)(pi>>37)&0x1FF;
		splits[++hist[0x200+pos]]=pi;
	}
	for(int i=0; i<n; i++) 
	{
		LONG64 pi=splits[i];
		pos=(unsigned __int64)(pi>>46)&0x1FF;
		sorted[++hist[0x400+pos]]=pi;
	}
	for(int i=0; i<n; i++) 
	{
		LONG64 pi=sorted[i];
		pos=(unsigned __int64)pi>>55;
		splits[++hist[0x600+pos]]=pi;
	}	
}

void KDTree::buildTree(float minx,float maxx,float miny,float maxy,float minz,
	float maxz,const BuildTask&task,int depth,vector<int>&tempTree,int offset,
    vector<int>&tempList,BuildStats&stats) 
{	
	if( task.numObjects>maxPrims && depth<MAX_DEPTH ) 
	{
		float dx=maxx-minx;
		float dy=maxy-miny;
		float dz=maxz-minz;		
		float bestCost=INTERSECT_COST*task.numObjects;
		int bestAxis=-1;
		int bestOffsetStart=-1;
		int bestOffsetEnd=-1;
		float bestSplit=0.0f;
		BOOL bestPlanarLeft=FALSE;
		int bnl=0,bnr=0;	
		float area=dx*dy+dy*dz+dz*dx;
		float ISECT_COST=INTERSECT_COST/area;	
		vector<int> nl,nr;
		nl.push_back(0); 
		nl.push_back(0); 
		nl.push_back(0);
		nr.push_back(task.numObjects);
		nr.push_back(task.numObjects); 
		nr.push_back(task.numObjects);	
        vector<float> dp,ds,nodeMin,nodeMax;
		dp.push_back(dy*dz);
	    dp.push_back(dz*dx);
		dp.push_back(dx*dy);
		ds.push_back(dy+dz);
		ds.push_back(dz+dx);
		ds.push_back(dx+dy);
		nodeMin.push_back(minx);
		nodeMin.push_back(miny);
		nodeMin.push_back(minz);
		nodeMax.push_back(maxx);
		nodeMax.push_back(maxy); 
		nodeMax.push_back(maxz);	
		int nSplits=task.n;
		vector<LONG64> splits;
		vector<byte> lrtable;
		splits=task.splits;
		lrtable=task.leftRightTable;
		for(int i=0; i<nSplits;) 
		{		
			__int64 ptr=splits[i];
			float split=unpackSplit(ptr);
			int axis=unpackAxis(ptr);		
			int currentOffset=i;		
			int pClosed=0,pPlanar=0,pOpened=0;
			unsigned __int64 ptrMasked=(unsigned __int64)ptr&(~TYPE_MASK&0xFFFFFFFFF0000000L);
			unsigned __int64 ptrClosed=ptrMasked | CLOSED;
			unsigned __int64 ptrPlanar=ptrMasked | PLANAR;
			unsigned __int64 ptrOpened=ptrMasked | OPENED;
			while( i<nSplits && (splits[i]&0xFFFFFFFFF0000000L)==ptrClosed ) 
			{
				int obj=unpackObject(splits[i]);
				lrtable[obj>>2]=0;
				pClosed++;
				i++;
			}
			while( i<nSplits && (splits[i]&0xFFFFFFFFF0000000L)==ptrPlanar ) 
			{
				int obj=unpackObject(splits[i]);
				lrtable[obj>>2]=0;
				pPlanar++;
				i++;
			}
			while( i<nSplits && (splits[i]&0xFFFFFFFFF0000000L)==ptrOpened ) 
			{
				int obj=unpackObject(splits[i]);
				lrtable[obj>>2]=0;
				pOpened++;
				i++;
			}			
			nr[axis]-=pPlanar+pClosed;		
			if( split>=nodeMin[axis] && split<=nodeMax[axis] ) 
			{			
				float dl=split-nodeMin[axis];
				float dr=nodeMax[axis]-split;
				float lp=dp[axis]+dl*ds[axis];
				float rp=dp[axis]+dr*ds[axis];			
				BOOL planarLeft=dl<dr;
				int numLeft=nl[axis]+(planarLeft?pPlanar:0);
				int numRight=nr[axis]+(planarLeft?0:pPlanar);
				float eb=( (numLeft==0 && dl>0) || (numRight==0 && dr>0) )?EMPTY_BONUS:0;
				float cost=TRAVERSAL_COST+ISECT_COST*(1.0f-eb)*(lp*numLeft+rp*numRight);

				if(cost<bestCost)
				{
					bestCost=cost;
					bestAxis=axis;
					bestSplit=split;
					bestOffsetStart=currentOffset;
					bestOffsetEnd=i;
					bnl=numLeft;
					bnr=numRight;
					bestPlanarLeft=planarLeft;
				}
			}		
			nl[axis]+=pOpened+pPlanar;
		}	
		for(int axis=0; axis<3; axis++) 
		{
			int numLeft=nl[axis];
			int numRight=nr[axis];
			if( numLeft!=task.numObjects || numRight!=0 )
				StrUtil::PrintPrompt(
				"未全范围的扫描对象@深度等于%d，轴向剩余%d:[左:%d] [右:%d]",
				depth,axis,numLeft,numRight);
		}	
		if(bestAxis!=-1) 
		{		
			BuildTask taskL(bnl,task);
			BuildTask taskR(bnr,task);
			int lk=0,rk=0;
			for(int i=0; i<bestOffsetStart; i++) 
			{
				LONG64 ptr=splits[i];
				if(unpackAxis(ptr)==bestAxis)
				{
					if(unpackSplitType(ptr)!=CLOSED) 
					{
						int obj=unpackObject(ptr);
						lrtable[obj>>2]|=1<<((obj&3)<<1);
						lk++;
					}
				}
			}
			for(int i=bestOffsetStart; i<bestOffsetEnd; i++) 
			{
				LONG64 ptr=splits[i];
				ASSERT( unpackAxis(ptr)==bestAxis );
				if(unpackSplitType(ptr)==PLANAR) 
				{
					if(bestPlanarLeft) 
					{
						int obj=unpackObject(ptr);
						lrtable[obj>>2]|=1<<((obj&3)<<1);
						lk++;
					} 
					else 
					{
						int obj=unpackObject(ptr);
						lrtable[obj>>2]|=2<<((obj&3)<<1);
						rk++;
					}
				}
			}
			for(int i=bestOffsetEnd; i<nSplits; i++) 
			{
				LONG64 ptr=splits[i];
				if(unpackAxis(ptr)==bestAxis) 
				{
					if(unpackSplitType(ptr)!=OPENED)
					{
						int obj=unpackObject(ptr);
						lrtable[obj>>2]|=2<<((obj&3)<<1);
						rk++;
					}
				}
			}		
			vector<LONG64> splitsL=taskL.splits;
			vector<LONG64> splitsR=taskR.splits;
			int nsl=0,nsr=0;
			for(int i=0; i<nSplits; i++)
			{
				LONG64 ptr=splits[i];
				int obj=unpackObject(ptr);
				int idx=obj>>2;
				int mask=1<<((obj&3)<<1);
				if( (lrtable[idx]&mask)!=0 )
				{
					splitsL[nsl]=ptr;
					nsl++;
				}
				if( (lrtable[idx]&(mask<<1))!=0 ) 
				{
					splitsR[nsr]=ptr;
					nsr++;
				}
			}
			taskL.n=nsl;
			taskR.n=nsr;			
			int nextOffset=tempTree.size();
			tempTree.push_back(0);
			tempTree.push_back(0);
			tempTree.push_back(0);
			tempTree.push_back(0);		
			tempTree[offset+0]=(bestAxis<<30)|nextOffset;
			tempTree[offset+1]=MathUtil::floatToRawIntBits(bestSplit);		
			stats.updateInner();
			switch(bestAxis) 
			{
			case 0:
				buildTree(minx,bestSplit,miny,maxy,minz,maxz,taskL, 
					depth+1,tempTree,nextOffset,tempList,stats);						
				buildTree(bestSplit,maxx,miny,maxy,minz,maxz,taskR, 
					depth+1,tempTree,nextOffset+2,tempList,stats);							
				return;
			case 1:
				buildTree(minx,maxx,miny,bestSplit,minz,maxz,taskL,
					depth+1,tempTree,nextOffset,tempList,stats);						
				buildTree(minx,maxx,bestSplit,maxy,minz,maxz,taskR, 
					depth+1,tempTree,nextOffset+2,tempList,stats);						
				return;
			case 2:
				buildTree(minx,maxx,miny,maxy,minz,bestSplit,taskL,
					depth+1,tempTree,nextOffset,tempList,stats);						
				buildTree(minx,maxx,miny,maxy,bestSplit,maxz,taskR,
					depth+1,tempTree,nextOffset+2,tempList,stats);						
				return;
			default:
				ASSERT(FALSE);
			}
		}
	}
	int listOffset=tempList.size();
	int n=0;
	for(int i=0; i<task.n; i++) 
	{
		LONG64 ptr=task.splits[i];
		if( unpackAxis(ptr)==0 
			&& unpackSplitType(ptr)!=CLOSED ) 
		{
			tempList.push_back(unpackObject(ptr));
			n++;
		}
	}
	stats.updateLeaf(depth,n);
	if(n!=task.numObjects)
		StrUtil::PrintPrompt("创建叶结点错误，预期%d，找到%d", 
		task.numObjects,n);
	tempTree[offset+0]=(3<<30)|listOffset;
	tempTree[offset+1]=task.numObjects;		
}

void KDTree::intersect(Ray&r,IntersectionState&state) const
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
	aa=MathUtil::floatToRawIntBits(dirX)&(1<<31);
	int offsetXFront=aa>>30;
	aa=MathUtil::floatToRawIntBits(dirY)&(1<<31);
	int offsetYFront=aa>>30;
	aa=MathUtil::floatToRawIntBits(dirZ)&(1<<31);
	int offsetZFront=aa>>30;

	int offsetXBack=offsetXFront^2;
	int offsetYBack=offsetYFront^2;
	int offsetZBack=offsetZFront^2;

	vector<IntersectionState::StackNode>& stack=state.getStack();
	int stackPos=0;
	int node=0;

	while(TRUE) 
	{
		int tn=tree[node];
		int axis=tn&(3<<30);
		int offset=tn&~(3<<30);
		switch(axis) 
		{
		case 0: 
			{
				float d=(MathUtil::intBitsToFloat(
					tree[node+1])-orgX)*invDirX;
				int back=offset+offsetXBack;
				node=back;
				if(d<intervalMin)
					continue;
				node=offset+offsetXFront; 
				if(d>intervalMax)
					continue;

				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(d>=intervalMin)?d:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;					
				intervalMax=(d<=intervalMax)?d:intervalMax;
				continue;
			}
		case 1<<30: 
			{				
				float d=(MathUtil::intBitsToFloat(
					tree[node+1])-orgY)*invDirY;
				int back=offset+offsetYBack;
				node=back;
				if(d<intervalMin)
					continue;
				node=offset+offsetYFront; 
				if(d>intervalMax)
					continue;				
				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(d>=intervalMin)?d:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;					
				intervalMax=(d<=intervalMax)?d:intervalMax;
				continue;
			}
		case 2<<30:
			{					
				float d=(MathUtil::intBitsToFloat(
					tree[node+1])-orgZ)*invDirZ;
				int back=offset+offsetZBack;
				node=back;
				if(d<intervalMin)
					continue;
				node=offset+offsetZFront; 
				if(d>intervalMax)
					continue;					
				stack[stackPos].m_node=back;
				stack[stackPos].m_near=(d>=intervalMin)?d:intervalMin;
				stack[stackPos].m_far=intervalMax;
				stackPos++;					
				intervalMax=(d<=intervalMax)?d:intervalMax;
				continue;
			}
		default: 
			{					
				int n=tree[node+1];
				while(n>0) 
				{
					primitiveList->intersectPrimitive(r,primitives[offset],state);
					n--;
					offset++;
				}
				if(r.getMax()<intervalMax)
					return;
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
	} 
}