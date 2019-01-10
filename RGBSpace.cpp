#include "StdAfx.h"

#include "RGBSpace.h"
#include "mathutil.h"
#include "color.h"
#include "XYZColor.h"

RGBSpace RGBSpace::ADOBE(0.6400f, 0.3300f, 0.2100f, 0.7100f, 0.1500f, 0.0600f, 0.31271f, 0.32902f, 2.2f, 0);
RGBSpace RGBSpace::APPLE(0.6250f, 0.3400f, 0.2800f, 0.5950f, 0.1550f, 0.0700f, 0.31271f, 0.32902f, 1.8f, 0);
RGBSpace RGBSpace::NTSC(0.6700f, 0.3300f, 0.2100f, 0.7100f, 0.1400f, 0.0800f, 0.31010f, 0.31620f, 20.0f/9.0f, 0.018f);
RGBSpace RGBSpace::HDTV(0.6400f, 0.3300f, 0.3000f, 0.6000f, 0.1500f, 0.0600f, 0.31271f, 0.32902f, 20.0f/9.0f, 0.018f);
RGBSpace RGBSpace::SRGB(0.6400f, 0.3300f, 0.3000f, 0.6000f, 0.1500f, 0.0600f, 0.31271f, 0.32902f, 2.4f, 0.00304f);
RGBSpace RGBSpace::CIE(0.7350f, 0.2650f, 0.2740f, 0.7170f, 0.1670f, 0.0090f, 1.0/3.0f, 1.0/3.0f, 2.2f, 0);
RGBSpace RGBSpace::EBU(0.6400f, 0.3300f, 0.2900f, 0.6000f, 0.1500f, 0.0600f, 0.31271f, 0.32902f, 20.0f/9.0f, 0.018f);
RGBSpace RGBSpace::SMPTE_C(0.6300f, 0.3400f, 0.3100f, 0.5950f, 0.1550f, 0.0700f, 0.31271f, 0.32902f, 20.0f/9.0f, 0.018f);
RGBSpace RGBSpace::SMPTE_240M(0.6300f, 0.3400f, 0.3100f, 0.5950f, 0.1550f, 0.0700f, 0.31271f, 0.32902f, 20.0f/9.0f, 0.018f);
RGBSpace RGBSpace::WIDE_GAMUT(0.7347f, 0.2653f, 0.1152f, 0.8264f, 0.1566f, 0.0177f, 0.3457f, 0.3585f, 2.2f, 0);

RGBSpace::RGBSpace(void)
{
	gamma=breakPoint=0.0f;
	slope=slopeMatch=segmentOffset=0.0f;
	xr=yr=zr=xg=yg=zg=xb=yb=zb=0.0f;
	xw=yw=zw=0.0f;
	rx=ry=rz=gx=gy=gz=bx=by=bz=0.0f;
    rw=gw=zw=0.0f;
}

RGBSpace::RGBSpace(const RGBSpace&rs)
{
	if(this==&rs) return;

	gamma=rs.gamma;
	breakPoint=rs.breakPoint;
	slope=rs.slope;
	slopeMatch=rs.slopeMatch;
	segmentOffset=rs.segmentOffset;
	xr=rs.xr;
	yr=rs.yr;
	zr=rs.zr;
	xg=rs.xg;
	yg=rs.yg;
	zg=rs.zg;
	xb=rs.xb;
	yb=rs.yb;
	zb=rs.zb;
	xw=rs.xw;
	yw=rs.yw;
	zw=rs.zw;
	rx=rs.rx;
	ry=rs.ry;
	rz=rs.rz;
	gx=rs.gx;
	gy=rs.gy;
	gz=rs.gz;
	bx=rs.bx;
	by=rs.by;
	bz=rs.bz;
	rw=rs.rw;	
	gw=rs.gw;
	bw=rs.bw;
	GAMMA_CURVE=rs.GAMMA_CURVE;
	INV_GAMMA_CURVE=rs.INV_GAMMA_CURVE;
}

RGBSpace::~RGBSpace(void)
{
}

RGBSpace::RGBSpace(float _xRed,float _yRed,float _xGreen,float _yGreen,float _xBlue, 
	float _yBlue,float _xWhite,float _yWhite,float _gamma,float _breakPoint) 
{
    set(_xRed,_yRed,_xGreen,_yGreen,_xBlue,
		_yBlue,_xWhite,_yWhite,_gamma,_breakPoint);
}

void RGBSpace::set(float _xRed,float _yRed,float _xGreen,float _yGreen,float _xBlue, 
				   float _yBlue,float _xWhite,float _yWhite,float _gamma,float _breakPoint)
{
	gamma=_gamma;
	breakPoint=_breakPoint;

	if(_breakPoint>0.0f) 
	{
		slope=1.0f/(_gamma/(float)pow(_breakPoint,1.0f/_gamma-1.0f) 
			-_gamma*_breakPoint+_breakPoint);
		slopeMatch=_gamma*slope/(float)pow(_breakPoint,1.0f/_gamma-1.0f);
		segmentOffset=slopeMatch*(float)pow(_breakPoint,1.0f/_gamma)-slope*_breakPoint;
	} 
	else 
	{
		slope=1.0f;
		slopeMatch=1.0f;
		segmentOffset=0.0f;
	}

	GAMMA_CURVE.resize(256);
	INV_GAMMA_CURVE.resize(256);
	for(int i=0; i<256; i++) 
	{
		float c=i/255.0f;
		GAMMA_CURVE[i]=MathUtil::clamp((int)(gammaCorrect(c)*255+0.5f),0,255);
		INV_GAMMA_CURVE[i]=MathUtil::clamp((int)(ungammaCorrect(c)*255+0.5f),0,255);
	}

	float _xr=_xRed;
	float _yr=_yRed;
	float _zr=1.0f-(_xr+_yr);
	float _xg=_xGreen;
	float _yg=_yGreen;
	float _zg=1.0f-(_xg+_yg);
	float _xb=_xBlue;
	float _yb=_yBlue;
	float _zb=1.0f-(_xb+_yb);

	xw=_xWhite;
	yw=_yWhite;
	zw=1.0f-(xw+yw);

	float _rx=(_yg*_zb)-(_yb*_zg);
	float _ry=(_xb*_zg)-(_xg*_zb);
	float _rz=(_xg*_yb)-(_xb*_yg);
	float _gx=(_yb*_zr)-(_yr*_zb);
	float _gy=(_xr*_zb)-(_xb*_zr);
	float _gz=(_xb*_yr)-(_xr*_yb);
	float _bx=(_yr*_zg)-(_yg*_zr);
	float _by=(_xg*_zr)-(_xr*_zg);
	float _bz=(_xr*_yg)-(_xg*_yr);

	rw=((_rx*xw)+(_ry*yw)+(_rz*zw))/yw;
	gw=((_gx*xw)+(_gy*yw)+(_gz*zw))/yw;
	bw=((_bx*xw)+(_by*yw)+(_bz*zw))/yw;

	rx=_rx/rw;
	ry=_ry/rw;
	rz=_rz/rw;
	gx=_gx/gw;
	gy=_gy/gw;
	gz=_gz/gw;
	bx=_bx/bw;
	by=_by/bw;
	bz=_bz/bw;

	float s=1.0f/(rx*(gy*bz-by*gz) 
		-ry*(gx*bz-bx*gz) 
		+rz*(gx*by-bx*gy));
	xr=s*(gy*bz-gz*by);
	xg=s*(rz*by-ry*bz);
	xb=s*(ry*gz-rz*gy);

	yr=s*(gz*bx-gx*bz);
	yg=s*(rx*bz-rz*bx);
	yb=s*(rz*gx-rx*gz);

	zr=s*(gx*by-gy*bx);
	zg=s*(ry*bx-rx*by);
	zb=s*(rx*gy-ry*gx);
}

void RGBSpace::convertXYZtoRGB(const XYZColor&c1,Color&c2) const
{
	convertXYZtoRGB(c1.getX(),c1.getY(),c1.getZ(),c2);
}

void RGBSpace::convertXYZtoRGB(float X,float Y,float Z,Color&c)const 
{
	float r=(rx*X)+(ry*Y)+(rz*Z);
	float g=(gx*X)+(gy*Y)+(gz*Z);
	float b=(bx*X)+(by*Y)+(bz*Z);
	c.set(r,g,b);
}

XYZColor RGBSpace::convertRGBtoXYZ(const Color&c) const
{
	vector<float> rgb=c.getRGB();
	float X=(xr*rgb[0])+(xg*rgb[1])+(xb*rgb[2]);
	float Y=(yr*rgb[0])+(yg*rgb[1])+(yb*rgb[2]);
	float Z=(zr*rgb[0])+(zg*rgb[1])+(zb*rgb[2]);

	return XYZColor(X,Y,Z);
}

BOOL RGBSpace::insideGamut(float r,float g,float b)const 
{
	return r>=0.0f && g>=0.0f && b>=0.0f;
}

float RGBSpace::gammaCorrect(float v) const
{
	if(v<=0.0f)
		return 0.0f;
	else if(v>=1.0f)
		return 1.0f;
	else if(v<=breakPoint)
		return slope*v;
	else
		return slopeMatch*(float)pow(v,1.0f/gamma)-segmentOffset;
}

float RGBSpace::ungammaCorrect(float vp)const
{
	if(vp<=0.0f)
		return 0.0f;
	else if (vp>=1.0f)
		return 1.0f;
	else if(vp<=breakPoint*slope)
		return vp/slope;
	else
		return (float)pow((vp+segmentOffset)/slopeMatch,gamma);
}

int RGBSpace::rgbToNonLinear(int rgb) const
{	
	int bp=GAMMA_CURVE[(rgb>>16)&0xFF];
	int gp=GAMMA_CURVE[(rgb>>8)&0xFF];
	int rp=GAMMA_CURVE[rgb&0xFF];

	return (bp<<16) | (gp<<8) | rp;
}

int RGBSpace::rgbToLinear(int rgb) const
{	
	int bp=INV_GAMMA_CURVE[(rgb>>16)&0xFF];
	int gp=INV_GAMMA_CURVE[(rgb>>8)&0xFF];
	int rp=INV_GAMMA_CURVE[rgb&0xFF];

	return (bp<<16) | (gp<<8) | rp;
}

byte RGBSpace::rgbToNonLinear(byte r)const 
{
	return (byte)GAMMA_CURVE[r&0xFF];
}

byte RGBSpace::rgbToLinear(byte r)const
{
	return (byte)INV_GAMMA_CURVE[r&0xFF];
}

CString RGBSpace::toString() const
{
	CString info="Gamma函数参数:\n";
	CString str;
	str.Format(_T("  * Gamma:      %7.4f\n"),gamma);
	info+=str;
	str.Format(_T("  * 分割点:     %7.4f\n"),breakPoint);
	info+=str;
	str.Format(_T("  * 斜度:       %7.4f\n"),slope);
	info+=str;
	str.Format(_T("  * 斜度配比:   %7.4f\n"),slopeMatch);
	info+=str;
	str.Format(_T("  * 段偏移:     %7.4f\n"),segmentOffset);
	info+=str;
	info+="XYZ -> RGB 矩阵:\n";
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),rx,ry,rz);
	info+=str;
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),gx,gy,gz);
	info+=str;
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),bx,by,bz);
	info+=str;
	info+="RGB -> XYZ 矩阵:\n";
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),xr,xg,xb);
	info+=str;
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),yr,yg,yb);
	info+=str;
	str.Format(_T("| %7.4f %7.4f %7.4f|\n"),zr,zg,zb);
	info+=str;

	return info;
}

RGBSpace& RGBSpace::operator=(const RGBSpace&rs)
{
	if(this==&rs) return *this;

    gamma=rs.gamma;
	breakPoint=rs.breakPoint;
    slope=rs.slope;
	slopeMatch=rs.slopeMatch;
    segmentOffset=rs.segmentOffset;
    xr=rs.xr;
	yr=rs.yr;
	zr=rs.zr;
	xg=rs.xg;
	yg=rs.yg;
    zg=rs.zg;
	xb=rs.xb;
	yb=rs.yb;
	zb=rs.zb;
	xw=rs.xw;
	yw=rs.yw;
	zw=rs.zw;
	rx=rs.rx;
	ry=rs.ry;
	rz=rs.rz;
	gx=rs.gx;
	gy=rs.gy;
	gz=rs.gz;
	bx=rs.bx;
	by=rs.by;
	bz=rs.bz;
    rw=rs.rw;	
	gw=rs.gw;
	bw=rs.bw;
    GAMMA_CURVE=rs.GAMMA_CURVE;
	INV_GAMMA_CURVE=rs.INV_GAMMA_CURVE;

	return *this;
}