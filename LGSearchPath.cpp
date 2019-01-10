#include "stdafx.h"
#include "LGSearchPath.h"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "StrUtil.h"

LGSearchPath::LGSearchPath()
{
}

LGSearchPath::LGSearchPath(CString t) 
{
	LGSPType=t;	
}

void LGSearchPath::Set(CString t)
{
	searchPath.clear();
    LGSPType=t;
}

void LGSearchPath::resetSearchPath() 
{
	searchPath.clear();
}

void LGSearchPath::addSearchPath(CString path) 
{	
	if(PathFileExists(path))
	{
		try 
		{			
			for(list<CString>::iterator it=searchPath.begin();
				it!=searchPath.end(); it++)
			{
				CString prefix=*it;
				if(prefix==path) return;
			}
			StrUtil::PrintPrompt("增加%s搜索路径:\"%s\"",LGSPType,path);
			searchPath.push_back(path);
		} 
		catch(...)
		{
			StrUtil::PrintPrompt("指定的%s搜索路径无效:\"%s\"",LGSPType,path);
		}
	} 
	else
		StrUtil::PrintPrompt("指定的%s搜索路径无效:\"%s\" -无效目录",LGSPType,path);
}

CString LGSearchPath::resolvePath(CString fileName)
{	
	if(fileName.Find("//",0)==0)
		fileName=StrUtil::subString(fileName,2);
	StrUtil::PrintPrompt("解析%s路径\"%s\"...",LGSPType,fileName);
	if(!PathFileExists(fileName)) 
	{
		for(list<CString>::iterator it=searchPath.begin();
			it!=searchPath.end(); it++)
		{
			CString prefix=*it;
			StrUtil::PrintPrompt("  * 搜索:\"%s\"...",prefix);
			prefix+=fileName;

			if(PathFileExists(prefix))						
				return prefix;		
		}
	}

	return fileName;
}

LGSearchPath& LGSearchPath::operator=(const LGSearchPath&lgsp)
{
	if(this==&lgsp) return *this;

    searchPath=lgsp.searchPath;
    LGSPType=lgsp.LGSPType;

	return *this;
}