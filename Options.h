#pragma once

#include "RenderObject.h"
#include "ParameterList.h"

class Options : public ParameterList,public RenderObject
{
	LG_DECLARE_DYNCREATE(Options);

public:
	virtual ~Options(){};
	BOOL update(ParameterList&pl,LGAPI&api);
};
