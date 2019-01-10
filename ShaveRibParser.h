#pragma once
#include "sceneparser.h"

class ShaveRibParser :
	public SceneParser
{
	LG_DECLARE_DYNCREATE(ShaveRibParser);

public:
	BOOL parse(CString fileName,LGAPIInterface&api);

private:
	vector<int> parseIntArray(Parser&p);
	vector<float> parseFloatArray(Parser&p);	
};
