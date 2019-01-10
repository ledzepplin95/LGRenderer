#pragma once
#include "sceneparser.h"
#include "parameterlist.h"

class SCAbstractParser :
	public SceneParser
{
	LG_DECLARE_DYNCREATE(SCAbstractParser);

public:

	enum SCAP_Keyword
	{
		SCAP_RESET, 
		SCAP_PARAMETER, 
		SCAP_GEOMETRY,
		SCAP_INSTANCE,
		SCAP_SHADER, 
		SCAP_MODIFIER,
		SCAP_LIGHT, 
		SCAP_CAMERA, 
		SCAP_OPTIONS, 
		SCAP_INCLUDE,
		SCAP_REMOVE,
		SCAP_FRAME, 
		SCAP_PLUGIN,
		SCAP_SEARCHPATH,
		SCAP_STRING,
		SCAP_BOOL,
		SCAP_INT,
		SCAP_FLOAT,
		SCAP_COLOR,
		SCAP_POINT, 
		SCAP_VECTOR, 
		SCAP_TEXCOORD,
		SCAP_MATRIX, 
		SCAP_STRING_ARRAY,
		SCAP_INT_ARRAY,
		SCAP_FLOAT_ARRAY,
		SCAP_POINT_ARRAY, 
		SCAP_VECTOR_ARRAY, 
		SCAP_TEXCOORD_ARRAY,
		SCAP_MATRIX_ARRAY,
		SCAP_END_OF_FILE,
	};

    BOOL parse(CString fileName,LGAPIInterface&api);

	static CString GetKeywordName(SCAbstractParser::SCAP_Keyword k);

protected:

	virtual ParameterList::InterpolationType parseInterpolationType();
	virtual void openParser(CString fileName);
	virtual void closeParser();
	virtual SCAP_Keyword parseKeyword();
	virtual BOOL parseBoolean();
	virtual int parseInt();
	virtual float parseFloat();
	virtual CString parseString();
	virtual CString parseVerbatimString();
	virtual Matrix4 parseMatrix();

	vector<float> parseFloatArray(int size);

private:
	void parseParameter(LGAPIInterface&api);
    vector<CString> parseStringArray(int size);
	vector<int> parseIntArray(int size);
    vector<float> parseMatrixArray(int size);
	Point3 parsePoint();
	Vector3 parseVector();
	Point2 parseTexcoord();
};
