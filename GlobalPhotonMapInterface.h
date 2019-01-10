#pragma once
#include "photonstore.h"

class GlobalPhotonMapInterface :
	public PhotonStore
{
	LG_DECLARE_DYNCREATE(GlobalPhotonMapInterface);

public:
	virtual Color getRadiance(const Point3&p,const Vector3&n);
};
