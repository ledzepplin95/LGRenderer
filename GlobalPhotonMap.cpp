#include "StdAfx.h"

#include "GlobalPhotonMap.h"
#include "StrUtil.h"
#include "ShadingState.h"
#include "Timer.h"
#include "LGDef.h"
#include "options.h"

LG_IMPLEMENT_DYNCREATE(GlobalPhotonMap,GlobalPhotonMapInterface)

GlobalPhotonMap::Photon::Photon(const Point3&p,const Vector3&n,const Vector3&d,
								const Color&c,const Color&diffuse) 
{
	x=p.x;
	y=p.y;
	z=p.z;
	dir=d.encode();
	power=c.toRGBE();
	flags=0;
	normal=n.encode();
	data=diffuse.toRGB();
}

void GlobalPhotonMap::Photon::setSplitAxis(int axis)
{
	flags&=~SPLIT_MASK;
	flags|=axis;
}

float GlobalPhotonMap::Photon::getCoord(int axis) const
{
	switch(axis)
	{
	case SPLIT_X:
		return x;
	case SPLIT_Y:
		return y;
	default:
		return z;
	}
}

float GlobalPhotonMap::Photon::getDist1(float px,float py,float pz) const
{
	switch(flags&SPLIT_MASK)
	{
	case SPLIT_X:
		return px-x;
	case SPLIT_Y:
		return py-y;
	default:
		return pz-z;
	}
}

float GlobalPhotonMap::Photon::getDist2(float px,float py,float pz) const
{
	float dx=x-px;
	float dy=y-py;
	float dz=z-pz;

	return (dx*dx)+(dy*dy)+(dz*dz);
}

GlobalPhotonMap::Photon& GlobalPhotonMap::Photon::operator=(const Photon&p)
{
	if(this==&p) return *this;

    x=p.x;
	y=p.y;
	z=p.z;
	dir=p.dir;
	normal=p.normal;
	data=p.data;
	power=p.power;
	flags=p.flags;

	return *this;
}


GlobalPhotonMap::NearestPhotons::NearestPhotons(const Point3&p,int n,float maxDist2) 
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

void GlobalPhotonMap::NearestPhotons::reset(const Point3&p,float maxDist2)
{
	found=0;
	_gotHeap=FALSE;
	px=p.x;
	py=p.y;
	pz=p.z;
	dist2[0]=maxDist2;
}

void GlobalPhotonMap::NearestPhotons::checkAddNearest(const Photon&p)
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
				for(int k=halfFound; k>= 1; k--) 
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

GlobalPhotonMap::NearestPhotons& 
    GlobalPhotonMap::NearestPhotons::operator=(const NearestPhotons&np)
{
	if(this==&np) return *this;

	_max=np._max;
	_gotHeap=np._gotHeap;
	found=np.found;
	px=np.px;
	py=np.py;
	pz=np.pz;
	dist2=np.dist2;
	index=np.index;

	return *this;
}

GlobalPhotonMap::GlobalPhotonMap() 
{
	hasRadiance=FALSE;
	maxPower=0.0f;
	maxRadius=0.0f;
}

void GlobalPhotonMap::prepare(Options*options,const BoundingBox&sceneBounds) 
{	
	nEmit=options->getInt("gi.irr-cache.gmap.emit",100000);
	numGather=options->getInt("gi.irr-cache.gmap.gather",50);
	gatherRadius=options->getFloat("gi.irr-cache.gmap.radius",0.5f);		
	storedPhotons=halfStoredPhotons=0;
}

void GlobalPhotonMap::store(ShadingState&state,const Vector3&dir,
							const Color&power,const Color&diffuse) 
{
	Photon p(state.getPoint(),state.getNormal(),dir,power,diffuse);

	cs.Lock();
	storedPhotons++;
	photonList.push_back(p);
	bounds.include(Point3(p.x,p.y,p.z));
	maxPower=max(maxPower,power.getMax());
	cs.Unlock();
}

void GlobalPhotonMap::locatePhotons(NearestPhotons&np) 
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

void GlobalPhotonMap::balance() 
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

void GlobalPhotonMap::balanceSegment(vector<Photon>&temp,int index,int start,int end) 
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
			if(i>=j)
				break;
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
		if((median+1)<end)
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

void GlobalPhotonMap::swap(int i,int j) 
{
	Photon tmp=photons[i];
	photons[i]=photons[j];
	photons[j]=tmp;
}

void GlobalPhotonMap::init() 
{
	StrUtil::PrintPrompt("平衡global photon map...");
	StrUtil::PrintPrompt("平衡global photon map");
	Timer t;
	t.start();
	balance();
	t.end();
	StrUtil::PrintPrompt("全局光子贴图:");
	StrUtil::PrintPrompt("  * 已存储光子:   %d",storedPhotons);
	StrUtil::PrintPrompt("  * 光子/估计:    %d",numGather);
	StrUtil::PrintPrompt("  * 估计半径:     %.3f",gatherRadius);
	maxRadius=1.4f*(float)sqrt(maxPower*numGather);
	StrUtil::PrintPrompt("  * 最大半径:     %.3f",maxRadius);
	StrUtil::PrintPrompt("  * 平衡时间:     %s",t.toString());
	if(gatherRadius>maxRadius)
		gatherRadius=maxRadius;
	t.start();
	precomputeRadiance();
	t.end();
	StrUtil::PrintPrompt("  * 预计算时间:   %s",t.toString());
	StrUtil::PrintPrompt("  * 辐射光子:     %d",storedPhotons);
	StrUtil::PrintPrompt("  * 查找半径:     %.3f",gatherRadius);
}

void GlobalPhotonMap::precomputeRadiance() 
{
	if(storedPhotons==0)
		return;	
	int quadStoredPhotons=halfStoredPhotons/2;
	Point3 p;
	Vector3 n;
	Point3 ppos;
	Vector3 pdir;
	Vector3 pvec;
	Color irr;
	Color pow;
	float maxDist2=gatherRadius*gatherRadius;
	NearestPhotons np(p,numGather,maxDist2);
	vector<Photon> temp;
	temp.resize(quadStoredPhotons+1);
	StrUtil::PrintPrompt("预计算辐射能");
	for(int i=1;i<=quadStoredPhotons; i++) 
	{		
		Photon curr=photons[i];
		p.set(curr.x,curr.y,curr.z);
		Vector3::decode(curr.normal,n);
		irr.set(Color::BLACK);
		np.reset(p,maxDist2);
		locatePhotons(np);
		if(np.found<8)
		{
			curr.data=0;
			temp[i]=curr;
			continue;
		}
		float invArea=1.0f/((float)LG_PI*np.dist2[0]);
		float maxNDist=np.dist2[0]*0.05f;
		for(int j=1; j<= np.found; j++)
		{
			Photon phot=np.index[j];
			Vector3::decode(phot.dir, pdir);
			float cos=-Vector3::dot(pdir,n);
			if(cos>0.01f) 
			{
				ppos.set(phot.x,phot.y,phot.z);
				Point3::sub(ppos,p,pvec);
				float pcos=Vector3::dot(pvec,n);
				if( (pcos<maxNDist) && (pcos>-maxNDist) )
					irr.add(pow.setRGBE(phot.power));
			}
		}
		irr.mul(invArea);		
		irr.mul(Color(curr.data)).mul(1.0f/(float)LG_PI);
		curr.data=irr.toRGBE();
		temp[i]=curr;
	}

	numGather/=4;
	maxRadius=1.4f*sqrt(maxPower*numGather);
	if(gatherRadius>maxRadius)
		gatherRadius=maxRadius;
	storedPhotons=quadStoredPhotons;
	halfStoredPhotons=storedPhotons/2;
	log2n=(int)ceil(log((double)storedPhotons)/log(2.0));
	photons=temp;
	hasRadiance=TRUE;
}

Color GlobalPhotonMap::getRadiance(const Point3&p,const Vector3&n)
{
	if( !hasRadiance || (storedPhotons==0) )
		return Color::BLACK;
	float px=p.x;
	float py=p.y;
	float pz=p.z;
	int i=1;
	int level=0;
	int cameFrom;
	float dist2;
	float maxDist2=gatherRadius*gatherRadius;
	Photon nearest;
	Photon curr;
	Vector3 photN;
	vector<float> dist1d2;
	dist1d2.resize(log2n);
	vector<int> chosen;
	chosen.resize(log2n);
	while(TRUE) 
	{
		while(i<halfStoredPhotons) 
		{
			float dist1d=photons[i].getDist1(px,py,pz);
			dist1d2[level]=dist1d*dist1d;
			i+=i;
			if(dist1d>0)
				i++;
			chosen[level++]=i;
		}
		curr=photons[i];
		dist2=curr.getDist2(px,py,pz);
		if(dist2<maxDist2)
		{
			Vector3::decode(curr.normal,photN);
			float currentDotN=Vector3::dot(photN,n);
			if(currentDotN>0.9f)
			{
				nearest=curr;
				maxDist2=dist2;
			}
		}
		do 
		{
			cameFrom=i;
			i>>=1;
			level--;
			if(i==0)
				return Color().setRGBE(nearest.data);
		} while( (dist1d2[level]>=maxDist2)
			|| (cameFrom!=chosen[level]) );
		curr=photons[i];
		dist2=curr.getDist2(px,py,pz);
		if(dist2<maxDist2) 
		{
			Vector3::decode(curr.normal,photN);
			float currentDotN=Vector3::dot(photN,n);
			if(currentDotN>0.9f) 
			{
				nearest=curr;
				maxDist2=dist2;
			}
		}
		i=chosen[level++]^1;
	}
}

BOOL GlobalPhotonMap::allowDiffuseBounced()const
{
	return TRUE;
}

BOOL GlobalPhotonMap::allowReflectionBounced() const
{
	return TRUE;
}

BOOL GlobalPhotonMap::allowRefractionBounced()const
{
	return TRUE;
}

int GlobalPhotonMap::numEmit() const
{
	return nEmit;
}