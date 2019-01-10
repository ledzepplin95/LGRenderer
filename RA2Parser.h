#pragma once
#include "sceneparser.h"

class RA2Parser :
	public SceneParser
{
	LG_DECLARE_DYNCREATE(RA2Parser);

public:
	BOOL parse(CString fileName,LGAPIInterface&api);	
};
