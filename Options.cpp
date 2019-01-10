#include "stdafx.h"
#include "options.h"

LG_IMPLEMENT_DYNCREATE(Options,RenderObject)

BOOL Options::update(ParameterList&pl,LGAPI&api) 
{	
	hash_map<CString,Parameter,
		hash_compare<const char*,CharLess>>::iterator iter;
	for(iter=pl._list.begin(); iter!=pl._list.end(); iter++) 
	{		
		_list.insert( ParameterList::CP_PAIR(iter->first,
			iter->second) );
		iter->second.check();
	}

	return TRUE;
}