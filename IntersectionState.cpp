#include "StdAfx.h"

#include "IntersectionState.h"

int IntersectionState::MAX_STACK_SIZE=64;

IntersectionState::StackNode::StackNode()
{
	m_node=0;
	m_near=0.0f;
	m_far=0.0f;
}

IntersectionState::StackNode::StackNode(int node,float n,float f)
{
    m_node=node;
	m_near=n;
    m_far=f;
}

IntersectionState::StackNode::StackNode(const IntersectionState::StackNode&sn)
{
	if(this==&sn) return;
	m_node=sn.m_node;
	m_near=sn.m_near;
	m_far=sn.m_far;
}

IntersectionState::StackNode& IntersectionState::StackNode::operator=(const IntersectionState::StackNode &sn)
{
	if(this==&sn) return *this;

	m_node=sn.m_node;
	m_near=sn.m_near;
	m_far=sn.m_far;

	return *this;
}

BOOL IntersectionState::StackNode::operator==(const StackNode&sn)const
{
	const float eps=1.0e-6f;
	if(m_node!=sn.m_node) return FALSE;
    if(fabs(m_far-sn.m_far)>eps) return FALSE;
	if(fabs(m_near-sn.m_near)>eps) return FALSE;

	return TRUE;
}

BOOL IntersectionState::StackNode::operator!=(const StackNode&sn)const
{
	const float eps=1.0e-6f;
	if(m_node!=sn.m_node) return TRUE;
	if(fabs(m_far-sn.m_far)>eps) return TRUE;
	if(fabs(m_near-sn.m_near)>eps) return TRUE;

	return FALSE;
}

IntersectionState::IntersectionState() 
{
	instance=NULL;
	current=NULL;
    time=0.0f;
	u=v=w=0.0f;
	id=0;
	numRays=0;
	numEyeRays=0;
	numShadowRays=0;
	numReflectionRays=0;
	numGlossyRays=0;
	numRefractionRays=0;
    stacks.resize(2);
	for(int i=0; i<stacks.size(); i++)			
		stacks[i].resize(MAX_STACK_SIZE);	
}

float IntersectionState::getTime()const
{
	return time;
}

vector<IntersectionState::StackNode>& IntersectionState::getStack() 
{
	return current==NULL?stacks[0]:stacks[1];
}

BOOL IntersectionState::hit() const
{
	return instance!=NULL;
}

void IntersectionState::setIntersection(int iden)
{
	instance=current;
	id=iden;
}

void IntersectionState::setIntersection(int iden,float uu,float vv) 
{
	instance=current;
	id=iden;
	u=uu;
	v=vv;
}

void IntersectionState::setIntersection(int iden,float uu,float vv,float ww) 
{
	instance=current;
	id=iden;
	u=uu;
	v=vv;
	w=ww;
}

IntersectionState& IntersectionState::operator=(const IntersectionState&iState)
{
	if(this==&iState) return *this;

	time=iState.time;
	u=iState.u;
	v=iState.v;
	w=iState.w;
	instance=iState.instance;
    id=iState.id;
	stacks=iState.stacks;
	current=iState.current;
	numEyeRays=iState.numEyeRays;
	numShadowRays=iState.numShadowRays;
	numReflectionRays=iState.numReflectionRays;
	numGlossyRays=iState.numGlossyRays;
	numRefractionRays=iState.numRefractionRays;
	numRays=iState.numRays;

	return *this;
}

BOOL IntersectionState::operator==(const IntersectionState&is)const
{
	const float eps=1.0e-6f;
	if(fabs(time-is.time)>eps) return FALSE;
	if(fabs(u-is.u)>eps) return FALSE;
	if(fabs(v-is.v)>eps) return FALSE;
	if(fabs(w-is.w)>eps) return FALSE;
	if(id!=is.id) return FALSE;
	if(stacks!=is.stacks) return FALSE;
	if(numEyeRays!=is.numEyeRays) return FALSE;
	if(numShadowRays!=is.numShadowRays) return FALSE;
	if(numReflectionRays!=is.numReflectionRays) return FALSE;
	if(numGlossyRays!=is.numGlossyRays) return FALSE;
	if(numRefractionRays!=is.numRefractionRays) return FALSE;
	if(numRays!=is.numRays) return FALSE;
	if(instance!=is.instance) return FALSE;
	if(current!=is.current) return FALSE;

	return TRUE;
}

BOOL IntersectionState::operator !=(const IntersectionState &is) const
{
	const float eps=1.0e-6f;
	if(fabs(time-is.time)>eps) return TRUE;
	if(fabs(u-is.u)>eps) return TRUE;
	if(fabs(v-is.v)>eps) return TRUE;
	if(fabs(w-is.w)>eps) return TRUE;
	if(id!=is.id) return TRUE;
	if(stacks!=is.stacks) return TRUE;
	if(numEyeRays!=is.numEyeRays) return TRUE;
	if(numShadowRays!=is.numShadowRays) return TRUE;
	if(numReflectionRays!=is.numReflectionRays) return TRUE;
	if(numGlossyRays!=is.numGlossyRays) return TRUE;
	if(numRefractionRays!=is.numRefractionRays) return TRUE;
	if(numRays!=is.numRays) return TRUE;
	if(instance!=is.instance) return TRUE;
	if(current!=is.current) return TRUE;

	return FALSE;
}