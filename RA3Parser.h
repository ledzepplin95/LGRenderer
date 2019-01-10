#pragma once
#include "sceneparser.h"

class RA3Parser :
	public SceneParser
{
	LG_DECLARE_DYNCREATE(RA3Parser)
public:
	BOOL parse(CString fileName,LGAPIInterface&api);	
};
