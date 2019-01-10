#pragma once
#include "scabstractparser.h"
#include "parser.h"

class SCAsciiParser :
	public SCAbstractParser
{
	LG_DECLARE_DYNCREATE(SCAsciiParser);

protected:    
	Color parseColor();
    Matrix4 parseMatrix();
	void closeParser();
	void openParser(CString fileName);
	BOOL parseBoolean();
	float parseFloat();
	int parseInt();
	CString parseString();
	CString parseVerbatimString();
    ParameterList::InterpolationType parseInterpolationType();
	SCAbstractParser::SCAP_Keyword parseKeyword();
	
private:
    Parser p;

	BOOL anyEqual(const CString&source,long num,CString first,...);	
};
