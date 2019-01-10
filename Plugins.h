#pragma once

#include <hash_map>
#include "CharLess.h"
#include "LGObject.h"

template <class T> class Plugins
{
public:
	Plugins() 
	{			
	}

	T* createObject(CString name,int tt=1) 
	{
		if(name.IsEmpty() || name=="none")
			return NULL;
		T* c=NULL;		
		LGObjectClassInfo*pClass=NULL;
		pClass=LGObjectClassInfo::FindClassByName(pluginClasses[name]);	
		if(tt==1)
		{
		    c=(T*)pClass->CreateNewLGObject();
		}
		else if(tt==2)
		{
			c=(T*)pClass->CreateNewLGObject2();
		}
        
		return c;
	}

	BOOL hasType(CString name)
	{
		hash_map<CString,CString,hash_compare<const char*,CharLess>>::const_iterator iter;
		iter=pluginClasses.find(name);
		
		return iter!=pluginClasses.end();		
	}

	CString generateUniqueName(CString prefix)
	{
		CString t;
		int i=0;

		for(;;) 
		{			
			t.Format("%s_%d",prefix,i);
			if(!hasType(t)) break;
			i++;
		}

		return t;
	}
	
	BOOL registerPlugin(CString name,CString sourceCode)
	{
        pluginClasses.insert(CC_PAIR(name,sourceCode));

		return TRUE;
	}

	Plugins<T>& operator=(const Plugins<T>&pt)
	{
		if(this==&pt) return *this;

        pluginClasses=pt.pluginClasses;		

		return *this;
	}

private:
	hash_map<CString,CString,hash_compare<const char*,CharLess>> pluginClasses;
	typedef pair<CString,CString> CC_PAIR;	
};