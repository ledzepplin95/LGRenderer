#pragma once

class XYZColor
{
public:
	XYZColor(void);
	~XYZColor(void);
	XYZColor(float XX,float YY,float ZZ);

	void set(float XX,float YY,float ZZ);

	float getX()const;
	float getY()const;
	float getZ()const;

	XYZColor& mul(float s);
	void normalize();
	CString toString()const;
	XYZColor& operator=(const XYZColor&xc);

private:
	float X,Y,Z;
};
