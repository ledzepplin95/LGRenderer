#pragma once

#include "LGObject.h"

class BucketOrder : public LGObject 
{
	LG_DECLARE_DYNCREATE(BucketOrder);
public:	
    virtual vector<int> getBucketSequence(int nbw,int nbh)const;
};
