#include "stdafx.h"
#include "PhotonStore.h"

LG_IMPLEMENT_DYNCREATE(PhotonStore,LGObject)

void PhotonStore::prepare(Options*options,const BoundingBox&sceneBounds)
{
}

void PhotonStore::store(ShadingState&state,const Vector3&dir,const Color&power,const Color&diffuse)
{
}

void PhotonStore::init()
{
}	

int PhotonStore::numEmit()const
{
	return 0;
}

BOOL PhotonStore::allowDiffuseBounced()const
{
	return FALSE;
}	

BOOL PhotonStore::allowReflectionBounced()const
{
	return FALSE;
}	

BOOL PhotonStore::allowRefractionBounced() const
{
	return FALSE;
}