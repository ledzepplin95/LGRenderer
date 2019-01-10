#pragma once

#include "AccelerationStructure.h"
#include "boundingbox.h"

class KDTree : public AccelerationStructure
{
	LG_DECLARE_DYNCREATE(KDTree);

private:
	class BuildStats
	{
	public:
		BuildStats();
		void updateInner();
		void updateLeaf(int depth,int n);
		void printStats()const;

        BuildStats& operator=(const BuildStats&stat);
	
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
	};

	class BuildTask 
	{
	public:
		vector<LONG64> splits;
		int numObjects;
		int n;
		vector<byte> leftRightTable;

		BuildTask(int num);
		BuildTask(int num,const BuildTask&parent);

		BuildTask& operator=(const BuildTask&task);
	};

public:
	KDTree(void);
	KDTree(int mp);

	static void setDumpMode(BOOL d,CString prefix);
	void build(PrimitiveList*p);
	void intersect(Ray&r,IntersectionState&state)const;

private:
	int dumpObj(int offset,int vertOffset,int maxN,BoundingBox&bounds, 
		CFile&file,CFile&mtlFile);
	static LONG64 pack(float split,unsigned long t,int axis,int object);
    static int unpackObject(LONG64 p);
	static int unpackAxis(LONG64 p);
	static long unpackSplitType(LONG64 p);
	static float unpackSplit(LONG64 p);
	static void radix12(vector<LONG64>&splits,int n);
	void buildTree(float minx,float maxx,float miny,float maxy,float minz,
		float maxz,const BuildTask&task,int depth,vector<int>&tempTree,int offset,
		vector<int>&tempList,BuildStats&stats);

private:
	vector<int> tree;
	vector<int> primitives;
	PrimitiveList* primitiveList;
	BoundingBox bounds;
	int maxPrims;

	static float INTERSECT_COST;
	static float TRAVERSAL_COST;
	static float EMPTY_BONUS;
	static int MAX_DEPTH;
	static BOOL dump;
	static CString dumpPrefix;
};
