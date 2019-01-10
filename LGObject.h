#pragma once

#include "LGMemoryAllocator.h"

class LGObject;
typedef LGObject*(*CreateLGObjectFunc)();
struct LGObjectClassInfo
{
	const char* m_className;
	DWORD m_nObjectSize;	
	CreateLGObjectFunc m_pCreateFunc;
	CreateLGObjectFunc m_pCreateFunc2;
	LGObjectClassInfo* m_pBaseClass;
	LGObjectClassInfo*m_next;

	static LGObjectClassInfo* pFirstClass;	
	static LGObjectClassInfo* FindClassByName(const char*szClassName);

public:
	BOOL IsDerivedFrom(const LGObjectClassInfo *baseClass) const;
	LGObject*CreateNewLGObject() const;
	LGObject*CreateNewLGObject2() const;
};

struct LG_CLASSINIT
{ LG_CLASSINIT(LGObjectClassInfo* pNewClass); };

#define LG_CLASS(class_name) \
	(&class_name::class##class_name)

#define LG_DECLARE_DYNAMIC(class_name) \
public: \
	static LGObjectClassInfo class##class_name; \
	virtual LGObjectClassInfo* GetClassInfo() const;

#define LG_DECLARE_DYNCREATE(class_name) \
	LG_DECLARE_DYNAMIC(class_name) \
	static LGObject* CreateNewLGObject(); \
    static LGObject* CreateNewLGObject2();

#define _IMPLEMENT_LGCLASS(class_name,base_class_name,pfnNew,pfnNew2) \
	static char _lpsz##class_name[] = #class_name; \
	LGObjectClassInfo class_name::class##class_name = { \
	_lpsz##class_name, sizeof(class_name), pfnNew, pfnNew2, \
	LG_CLASS(base_class_name), NULL }; \
	static LG_CLASSINIT _init_##class_name(&class_name::class##class_name); \
	LGObjectClassInfo* class_name::GetClassInfo() const \
{ return &class_name::class##class_name; } 		

#define LG_IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	_IMPLEMENT_LGCLASS(class_name, base_class_name)

#define LG_IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	LGObject* class_name::CreateNewLGObject() \
{ \
	void*p=LGMemoryAllocator::lg_malloc(sizeof(class_name)); \
	LGObject*v=reinterpret_cast<LGObject*>(new (p) class_name); \
	return v; \
} \
	LGObject* class_name::CreateNewLGObject2() \
{ \
	LGObject*p=reinterpret_cast<LGObject*>(new class_name); \
	LGMemoryAllocator::lg_mem_pointer(p); \
	return p; \
} \
	_IMPLEMENT_LGCLASS(class_name, base_class_name, \
	class_name::CreateNewLGObject, \
    class_name::CreateNewLGObject2) 

class LGObject
{
public:
	BOOL IsKindOf(const LGObjectClassInfo*pClass)const;
	virtual LGObjectClassInfo* GetClassInfo()const;
	virtual ~LGObject(){};
	static LGObjectClassInfo*Desc();	
	static LGObjectClassInfo* GetClassInfoHead();

public:
	static LGObjectClassInfo classLGObject;
};
