#include "StdAfx.h"

#include "Color.h"
#include "mathutil.h"
#include <boost/math/special_functions/fpclassify.hpp>

class vinitColor
{
public:
	vinitColor(void);		
};

RGBSpace Color::NATIVE_SPACE(0.6400f,0.3300f,0.3000f,0.6000f,0.1500f,0.0600f,0.31271f,0.32902f,2.4f,0.00304f);
Color Color::BLACK(0,0,0);
Color Color::WHITE(1,1,1);
Color Color::RED(1,0,0);
Color Color::GREEN(0,1,0);
Color Color::BLUE(0,0,1);
Color Color::YELLOW(1,1,0);
Color Color::CYAN(0,1,1);
Color Color::MAGENTA(1,0,1);
Color Color::GRAY(0.5f,0.5f,0.5f);
float Color::EXPONENT[256]={0.0f};
static vinitColor ini;

vinitColor::vinitColor()
{
	Color::initial();
}

Color::Color(void)
{
	r=g=b=0.0f;
}

Color::~Color(void)
{
}

void Color::initial()
{
	EXPONENT[0]=0.0f;
	for(int i=1; i<256; i++) 
	{
		float f=1.0f;
		int e=i-(128+8);
		if(e>0)
			for(int j=0; j<e; j++)
				f*=2.0f;
		else
			for(int j=0; j<-e; j++)
				f*=0.5f;
		EXPONENT[i]=f;
	}
}

Color::Color(float gray)
{
	r=g=b=gray;
}

Color::Color(float rr,float gg,float bb) 
{
	r=rr;
	g=gg;
	b=bb;
}

Color::Color(const Color&c)
{
	if(this==&c) return;
	r=c.r;
	g=c.g;
	b=c.b;
}

Color::Color(int rgb) 
{
	b=((rgb>>16)&0xFF)/255.0f;
	g=((rgb>>8)&0xFF)/255.0f;
	r=(rgb&0xFF)/255.0f;
}

Color& Color::toNonLinear()
{
	r=NATIVE_SPACE.gammaCorrect(r);
	g=NATIVE_SPACE.gammaCorrect(g);
	b=NATIVE_SPACE.gammaCorrect(b);

	return *this;
}

Color& Color::toLinear() 
{
	r=NATIVE_SPACE.ungammaCorrect(r);
	g=NATIVE_SPACE.ungammaCorrect(g);
	b=NATIVE_SPACE.ungammaCorrect(b);

	return *this;
}

Color Color::copy() const
{
	return Color(*this);
}

Color& Color::set(float rr,float gg,float bb) 
{
	r=rr;
	g=gg;
	b=bb;

	return *this;
}

Color& Color::set(const Color&c) 
{
	r=c.r;
	g=c.g;
	b=c.b;

	return *this;
}

Color& Color::setRGB(int rgb) 
{
	b=((rgb>>16)&0xFF)/255.0f;
	g=((rgb>>8)&0xFF)/255.0f;
	r=(rgb&0xFF)/255.0f;

	return *this;
}

Color& Color::setRGBE(int rgbe) 
{
	float f=EXPONENT[rgbe&0xFF];
	b=f*((rgbe>>24)+0.5f);
	g=f*(((rgbe>>16)&0xFF)+0.5f);
	r=f*(((rgbe>>8)&0xFF)+0.5f);

	return *this;
}

BOOL Color::isBlack() const
{
	return r<=0.0f && g<=0.0f && b<=0.0f;
}

float Color::getLuminance() const
{
	return (0.2989f*r)+(0.5866f*g)+(0.1145f*b);
}

float Color::getMin() const
{
	return MathUtil::mu_min(r,g,b);
}

float Color::getMax() const
{
	return MathUtil::mu_max(r,g,b);
}

float Color::getAverage() const
{
	return (r+g+b)/3.0f;
}

vector<float> Color::getRGB() const
{
	vector<float> cc;
    cc.push_back(r);
	cc.push_back(g); 
	cc.push_back(b);

	return cc;
}

int Color::toRGB() const
{
	int ir=(int)(r*255+0.5);
	int ig=(int)(g*255+0.5);
	int ib=(int)(b*255+0.5);
	ir=MathUtil::clamp(ir,0,255);
	ig=MathUtil::clamp(ig,0,255);
	ib=MathUtil::clamp(ib,0,255);

	return ir | (ig<<8) | (ib<<16);
}

int Color::toRGBA(float a) const 
{
	int ir=(int)(r*255+0.5);
	int ig=(int)(g*255+0.5);
	int ib=(int)(b*255+0.5);
	int ia=(int)(a*255+0.5);
	ir=MathUtil::clamp(ir,0,255);
	ig=MathUtil::clamp(ig,0,255);
	ib=MathUtil::clamp(ib,0,255);
	ia=MathUtil::clamp(ia,0,255);
	
	return  ir | (ig<<8) | (ib<<16) | (ia<<24);
}

int Color::toRGBE() const
{	
	float v=MathUtil::mu_max(r,g,b);
	if(v<1e-32f)
		return 0;

	float m=v;
	int e=0;
	if(v>1.0f) 
	{
		while(m>1.0f)
		{
			m*=0.5f;
			e++;
		}
	} 
	else if(v<=0.5f)
	{
		while(m<=0.5f) 
		{
			m*=2.0f;
			e--;
		}
	}
	v=(m*255.0f)/v;
	int c=e+128;
	c|=((int)(b*v)<<24);
	c|=((int)(g*v)<<16);
	c|=((int)(r*v)<<8);

	return c;
}

Color& Color::constrainRGB() 
{	
	float w=-MathUtil::mu_min(0,r,g,b);
	if(w>0.0f) 
	{
		r+=w;
		g+=w;
		b+=w;
	}

	return *this;
}

BOOL Color::isNan() const
{
	return boost::math::isnan(r) 
		|| boost::math::isnan(g) 
		|| boost::math::isnan(b);
}

BOOL Color::isInf() const
{
	return boost::math::isinf(r) 
		|| boost::math::isinf(g) 
		|| boost::math::isinf(b);
}

Color& Color::add(const Color&c) 
{
	r+=c.r;
	g+=c.g;
	b+=c.b;

	return *this;
}

Color Color::add(const Color&c1,const Color&c2)
{
	return add(c1,c2,Color());
}

Color Color::add(const Color&c1,const Color&c2,Color&dest) 
{
	dest.r=c1.r+c2.r;
	dest.g=c1.g+c2.g;
	dest.b=c1.b+c2.b;

	return dest;
}

Color& Color::madd(float s,const Color&c) 
{
	r+=(s*c.r);
	g+=(s*c.g);
	b+=(s*c.b);

	return *this;
}

Color& Color::madd(const Color&s,const Color&c) 
{
	r+=s.r*c.r;
	g+=s.g*c.g;
	b+=s.b*c.b;

	return *this;
}

Color& Color::sub(const Color&c)
{
	r-=c.r;
	g-=c.g;
	b-=c.b;

	return *this;
}

Color Color::sub(const Color&c1,const Color&c2) 
{
	return sub(c1,c2,Color());
}

Color Color::sub(const Color&c1,const Color&c2,Color&dest) 
{
	dest.r=c1.r-c2.r;
	dest.g=c1.g-c2.g;
	dest.b=c1.b-c2.b;

	return dest;
}

Color& Color::mul(const Color&c)
{
	r*=c.r;
	g*=c.g;
	b*=c.b;

	return *this;
}

Color Color::mul(const Color&c1,const Color&c2) 
{
	return mul(c1,c2,Color());
}

Color Color::mul(const Color&c1,const Color&c2,Color&dest)
{
	dest.r=c1.r*c2.r;
	dest.g=c1.g*c2.g;
	dest.b=c1.b*c2.b;

	return dest;
}

Color& Color::mul(float s)
{
	r*=s;
	g*=s;
	b*=s;

	return *this;
}

Color Color::mul(float s,const Color&c)
{
	return mul(s,c,Color());
}

Color Color::mul(float s,const Color&c,Color&dest) 
{
	dest.r=s*c.r;
	dest.g=s*c.g;
	dest.b=s*c.b;

	return dest;
}

Color& Color::div(const Color&c) 
{
	r/=c.r;
	g/=c.g;
	b/=c.b;

	return *this;
}

Color Color::div(const Color&c1,const Color&c2) 
{
	return div(c1,c2,Color());
}

Color Color::div(const Color&c1,const Color&c2,Color&dest) 
{
	dest.r=c1.r/c2.r;
	dest.g=c1.g/c2.g;
	dest.b=c1.b/c2.b;

	return dest;
}

Color& Color::exp() 
{
	r=(float)::exp(r);
	g=(float)::exp(g);
	b=(float)::exp(b);

	return *this;
}

Color& Color::opposite() 
{
	r=1.0f-r;
	g=1.0f-g;
	b=1.0f-b;

	return *this;
}

Color& Color::clamp(float minimum,float maximum) 
{
	r=MathUtil::clamp(r,minimum,maximum);
	g=MathUtil::clamp(g,minimum,maximum);
	b=MathUtil::clamp(b,minimum,maximum);

	return *this;
}

Color Color::blend(const Color&c1,const Color&c2,float b)
{
	return blend(c1,c2,b,Color());
}

Color Color::blend(const Color&c1,const Color&c2,float b,Color&dest)
{
	dest.r=(1.0f-b)*c1.r+b*c2.r;
	dest.g=(1.0f-b)*c1.g+b*c2.g;
	dest.b=(1.0f-b)*c1.b+b*c2.b;

	return dest;
}

Color Color::blend(const Color&c1,const Color&c2,const Color&b) 
{
	return blend(c1,c2,b,Color());
}

Color Color::blend(const Color&c1,const Color&c2,const Color&b,Color&dest) 
{
	dest.r=(1.0f-b.r)*c1.r+b.r*c2.r;
	dest.g=(1.0f-b.g)*c1.g+b.g*c2.g;
	dest.b=(1.0f-b.b)*c1.b+b.b*c2.b;

	return dest;
}

BOOL Color::hasContrast(const Color&c1,const Color&c2,float thresh) 
{
	if(fabs(c1.r-c2.r)/(c1.r+c2.r)>thresh)
		return TRUE;
	if(fabs(c1.g-c2.g)/(c1.g+c2.g)>thresh)
		return TRUE;
	if(fabs(c1.b-c2.b)/(c1.b+c2.b)>thresh)
		return TRUE;

	return FALSE;
}

CString Color::toString()const 
{
	CString str;
	str.Format(_T("(%.3f, %.3f, %.3f)"),r,g,b);

	return str;
}

BOOL Color::operator==(const Color&c)const
{
	const float tol=1.0e-6f;
	if(fabs(r-c.r)>tol) return FALSE;
	if(fabs(g-c.g)>tol) return FALSE;
	if(fabs(b-c.b)>tol) return FALSE;

	return TRUE;
}

BOOL Color::operator !=(const Color&c) const
{
	const float tol=1.0e-6f;
	if(fabs(r-c.r)>tol) return TRUE;
	if(fabs(g-c.g)>tol) return TRUE;
	if(fabs(b-c.b)>tol) return TRUE;

	return FALSE;
}

Color& Color::operator =(const Color&c)
{
	if(this==&c) return *this;

    r=c.r;
	g=c.g;
	b=c.b;

	return *this;
}

float Color::getRed()const
{
	return r;
}

float Color::getGreen() const
{
	return g;
}

float Color::getBlue() const
{
	return b;
}