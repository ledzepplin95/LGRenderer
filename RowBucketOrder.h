#pragma once

#include "BucketOrder.h"

class RowBucketOrder : public BucketOrder
{
	LG_DECLARE_DYNCREATE(RowBucketOrder);

public:
    vector<int> getBucketSequence(int nbw,int nbh)const;	
};
