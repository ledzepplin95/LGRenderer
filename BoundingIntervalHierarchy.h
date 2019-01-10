#pragma once

#include "AccelerationStructure.h"
#include "BoundingBox.h"

class IntArray;
class BoundingIntervalHierarchy : public AccelerationStructure
{
	LG_DECLARE_DYNCREATE(BoundingIntervalHierarchy);

private:

	class BuildStats
	{
	public:
		BuildStats();

		void updateInner();
		void updateBVH2();
		void updateLeaf(int depth,int n);
		void printStats()const;

		BuildStats& operator=(const BuildStats&stat);

	private:
		int numNodes;
		int numLeaves;
		int sumObjects;
		int minObjects;
		int maxObjects;
		int sumDepth;
		int minDepth;
		int maxDepth;
		int numLeaves0;
		int numLeaves1;
		int numLeaves2;
		int numLeaves3;
		int numLeaves4;
		int numLeaves4p;
		int numBVH2;		
	};

public:
	BoundingIntervalHierarchy(void);

	void build(PrimitiveList*p);
	void buildHierarchy(IntArray&tempTree,vector<int>&indices,BuildStats&stats);
	void createNode(IntArray&tempTree,int nodeIndex,int left,int right);
	void subdivide(int left,int right,IntArray&tempTree,vector<int>&indices,
		vector<float>&gridBox,vector<float>&nodeBox,int nodeIndex,int depth,
		BuildStats&stats);
	void intersect(Ray&r,IntersectionState&state)const;

private:
	vector<int> tree;
	vector<int> objects;
	PrimitiveList* primitives;
	BoundingBox bounds;
	int maxPrims;
};
