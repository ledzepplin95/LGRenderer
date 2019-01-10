#pragma once

class IntersectionState
{
public:

	struct StackNode
	{
		int m_node;
		float m_near;
		float m_far;
		StackNode();
		StackNode(int node,float n,float f);
		StackNode(const StackNode&sn);        

        StackNode& operator=(const StackNode&sn);

		BOOL operator==(const StackNode&sn)const;
		BOOL operator!=(const StackNode&sn)const;
	};

public:
	IntersectionState();

	float getTime()const;	
	BOOL hit()const;
	BOOL operator==(const IntersectionState&is)const;
	BOOL operator!=(const IntersectionState&is)const;

	vector<StackNode>& getStack();
	void setIntersection(int iden);
	void setIntersection(int iden,float uu,float vv);
	void setIntersection(int iden,float uu,float vv,float ww);
	IntersectionState& operator=(const IntersectionState&iState);	

	float time;
	float u,v,w;
	Instance* instance;
	int id;
	vector<vector<StackNode>> stacks;
	Instance* current;
	long numEyeRays;
	long numShadowRays;
	long numReflectionRays;
	long numGlossyRays;
	long numRefractionRays;
	long numRays;

private:
	static int MAX_STACK_SIZE;
};
