#include "StdAfx.h"
#include "FileUtil.h"

CString FileUtil::getExtension(CString fileName)
{
	if(fileName.IsEmpty())
		return fileName;

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];	
	_splitpath_s(fileName,drive,dir,fname,ext);

	return ext;
}
