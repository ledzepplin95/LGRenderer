#pragma  once
#include <list>

class LGSearchPath
{
public:
	LGSearchPath();
	LGSearchPath(CString t);
	void Set(CString t);

	void resetSearchPath();
	void addSearchPath(CString path);
	CString resolvePath(CString fileName);
	LGSearchPath& operator=(const LGSearchPath&lgsp);

private:
	list<CString> searchPath;
	CString LGSPType;
};