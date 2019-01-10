#pragma once

#include "BucketOrder.h"

class InvertedBucketOrder : public BucketOrder
{
    LG_DECLARE_DYNCREATE(InvertedBucketOrder);

public:
	InvertedBucketOrder(){}
	InvertedBucketOrder(BucketOrder* o);

	vector<int> getBucketSequence(int nbw,int nbh)const;

private:
	BucketOrder* order;
};
