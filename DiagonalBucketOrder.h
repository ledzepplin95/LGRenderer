#pragma once

#include "BucketOrder.h"

class DiagonalBucketOrder : public BucketOrder
{
	LG_DECLARE_DYNCREATE(DiagonalBucketOrder);

public:
    vector<int> getBucketSequence(int nbw,int nbh)const;
};
