#include "stdafx.h"
#include "GlobalPhotonMapInterface.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(GlobalPhotonMapInterface,PhotonStore)

Color GlobalPhotonMapInterface::getRadiance(const Point3&p,const Vector3&n)
{
	return Color();
}