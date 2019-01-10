#pragma once

#include "bucketorder.h"

class HilbertBucketOrder : public BucketOrder
{
	LG_DECLARE_DYNCREATE(HilbertBucketOrder);

public:
    vector<int> getBucketSequence(int nbw,int nbh)const;	
};
