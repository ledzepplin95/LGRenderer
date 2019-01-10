#include "StdAfx.h"

#include "CausticPhotonMap.h"
#include "LGdef.h"
#include "timer.h"
#include "ShadingState.h"
#include "strutil.h"
#include "options.h"

LG_IMPLEMENT_DYNCREATE(CausticPhotonMap,CausticPhotonMapInterface)

CausticPhotonMap::Photon::Photon(const Point3&p,const Vector3&d,const Color&c)
{
	x=p.x;
	y=p.y;
	z=p.z;
	dir=d.encode();
	power=c.toRGBE();
	flags=SPLIT_X;
}

void CausticPhotonMap::Photon::setSplitAxis(int axis) 
{
	flags&=~SPLIT_MASK;
	flags|=axis;
}

float CausticPhotonMap::Photon::getCoord(int axis) const
{
	switch(axis) 
	{
	case SPLIT_X:
		return x;
	case SPLIT_Y:
		return y;
	default:
		break;
	}

	return z;
}

float CausticPhotonMap::Photon::getDist1(float px,float py,float pz)const
{
	switch(flags&SPLIT_MASK) 
	{
	case SPLIT_X:
		return px-x;
	case SPLIT_Y:
		return py-y;
	default:
		break;
	}

	return pz-z;
}

float CausticPhotonMap::Photon::getDist2(float px,float py,float pz) const
{
	float dx=x-px;
	float dy=y-py;
	float dz=z-pz;

	return (dx*dx)+(dy*dy)+(dz*dz);
}

CausticPhotonMap::Photon& CausticPhotonMap::Photon::operator=(const Photon&p)
{
	if(this==&p) return *this;

	x=p.x;
	y=p.y;
	z=p.z;
	dir=p.dir;
	power=p.power;
	flags=p.flags;		

	return *this;
}

CausticPhotonMap::NearestPhotons::NearestPhotons(const Point3&p,int n,float maxDist2) 
{
	_max=n;
	found=0;
	_gotHeap=FALSE;
	px=p.x;
	py=p.y;
	pz=p.z;
	dist2.resize(n+1);
	index.resize(n+1);
	dist2[0]=maxDist2;
}

void CausticPhotonMap::NearestPhotons::reset(const Point3&p,float maxDist2) 
{
	found=0;
	_gotHeap=FALSE;
	px=p.x;
	py=p.y;
	pz=p.z;
	dist2[0]=maxDist2;
}

void CausticPhotonMap::NearestPhotons::checkAddNearest(const Photon&p) 
{
	float fdist2=p.getDist2(px,py,pz);
	if(fdist2<dist2[0]) 
	{
		if(found<_max) 
		{
			found++;
			dist2[found]=fdist2;
			index[found]=p;
		} 
		else 
		{
			int j;
			int parent;
			if(!_gotHeap)
			{
				float dst2;
				Photon phot;
				int halfFound=found>>1;
				for(int k=halfFound; k>=1; k--) 
				{
					parent=k;
					phot=index[k];
					dst2=dist2[k];
					while(parent<=halfFound)
					{
						j=parent+parent;
						if( (j<found) && (dist2[j]<dist2[j+1]) )
							j++;
						if(dst2>=dist2[j])
							break;
						dist2[parent]=dist2[j];
						index[parent]=index[j];
						parent=j;
					}
					dist2[parent]=dst2;
					index[parent]=phot;
				}
				_gotHeap=TRUE;
			}
			parent=1;
			j=2;
			while(j<=found) 
			{
				if( (j<found) && (dist2[j]<dist2[j+1]) )
					j++;
				if(fdist2>dist2[j])
					break;
				dist2[parent]=dist2[j];
				index[parent]=index[j];
				parent=j;
				j+=j;
			}
			dist2[parent]=fdist2;
			index[parent]=p;
			dist2[0]=dist2[1];
		}
	}
}


CausticPhotonMap::NearestPhotons& 
    CausticPhotonMap::NearestPhotons::operator=(const NearestPhotons&np)
{
	if(this==&np) return *this;

	_max=np._max;
	_gotHeap=np._gotHeap;
	dist2=np.dist2;
	index=np.index;
	found=np.found;
	px=np.px;
	py=np.py;
	pz=np.pz;

	return *this;
}

void CausticPhotonMap::prepare(Options*options,const BoundingBox&sceneBounds)
{	
	nEmit=options->getInt("caustics.emit",10000);
	gatherNum=options->getInt("caustics.gather",50);
	gatherRadius=options->getFloat("caustics.radius",0.5f);
	filterValue=options->getFloat("caustics.filter",1.1f);	
	
	photonList.push_back(Photon());
	maxPower=0.0f;
	maxRadius=0.0f;	
	storedPhotons=halfStoredPhotons=0;
	log2n=0;
}

void CausticPhotonMap::locatePhotons(NearestPhotons&np)const
{
	vector<float> dist1d2;
	dist1d2.resize(log2n);
	vector<int> chosen;
	chosen.resize(log2n);
	int i=1;
	int level=0;
	int cameFrom;
	while(TRUE)
	{
		while(i<halfStoredPhotons)
		{
			float dist1d=photons[i].getDist1(np.px,np.py,np.pz);
			dist1d2[level]=dist1d*dist1d;
			i+=i;
			if(dist1d>0.0f)
				i++;
			chosen[level++]=i;
		}
		np.checkAddNearest(photons[i]);
		do 
		{
			cameFrom=i;
			i>>=1;
			level--;
			if(i==0)
				return;
		} 
		while( (dist1d2[level]>=np.dist2[0])
			|| (cameFrom!=chosen[level]) );
		np.checkAddNearest(photons[i]);
		i=chosen[level++]^1;
	}
}

void CausticPhotonMap::balance()
{
	if(storedPhotons==0)
		return;
	photons.resize(photonList.size());
	photonList.clear();
	vector<Photon> temp;
	temp.resize(storedPhotons+1);
	balanceSegment(temp,1,1,storedPhotons);
	photons=temp;
	halfStoredPhotons=storedPhotons/2;
	log2n=(int)ceil(log((double)storedPhotons)/log(2.0));
}

void CausticPhotonMap::balanceSegment(vector<Photon>&temp,int index,int start,int end) 
{
	int median=1;
	while( (4*median)<=(end-start+1) )
		median+=median;
	if( (3*median)<=(end-start+1) ) 
	{
		median+=median;
		median+=(start-1);
	} 
	else
		median=end-median+1;
	int axis=Photon::SPLIT_Z;
	Vector3 extents=bounds.getExtents();
	if( (extents.x>extents.y) && (extents.x>extents.z) )
		axis=Photon::SPLIT_X;
	else if(extents.y>extents.z)
		axis=Photon::SPLIT_Y;
	int left=start;
	int right=end;
	while(right>left) 
	{
		double v=photons[right].getCoord(axis);
		int i=left-1;
		int j=right;
		while(TRUE)
		{
			while(photons[++i].getCoord(axis)<v) 
			{
			}
			while( (photons[--j].getCoord(axis)>v) && (j>left) )
			{
			}
			if(i>=j) break;
			swap(i,j);
		}
		swap(i,right);
		if(i>=median)
			right=i-1;
		if(i<=median)
			left=i+1;
	}
	temp[index]=photons[median];
	temp[index].setSplitAxis(axis);
	if(median>start) 
	{
		if(start<(median-1))
		{
			float tmp;
			switch(axis) 
			{
			case Photon::SPLIT_X:
				tmp=bounds.getMaximum().x;
				bounds.Maximum().x=temp[index].x;
				balanceSegment(temp,2*index,start,median-1);
				bounds.Maximum().x=tmp;
				break;
			case Photon::SPLIT_Y:
				tmp=bounds.getMaximum().y;
				bounds.Maximum().y=temp[index].y;
				balanceSegment(temp,2*index,start,median-1);
				bounds.Maximum().y=tmp;
				break;
			default:
				tmp=bounds.getMaximum().z;
				bounds.Maximum().z=temp[index].z;
				balanceSegment(temp,2*index,start,median-1);
				bounds.Maximum().z=tmp;
			}
		} 
		else
			temp[2*index]=photons[start];
	}
	if(median<end) 
	{
		if( (median+1)<end )
		{
			float tmp;
			switch(axis) 
			{
			case Photon::SPLIT_X:
				tmp=bounds.getMinimum().x;
				bounds.Minimum().x=temp[index].x;
				balanceSegment(temp,(2*index)+1,median+1,end);
				bounds.Minimum().x=tmp;
				break;
			case Photon::SPLIT_Y:
				tmp=bounds.getMinimum().y;
				bounds.Minimum().y=temp[index].y;
				balanceSegment(temp,(2*index)+1,median+1,end);
				bounds.Minimum().y=tmp;
				break;
			default:
				tmp=bounds.getMinimum().z;
				bounds.Minimum().z=temp[index].z;
				balanceSegment(temp,(2*index)+1,median+1,end);
				bounds.Minimum().z=tmp;
			}
		} 
		else
			temp[(2*index)+1]=photons[end];
	}
}

void CausticPhotonMap::swap(int i,int j)
{
	Photon tmp=photons[i];
	photons[i]=photons[j];
	photons[j]=tmp;
}

void CausticPhotonMap::store(ShadingState&state,const Vector3&dir,
							 const Color&power,const Color&diffuse)
{
	
	if(( (state.getDiffuseDepth()==0) 
		&& (state.getReflectionDepth()>0
		|| state.getRefractionDepth()>0)
	  )) 
	{	
		Photon p(state.getPoint(),dir,power);

	    cs.Lock();
		storedPhotons++;	
		photonList.push_back(p);
		bounds.include(Point3(p.x,p.y,p.z));
		maxPower=max(maxPower,power.getMax());	
		cs.Unlock();
	}
}

void CausticPhotonMap::init() 
{
	StrUtil::PrintPrompt("平衡caustics photon map...");
	Timer t;
	t.start();
	balance();
	t.end();
	StrUtil::PrintPrompt("焦散光子贴图:");
	StrUtil::PrintPrompt("  * 已存储光子:   %d",storedPhotons);
	StrUtil::PrintPrompt("  * 光子/估计:    %d",gatherNum);
	maxRadius=1.4f*(float)sqrt(maxPower*gatherNum);
	StrUtil::PrintPrompt("  * 估计半径:     %.3f",gatherRadius);
	StrUtil::PrintPrompt("  * 最大半径:     %.3f",maxRadius);
	StrUtil::PrintPrompt("  * 平衡时间:     %s",t.toString());
	if(gatherRadius>maxRadius)
		gatherRadius=maxRadius;
}

void CausticPhotonMap::getSamples(ShadingState&state) const
{
	if(storedPhotons==0)
		return;
	NearestPhotons np(state.getPoint(),gatherNum,gatherRadius*gatherRadius);
	locatePhotons(np);
	if(np.found<8) return;
	Point3 pPos;
	Vector3 pDir;
	Vector3 pVec;
	float invArea=1.0f/((float)LG_PI*np.dist2[0]);
	float maxNDist=np.dist2[0]*0.05f;
	float f2r2=1.0f/(filterValue*filterValue*np.dist2[0]);
	float fInv=1.0f/(1.0f-2.0f/(3.0f*filterValue));
	for(int i=1; i<=np.found; i++) 
	{
		const Photon& pHot=np.index[i];
		Vector3::decode(pHot.dir,pDir);
		float cos=-Vector3::dot(pDir,state.getNormal());
		if(cos>0.001f) 
		{
			pPos.set(pHot.x,pHot.y,pHot.z);
			Point3::sub(pPos,state.getPoint(),pVec);
			float pcos=Vector3::dot(pVec,state.getNormal());
			if( (pcos<maxNDist) && (pcos>-maxNDist) ) 
			{
				LightSample* sample=new LightSample();
				sample->setShadowRay(Ray(state.getPoint(),pDir.negate()));
				sample->setRadiance(Color().setRGBE(np.index[i].power).mul(invArea/cos),
					Color::BLACK);
				sample->getDiffuseRadiance().mul((1.0f-(float)sqrt(np.dist2[i]*f2r2))*fInv);
				state.addSample(sample);
			}
		}
	}
}

BOOL CausticPhotonMap::allowDiffuseBounced() const
{
	return FALSE;
}

BOOL CausticPhotonMap::allowReflectionBounced() const
{
	return TRUE;
}

BOOL CausticPhotonMap::allowRefractionBounced() const
{
	return TRUE;
}

int CausticPhotonMap::numEmit() const
{
	return nEmit;
}