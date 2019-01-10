#include "StdAfx.h"

#include "MathUtil.h"
#include "float.h"

float MathUtil::NotANumber=MathUtil::intBitsToFloat(0x7fc00000);
int MathUtil::MAX_EXPONENT=127;
int MathUtil::MIN_EXPONENT=-126;
int MathUtil::EXP_BIAS=127;
int MathUtil::SIGNIFICAND_WIDTH=24;
int MathUtil::EXP_BIT_MASK=0x7F800000;

MathUtil::MathUtil(void)
{
}

int MathUtil::clamp(int x,int min,int max) 
{
	if(x>max)
		return max;
	if(x>min)
		return x;

	return min;
}

float MathUtil::clamp(float x,float min,float max) 
{
	if(x>max)
		return max;
	if(x>min)
		return x;

	return min;
}

double MathUtil::clamp(double x,double min,double max)
{
	if(x>max)
		return max;
	if(x>min)
		return x;

	return min;
}

int MathUtil::mu_min(int a,int b,int c) 
{
	if(a>b)
		a=b;
	if(a>c)
		a=c;

	return a;
}

float MathUtil::mu_min(float a,float b,float c) 
{
	if(a>b)
		a=b;
	if(a>c)
		a=c;

	return a;
}

double MathUtil::mu_min(double a,double b,double c) 
{
	if(a>b)
		a=b;
	if(a>c)
		a=c;

	return a;
}

float MathUtil::mu_min(float a,float b,float c,float d)
{
	if(a>b)
		a=b;
	if(a>c)
		a=c;
	if(a>d)
		a=d;

	return a;
}

int MathUtil::mu_max(int a,int b,int c) 
{
	if(a<b)
		a=b;
	if(a<c)
		a=c;

	return a;
}

float MathUtil::mu_max(float a,float b,float c)
{
	if(a<b)
		a=b;
	if(a<c)
		a=c;

	return a;
}

double MathUtil::mu_max(double a,double b,double c) 
{
	if(a<b)
		a=b;
	if(a<c)
		a=c;

	return a;
}

float MathUtil::mu_max(float a,float b,float c,float d) 
{
	if(a<b)
		a=b;
	if(a<c)
		a=c;
	if(a<d)
		a=d;

	return a;
}

float MathUtil::smoothStep(float a,float b,float x) 
{
	if(x<=a)
		return 0.0f;
	if(x>=b)
		return 1.0f;
	float t=clamp((x-a)/(b-a),0.0f,1.0f);

	return t*t*(3.0-2.0*t);
}

float MathUtil::frac(float x) 
{
	return x<0.0f?x-(int)x+1.0:x-(int)x;
}

int MathUtil::floatToRawIntBits(float v)
{
    return *(int*)&v;
}

float MathUtil::intBitsToFloat(int bits)
{
	float x;
	*(int*)&x=bits;

	return x;
}

static float Log2(float i)
{
	static float OOshift23=1.0/(1<<23);
	float LogBodge=0.346607f;
	float x;
	float y;
	x=*(int *)&i;
	x*=OOshift23;
	x=x-127;

	y=x-floorf(x);
	y=(y-y*y)*LogBodge;

	return x+y;
}

static float Pow2(float i)
{
	static float shift23=(1<<23);
	float PowBodge=0.33971f;
	float x;
	float y=i-floorf(i);
	y=(y-y*y)*PowBodge;

	x=i+127-y;
	x*=shift23;
	*(int*)&x=(int)x;

	return x;
}

float MathUtil::fastPow(float a,float b) 
{	
	return Pow2(b*Log2(a));
}

float MathUtil::powerOfTwoF(int n) 
{
	ASSERT( n>=MIN_EXPONENT && n<=MAX_EXPONENT );

	return intBitsToFloat( ((n+EXP_BIAS)<<(SIGNIFICAND_WIDTH-1))
		&EXP_BIT_MASK );
}

int MathUtil::getExponent(float f)
{
	return ((floatToRawIntBits(f) & EXP_BIT_MASK)>>
		(SIGNIFICAND_WIDTH-1))-EXP_BIAS;
}

float MathUtil::ulp(float f)
{	
	int e=getExponent(f);
	switch(e)
	{
	case 128:     
		return fabs(f);			
	case -127:     
		return -FLT_MAX;			
	default:
		ASSERT(e<=MAX_EXPONENT && e>=MIN_EXPONENT);
		e=e-(SIGNIFICAND_WIDTH-1);
		if(e>=MIN_EXPONENT)
		{
			return powerOfTwoF(e);				
		}
		else 
		{			
			return intBitsToFloat(1<<(e-(MIN_EXPONENT-(SIGNIFICAND_WIDTH-1))));
		}
	}	
}