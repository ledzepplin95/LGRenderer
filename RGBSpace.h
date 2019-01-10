#pragma once

class RGBSpace
{
public:
	RGBSpace(void);
	~RGBSpace(void);
	RGBSpace(float _xRed,float _yRed,float _xGreen,float _yGreen,float _xBlue, 
		float _yBlue,float _xWhite,float _yWhite,float _gamma,float _breakPoint);
	RGBSpace(const RGBSpace&rs);

	void set(float _xRed,float _yRed,float _xGreen,float _yGreen,float _xBlue, 
		float _yBlue,float _xWhite,float _yWhite,float _gamma,float _breakPoint);
	RGBSpace& operator=(const RGBSpace&rs);

	void convertXYZtoRGB(const XYZColor&c1,Color&c2)const;
	void convertXYZtoRGB(float X,float Y,float Z,Color&c)const;
	XYZColor convertRGBtoXYZ(const Color&c)const;
	BOOL insideGamut(float r,float g,float b)const;
	float gammaCorrect(float v)const;
	float ungammaCorrect(float vp)const;
	int rgbToNonLinear(int rgb)const;
	int rgbToLinear(int rgb)const;
	byte rgbToNonLinear(byte r)const;
	byte rgbToLinear(byte r)const;
	CString toString()const;

	static RGBSpace ADOBE;
	static RGBSpace APPLE;
	static RGBSpace NTSC;
	static RGBSpace HDTV;
	static RGBSpace SRGB;
	static RGBSpace CIE;
	static RGBSpace EBU;
	static RGBSpace SMPTE_C;
	static RGBSpace SMPTE_240M;
	static RGBSpace WIDE_GAMUT;

private:
	float gamma,breakPoint;
	float slope,slopeMatch,segmentOffset;
	float xr,yr,zr,xg,yg,zg,xb,yb,zb;
	float xw,yw,zw;
	float rx,ry,rz,gx,gy,gz,bx,by,bz;
	float rw,gw,bw;
	vector<int> GAMMA_CURVE;
	vector<int> INV_GAMMA_CURVE;
};
