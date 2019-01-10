#pragma once
#include "sceneparser.h"

class TriParser :
	public SceneParser
{
	LG_DECLARE_DYNCREATE(TriParser);
public:
	BOOL parse(CString fileName,LGAPIInterface&api);	
};
