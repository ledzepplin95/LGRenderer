#include "StdAfx.h"

#include "Vector3.h"
#include "LGDef.h"

class vinitVector3
{
public:
	vinitVector3(void);		
};
static vinitVector3 ini;

float Vector3::COS_THETA[256]={0.0};
float Vector3::SIN_THETA[256]={0.0};
float Vector3::COS_PHI[256]={0.0};
float Vector3::SIN_PHI[256]={0.0};

vinitVector3::vinitVector3()
{
	Vector3::initial();
}

Vector3::Vector3(void)
{	
    x=y=z=0.0f;
}

Vector3::~Vector3(void)
{
}

Vector3::Vector3(float xx,float yy,float zz)
{
	x=xx;
	y=yy;
	z=zz;
}

Vector3::Vector3(const Vector3&v) 
{
	x=v.x;
	y=v.y;
	z=v.z;
}

Vector3 Vector3::decode(short n,Vector3&dest)
{
	int t=(n&0xFF00)>>8;
	int p=n&0xFF;
	dest.x=SIN_THETA[t]*COS_PHI[p];
	dest.y=SIN_THETA[t]*SIN_PHI[p];
	dest.z=COS_THETA[t];	

	return dest;
}

Vector3 Vector3::decode(short n) 
{
	return decode(n,Vector3());
}

short Vector3::encode() const
{
	int theta=(int)(acos(z)*(256.0/LG_PI));
	if(theta>255)
		theta=255;
	int phi=(int) (atan2(y,x)*(128.0/LG_PI));
	if(phi<0)
		phi+=256;
	else if(phi>255)
		phi=255;

	return (short)( ((theta&0xFF)<<8) | (phi&0xFF) );
}

float Vector3::get(int i) const
{
	switch(i) 
	{
		case 0:
			return x;
		case 1:
			return y;
		default:
			return z;
	}
}

float Vector3::length() const
{
	return (float)sqrt((x*x)+(y*y)+(z*z));
}

float Vector3::lengthSquared() const
{
	return (x*x)+(y*y)+(z*z);
}

Vector3& Vector3::negate() 
{
	x=-x;
	y=-y;
	z=-z;

	return *this;
}

Vector3 Vector3::negate(Vector3&dest)
{
	dest.x=-x;
	dest.y=-y;
	dest.z=-z;

	return dest;
}

Vector3& Vector3::mul(float s) 
{
	x*=s;
	y*=s;
	z*=s;

	return *this;
}

Vector3 Vector3::mul(float s,Vector3&dest) 
{
	dest.x=x*s;
	dest.y=y*s;
	dest.z=z*s;

	return dest;
}

Vector3 Vector3::div(float d) 
{
	x/=d;
	y/=d;
	z/=d;

	return *this;
}

Vector3 Vector3::div(float d,Vector3&dest) const
{
	dest.x=x/d;
	dest.y=y/d;
	dest.z=z/d;

	return dest;
}

float Vector3::normalizeLength() 
{
	float n=(float)sqrt(x*x+y*y+z*z);
	float inside=1.0f/n;
	x*=inside;
	y*=inside;
	z*=inside;

	return n;
}

Vector3& Vector3::normalize() 
{
	float inside=1.0f/sqrt((x*x)+(y*y)+(z*z));
	x*=inside;
	y*=inside;
	z*=inside;

	return *this;
}

Vector3 Vector3::normalize(Vector3&dest) const
{
	float inside=1.0f/sqrt((x*x)+(y*y)+(z*z));
	dest.x=x*inside;
	dest.y=y*inside;
	dest.z=z*inside;

	return dest;
}

Vector3& Vector3::set(float xx,float yy,float zz) 
{
	x=xx;
	y=yy;
	z=zz;

	return *this;
}

Vector3& Vector3::set(const Vector3&v) 
{
	x=v.x;
	y=v.y;
	z=v.z;

	return *this;
}

float Vector3::dot(float vx,float vy,float vz)const
{
	return vx*x+vy*y+vz*z;
}

float Vector3::dot(const Vector3&v1,const Vector3&v2)
{
	return (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
}

Vector3 Vector3::cross(const Vector3&v1,const Vector3&v2,Vector3&dest)
{
	dest.x=(v1.y*v2.z)-(v1.z*v2.y);
	dest.y=(v1.z*v2.x)-(v1.x*v2.z);
	dest.z=(v1.x*v2.y)-(v1.y*v2.x);

	return dest;
}

Vector3 Vector3::add(const Vector3&v1,const Vector3&v2,Vector3&dest)
{
	dest.x=v1.x+v2.x;
	dest.y=v1.y+v2.y;
	dest.z=v1.z+v2.z;

	return dest;
}

Vector3 Vector3::sub(const Vector3&v1,const Vector3&v2,Vector3&dest) 
{
	dest.x=v1.x-v2.x;
	dest.y=v1.y-v2.y;
	dest.z=v1.z-v2.z;

	return dest;
}

CString Vector3::toString()const 
{
	CString str;
	str.Format(_T("(%.2f %.2f %.2f)"),x,y,z);

	return str;
}

void Vector3::initial()
{
	for(int i=0; i<256; i++) 
	{
		double angle=(i*LG_PI)/256.0;
		Vector3::COS_THETA[i]=(float)cos(angle);
		SIN_THETA[i]=(float)sin(angle);
		COS_PHI[i]=(float)cos(2.0*angle);
		SIN_PHI[i]=(float)sin(2.0*angle);
	}
}

Vector3& Vector3::operator =(const Vector3&v)
{
	if(this==&v) return *this;

	x=v.x;
	y=v.y;
	z=v.z;

	return *this;
}


BOOL Vector3::operator !=(const Vector3 &v) const
{
	const float tol=1.0e-6f;
	if(fabs(x-v.x)>tol) return TRUE;
	if(fabs(y-v.y)>tol) return TRUE;
	if(fabs(z-v.z)>tol) return TRUE;

	return FALSE;
}

BOOL Vector3::operator==(const Vector3&v)const
{
	const float tol=1.0e-6f;
	if(fabs(x-v.x)>tol) return FALSE;
	if(fabs(y-v.y)>tol) return FALSE;
	if(fabs(z-v.z)>tol) return FALSE;

	return TRUE;
}