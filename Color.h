#pragma once

#include "RGBSpace.h"

class Color
{
public:
	Color(void);
	~Color(void);
    Color(float gray);
	Color(float rr,float gg,float bb);
	Color(const Color&c);
	Color(int rgb);

	float getRed()const;
	float getGreen()const;
	float getBlue()const;
	Color copy() const ;
	BOOL isBlack()const;
	float getLuminance()const;
	float getMin()const;
	float getMax()const;
	float getAverage()const;
	vector<float> getRGB()const;
	int toRGB() const;
	int toRGBA(float a)const;
	int toRGBE()const;
	BOOL isNan()const;
	BOOL isInf()const;
	CString toString()const;
	BOOL operator==(const Color&c)const;
	BOOL operator!=(const Color&c)const;

	Color& toNonLinear();
	Color& toLinear();	
	Color& set(float rr,float gg,float bb);
	Color& set(const Color&c);
	Color& setRGB(int rgb);
	Color& setRGBE(int rgbe);
	Color& constrainRGB();	
	Color& add(const Color&c);
	Color& madd(float s,const Color&c);
	Color& madd(const Color&s,const Color&c);
	Color& sub(const Color&c);	
	Color& mul(const Color&c);
	Color& mul(float s);
	Color& div(const Color&c);
	Color& exp();
	Color& opposite();
	Color& clamp(float minimum,float maximum);
	Color& operator=(const Color&c);

	static Color add(const Color&c1,const Color&c2);
	static Color add(const Color&c1,const Color&c2,Color&dest);
	static Color sub(const Color&c1,const Color&c2);
	static Color sub(const Color&c1,const Color&c2,Color&dest);
	static Color mul(const Color&c1,const Color&c2);
	static Color mul(const Color&c1,const Color&c2,Color&dest);
	static Color mul(float s,const Color&c);
	static Color mul(float s,const Color&c,Color&dest);
	static Color div(const Color&c1,const Color&c2);
	static Color div(const Color&c1,const Color&c2,Color&dest);
	static Color blend(const Color&c1,const Color&c2,float b);
	static Color blend(const Color&c1,const Color&c2,float b,Color&dest);
	static Color blend(const Color&c1,const Color&c2,const Color&b);
	static Color blend(const Color&c1,const Color&c2,const Color&b,Color&dest);
	static BOOL hasContrast(const Color&c1,const Color&c2,float thresh);
	static void initial();

    static RGBSpace NATIVE_SPACE;
	static Color BLACK;
	static Color WHITE;
	static Color RED;
	static Color GREEN;
	static Color BLUE;
	static Color YELLOW;
	static Color CYAN;
	static Color MAGENTA;
	static Color GRAY;

private:
	float r,g,b;

	static float EXPONENT[256];	
};
