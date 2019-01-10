#include "stdafx.h"
#include "modifier.h"

LG_IMPLEMENT_DYNCREATE(Modifier,RenderObject)

void Modifier::modify(ShadingState&state)const
{
}

BOOL Modifier::operator==(const Modifier &vec) const
{
	return FALSE;
}