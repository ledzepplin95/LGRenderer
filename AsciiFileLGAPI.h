#pragma once
#include "filelgapi.h"

class AsciiFileLGAPI :
	public FileLGAPI
{
public:
	AsciiFileLGAPI(CString fileName);
 
	void close();

protected:
	void writeBoolean(boolean v);
	void writeFloat(float v);
    void writeInt(int v);
	void writeInterpolationType(ParameterList::InterpolationType interp);
	void writeKeyword(SCAbstractParser::SCAP_Keyword k);
    void writeMatrix(const Matrix4&v);
	void writeNewline(int indentNext);
	void writeString(CString s);
	void writeVerbatimString(CString s);

private:
	CFile m_file;	
};
