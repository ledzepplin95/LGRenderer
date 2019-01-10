#include "StdAfx.h"

#include "GridPhotonMap.h"
#include "LGdef.h"
#include "strutil.h"
#include "ShadingState.h"
#include "MathUtil.h"
#include "options.h"

float GridPhotonMap::NORMAL_THRESHOLD=(float)cos(10.0*LG_PI/180.0);
int GridPhotonMap::PRIMES[]={ 11, 19, 37, 109, 163, 251, 367, 557,
823, 1237, 1861, 2777, 4177, 6247, 9371, 21089, 31627, 47431,
71143, 106721, 160073, 240101, 360163, 540217, 810343, 1215497,
1823231, 2734867, 4102283, 6153409, 9230113, 13845163 };

LG_IMPLEMENT_DYNCREATE(GridPhotonMap,GlobalPhotonMapInterface)
	LG_IMPLEMENT_DYNCREATE(PhotonGroup,LGObject)

PhotonGroup::PhotonGroup(int iden,const Vector3&n) 
{
	normal.set(n);
	flux=Color::BLACK;
	diffuse=Color::BLACK;		
	count=0;
	id=iden;
	next=NULL;
}

GridPhotonMap::GridPhotonMap() 
{
	numStoredPhotons=0;
	hashSize=0;
	nEmit=100000;
}

void GridPhotonMap::prepare(Options*options,const BoundingBox&sceneBounds)
{	
	nEmit=options->getInt("gi.irr-cache.gmap.emit",100000);
	numGather=options->getInt("gi.irr-cache.gmap.gather",50);
	gatherRadius=options->getFloat("gi.irr-cache.gmap.radius",0.5f);

	bounds.set(sceneBounds);
	bounds.enlargeUlps();
	Vector3 w=bounds.getExtents();
	nx=(int)max(((w.x/gatherRadius)+0.5f),1);
	ny=(int)max(((w.y/gatherRadius)+0.5f),1);
	nz=(int)max(((w.z/gatherRadius)+0.5f),1);
	int numCells=nx*ny*nz;
	StrUtil::PrintPrompt("初始化grid photon map:");
	StrUtil::PrintPrompt("  * 分辨率:   %dx%dx%d",nx,ny,nz);
	StrUtil::PrintPrompt("  * 单元总数: %d",numCells);
	for(hashPrime=0; hashPrime<32; hashPrime++)
		if(PRIMES[hashPrime]>(numCells/5))
			break;
	cellHash.resize(PRIMES[hashPrime]);
	StrUtil::PrintPrompt("  * 初始化hash大小: %d",cellHash.size());
}

int GridPhotonMap::size() 
{
	return numStoredPhotons;
}

void GridPhotonMap::store(ShadingState&state,const Vector3&dir,
						  const Color&power,const Color&diffuse)
{
	if( Vector3::dot(state.getNormal(),dir)>0.0f )
		return;
	Point3 pt=state.getPoint();	
	if( !bounds.contains(pt) )
		return;
	Vector3 ext=bounds.getExtents();
	int ix=(int)(((pt.x-bounds.getMinimum().x)*nx)/ext.x);
	int iy=(int)(((pt.y-bounds.getMinimum().y)*ny)/ext.y);
	int iz=(int)(((pt.z-bounds.getMinimum().z)*nz)/ext.z);
	ix=MathUtil::clamp(ix,0,nx-1);
	iy=MathUtil::clamp(iy,0,ny-1);
	iz=MathUtil::clamp(iz,0,nz-1);
	int id=ix+iy*nx+iz*nx*ny;
	
	cs.Lock();
	int hid=id%cellHash.size();
	PhotonGroup* g=cellHash[hid];
	PhotonGroup* last=NULL;
	BOOL hasID=FALSE;
	while(g!=NULL) 
	{
		if(g->id==id)
		{
			hasID=TRUE;
			if(Vector3::dot(state.getNormal(),g->normal)>NORMAL_THRESHOLD)
				break;
		}
		last=g;
		g=g->next;
	}
	if(g==NULL)
	{	
		void*lgp=LGMemoryAllocator::lg_malloc(sizeof(PhotonGroup));
		g=new (lgp) PhotonGroup(id,state.getNormal());		
		if(last==NULL)
			cellHash[hid]=g;
		else
			last->next=g;
		if(!hasID) 
		{
			hashSize++; 				
			if(hashSize>cellHash.size())
				growPhotonHash();
		}
	}
	g->count++;
	g->flux.add(power);
	g->diffuse.add(diffuse);
	numStoredPhotons++;
	cs.Unlock();
}

void GridPhotonMap::init() 
{
	StrUtil::PrintPrompt("初始化光子网格...");
	StrUtil::PrintPrompt("  * 光子命中:      %d",numStoredPhotons);
	StrUtil::PrintPrompt("  * 最终hash大小:  %d",cellHash.size());
	int cells=0;
	for(int i=0; i<cellHash.size(); i++) 
	{
		for(PhotonGroup* g=cellHash[i]; g!=NULL; g=g->next)
		{
			g->diffuse.mul(1.0f/g->count);
			cells++;
		}
	}
	StrUtil::PrintPrompt("  * 光子单元数: %d",cells);
}

void GridPhotonMap::precomputeRadiance(BOOL includeDirect,BOOL includeCaustics) 
{
}

void GridPhotonMap::growPhotonHash()
{
	if(hashPrime>=32-1)
		return;
	vector<PhotonGroup*> temp;
	temp.resize(PRIMES[++hashPrime]);
	for(int i=0; i<cellHash.size(); i++) 
	{
		PhotonGroup* g=cellHash[i];
		while(g!=NULL)
		{			
			int hid=g->id%temp.size();
			PhotonGroup* last=NULL;
			for(PhotonGroup* gn=temp[hid]; gn!=NULL; gn=gn->next)
				last=gn;
			if(last==NULL)
				temp[hid]=g;
			else
				last->next=g;
			PhotonGroup* next=g->next;
			g->next=NULL;
			g=next;
		}
	}
	cellHash=temp;
}

Color GridPhotonMap::getRadiance(const Point3&p,const Vector3&n)
{
	if(!bounds.contains(p))	return Color::BLACK;

	cs.Lock();
	Vector3 ext=bounds.getExtents();
	int ix=(int)(((p.x-bounds.getMinimum().x)*nx)/ext.x);
	int iy=(int)(((p.y-bounds.getMinimum().y)*ny)/ext.y);
	int iz=(int)(((p.z-bounds.getMinimum().z)*nz)/ext.z);
	ix=MathUtil::clamp(ix,0,nx-1);
	iy=MathUtil::clamp(iy,0,ny-1);
	iz=MathUtil::clamp(iz,0,nz-1);
	int id=ix+iy*nx+iz*nx*ny;
	PhotonGroup* center=NULL;
	for(PhotonGroup* g=get(ix,iy,iz); g!=NULL; g=g->next) 
	{
		if(g->id==id && Vector3::dot(n,g->normal)>NORMAL_THRESHOLD) 
		{
			if(g->radiance==Color(0,0,0)) 
			{
				center=g;
				break;
			}
			Color r=g->radiance.copy();	

			return r;
		}
	}
	int vol=1;
	while(TRUE)
	{
		int numPhotons=0;
		int ndiff=0;
		Color irr=Color::BLACK;
		Color diff=Color::BLACK;
		for(int z=iz-(vol-1); z<=iz+(vol-1); z++) 
		{
			for(int y=iy-(vol-1); y<=iy+(vol-1); y++) 
			{
				for(int x=ix-(vol-1); x <=ix+(vol-1); x++) 
				{
					int vid=x+y*nx+z*nx*ny;
					for(PhotonGroup* g=get(x,y,z); g!=NULL; g=g->next)
					{
						if( g->id==vid && Vector3::dot(n,g->normal)>NORMAL_THRESHOLD ) 
						{
							numPhotons+=g->count;
							irr.add(g->flux);
							if(diff!=Color(0,0,0)) 
							{
								diff.add(g->diffuse);
								ndiff++;
							}
							break; 							
						}
					}
				}
			}
		}
		if( numPhotons>=numGather || vol>=3 ) 
		{			
			float area=(2.0f*vol-1.0f)/3.0f*((ext.x/nx)+(ext.y/ny)+(ext.z/nz));
			area*=area;
			area*=LG_PI;
			irr.mul(1.0f/area);			
			if(ndiff>0)
				diff.mul(1.0f/ndiff);	
			void*lgp=LGMemoryAllocator::lg_malloc(sizeof(PhotonGroup));
			center=new (lgp) PhotonGroup(id,n);			
			center->diffuse.set(diff);
			center->next=cellHash[id%cellHash.size()];
			cellHash[id%cellHash.size()]=center;			
			irr.mul(center->diffuse);
			center->radiance=irr.copy();

			return irr;
		}
		vol++;
	}
	cs.Unlock();
}

PhotonGroup* GridPhotonMap::get(int x,int y,int z) const
{
	if( x<0 || x>=nx )
		return NULL;
	if( y<0 || y>=ny )
		return NULL;
	if( z<0 || z>=nz )
		return NULL;

	return cellHash[(x+y*nx+z*nx*ny)%cellHash.size()];
}

BOOL GridPhotonMap::allowDiffuseBounced()const 
{
	return TRUE;
}

BOOL GridPhotonMap::allowReflectionBounced() const
{
	return TRUE;
}

BOOL GridPhotonMap::allowRefractionBounced() const
{
	return TRUE;
}

int GridPhotonMap::numEmit() const
{
	return nEmit;
}