#pragma once

#include "LGObject.h"

class PhotonStore : public LGObject 
{
	LG_DECLARE_DYNCREATE(PhotonStore);

public:	
	virtual void prepare(Options*options,const BoundingBox&sceneBounds);	
	virtual void store(ShadingState&state,const Vector3&dir,const Color&power,const Color&diffuse);
	virtual void init();	

	virtual int numEmit()const;
	virtual BOOL allowDiffuseBounced()const;
	virtual BOOL allowReflectionBounced()const;
	virtual BOOL allowRefractionBounced()const;	
};
