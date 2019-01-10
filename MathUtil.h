#pragma once

#include "LGDef.h"

#define  DEG_TO_RAD  (LG_PI/180.0f) 
#define  RAD_TO_DEG  (180.0f/LG_PI) 
#define  DegToRad(deg) (((float)deg)*DEG_TO_RAD) 
#define  RadToDeg(rad) (((float)rad)*RAD_TO_DEG) 

class MathUtil
{
	MathUtil(void);

public:
	static int clamp(int x,int min,int max);
	static float clamp(float x,float min,float max);
	static double clamp(double x,double min,double max);
	static int mu_min(int a,int b,int c);
	static float mu_min(float a,float b,float c);
	static double mu_min(double a,double b,double c);
	static float mu_min(float a,float b,float c,float d);
	static int mu_max(int a,int b,int c);
	static float mu_max(float a,float b,float c);
	double mu_max(double a,double b,double c);
	static float mu_max(float a,float b,float c,float d);
	static float smoothStep(float a,float b,float x);
	static float frac(float x);
	static float fastPow(float a,float b);
	static int floatToRawIntBits(float v);
	static float intBitsToFloat(int bits);
	static float ulp(float f);
	static float powerOfTwoF(int n);
	static int getExponent(float f);

	static float NotANumber;
	static int MAX_EXPONENT;
	static int MIN_EXPONENT;
	static int EXP_BIAS;
	static int SIGNIFICAND_WIDTH;
	static int EXP_BIT_MASK;
};
