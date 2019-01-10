#pragma once

#include "BucketOrder.h"

class SpiralBucketOrder : public BucketOrder
{
	LG_DECLARE_DYNCREATE(SpiralBucketOrder);

public:
    vector<int> getBucketSequence(int nbw,int nbh)const;
};
