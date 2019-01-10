#pragma once

#include "BucketOrder.h"

class ColumnBucketOrder : public BucketOrder
{
	LG_DECLARE_DYNCREATE(ColumnBucketOrder);

public:
	vector<int> getBucketSequence(int nbw,int nbh)const;
};
