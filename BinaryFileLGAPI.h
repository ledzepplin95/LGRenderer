#pragma once
#include "filelgapi.h"

class BinaryFileLGAPI :
	public FileLGAPI
{
public:
	BinaryFileLGAPI(CString fileName);

	void close();

protected:
	void writeBoolean(boolean v);
	void writeFloat(float v);
	void writeInt(int v);
	void writeInterpolationType(ParameterList::InterpolationType interp);
	void writeKeyword(SCAbstractParser::SCAP_Keyword k);
	void writeMatrix(const Matrix4&v);
	void writeString(CString s);
	void writeVerbatimString(CString s);
	void writeNewline(int indentNext);

private:
	CFile m_file;

	void writeExtendedKeyword(int code);
	void writeDatatypeKeyword(int t,boolean isArray);
};
