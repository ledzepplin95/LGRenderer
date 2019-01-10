#include "StdAfx.h"

#include "BucketOrderFactory.h"
#include "InvertedBucketOrder.h"
#include "PluginRegistry.h"
#include "strutil.h"
#include "BucketOrder.h"

BucketOrder* BucketOrderFactory::create(CString order)
{
	BOOL flip=FALSE;
	BOOL b1=order.Find("inverse")==0;
    BOOL b2=order.Find("invert")==0;
	BOOL b3=order.Find("reverse")==0;
	if( b1||b2||b3 ) 
	{
		vector<CString> tokens=StrUtil::split(order);
		if(tokens.size()==2) 
		{
			order=tokens[1];
			flip=TRUE;
		}
	}
	BucketOrder* o=PluginRegistry::bucketOrderPlugins.createObject(order);

	if(flip)
	{
		void *p=LGMemoryAllocator::lg_malloc(sizeof(InvertedBucketOrder));		
		return new (p) InvertedBucketOrder(o);
	}
	else 
		return o;
}
