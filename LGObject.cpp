#include "StdAfx.h"
#include "LGObject.h"

static LGObjectClassInfo*g_classInfo=NULL;

static char szLGObject[]="LGObject";
struct LGObjectClassInfo LGObject::classLGObject=
{ szLGObject,sizeof(LGObject),NULL,NULL,NULL };
static LG_CLASSINIT _init_CObject(&LGObject::classLGObject);

LGObjectClassInfo* LGObjectClassInfo::pFirstClass=NULL;

LG_CLASSINIT::LG_CLASSINIT(LGObjectClassInfo* pNewClass)
{
	pNewClass->m_next=LGObjectClassInfo::pFirstClass;
	LGObjectClassInfo::pFirstClass=pNewClass;
}

LGObjectClassInfo* LGObjectClassInfo::FindClassByName(const char*szClassName)
{
	LGObjectClassInfo* pClass=g_classInfo;

	pClass=pFirstClass;
	while(pClass!=NULL )
	{	
		if(strcmp(szClassName,pClass->m_className)==0)
			return pClass;
		pClass=pClass->m_next;
	}

	return NULL; 
}

BOOL LGObjectClassInfo::IsDerivedFrom(const LGObjectClassInfo *baseClass)const
{
	const LGObjectClassInfo*info=this->m_pBaseClass;
	while(info)
	{
		if(info==baseClass)return TRUE;
		info=info->m_pBaseClass;
	}
	return FALSE;
}

LGObject* LGObjectClassInfo::CreateNewLGObject()const
{
	if(m_pCreateFunc==NULL)
		return NULL;	

	LGObject* pObject=NULL;
	pObject=(*m_pCreateFunc)();

	return pObject;
}
LGObject* LGObjectClassInfo::CreateNewLGObject2()const
{
	if(m_pCreateFunc==NULL)
		return NULL;	

	LGObject* pObject=NULL;
	pObject=(*m_pCreateFunc2)();

	return pObject;
}

BOOL LGObject::IsKindOf(const LGObjectClassInfo*pClass)const
{
	const LGObjectClassInfo*the_info=GetClassInfo();
	if(the_info==pClass)return TRUE;

	return the_info->IsDerivedFrom(pClass);
}

LGObjectClassInfo* LGObject::GetClassInfo()const
{
	return Desc();
}

static LGObject*obj_CreateNewLGObject()
{
	void*v=LGMemoryAllocator::lg_malloc(sizeof(LGObject));
	LGObject*p=new (v) LGObject;	

	return p;
}

LGObjectClassInfo*LGObject::Desc() 
{	
	return &classLGObject;
}

LGObjectClassInfo* LGObject::GetClassInfoHead()
{
	return g_classInfo;
}

