#pragma once
#include "photonstore.h"

class CausticPhotonMapInterface :
	public PhotonStore
{
	LG_DECLARE_DYNCREATE(CausticPhotonMapInterface);

public:
    virtual void getSamples(ShadingState&state)const;	
};
