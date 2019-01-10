#pragma once
#include "scabstractparser.h"

class SCBinaryParser :
	public SCAbstractParser
{
	LG_DECLARE_DYNCREATE(SCBinaryParser)
public:
	void openParser(CString fileName);
    void closeParser();
	BOOL parseBoolean();
    float parseFloat();
	int parseInt();
    Matrix4 parseMatrix();
    CString parseString();
    CString parseVerbatimString();
	ParameterList::InterpolationType parseInterpolationType();
    SCAbstractParser::SCAP_Keyword parseKeyword();
	
private:
	CFile m_file;
    CArchive* m_ar;
};
