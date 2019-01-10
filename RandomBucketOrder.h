#pragma once

#include "BucketOrder.h"

class RandomBucketOrder : public BucketOrder
{
    LG_DECLARE_DYNCREATE(RandomBucketOrder);

public:
    vector<int> getBucketSequence(int nbw,int nbh)const;

private:
	int mod(int a,int b)const;
	long xorshift(long y)const;	
};
