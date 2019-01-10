#pragma once

#include "parser.h"
#include "CharLess.h"
#include <hash_map>
#include "sceneparser.h"

class SCParser : public SceneParser
{
	LG_DECLARE_DYNCREATE(SCParser);

public:
	SCParser(void);
	
	BOOL parse(CString fileName,LGAPIInterface&api); 
private:
	static int instanceCounter;

	int instanceNumber;
	Parser p;
	int numLightSamples;
	hash_map<CString,int,
		hash_compare<const char*,CharLess>> objectNames;
	typedef pair<CString,int> CI_PAIR;	
	int width,height;

	CString generateUniqueName(CString prefix);
	void parseImageBlock(LGAPIInterface&api);
	void parseBackgroundBlock(LGAPIInterface&api);
	void parseFilter(LGAPIInterface&api);
	void parsePhotonBlock(LGAPIInterface&api);
	void parseGIBlock(LGAPIInterface&api);
	void parseLightserverBlock(LGAPIInterface&api);
	void parseTraceBlock(LGAPIInterface&api);
	void parseCamera(LGAPIInterface&api);
	void parseCameraTransform(LGAPIInterface&api);
	void parseCameraMatrix(int index,LGAPIInterface&api);
	BOOL parseShader(LGAPIInterface&api);
	BOOL parseModifier(LGAPIInterface&api);
	void parseObjectBlock(LGAPIInterface&api);
	void parseInstanceBlock(LGAPIInterface&api);
	void parseLightBlock(LGAPIInterface&api);
	Color parseColor();
	Point3 parsePoint();
	Vector3 parseVector();
	vector<int> parseIntArray(int size);
	vector<float> parseFloatArray(int size);
	Matrix4 parseMatrix();
	void parseCornellBox(LGAPIInterface&api);

	BOOL IsValueIn(CString name,int&v)const;
};
