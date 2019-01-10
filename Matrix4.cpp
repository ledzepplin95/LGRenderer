#include "StdAfx.h"

#include "Matrix4.h"
#include "strutil.h"
#include "BoundingBox.h"
#include "point3.h"
#include "OrthoNormalBasis.h"

const Matrix4 Matrix4::ZERO;
const Matrix4 Matrix4::IDENTITY=Matrix4::scale(1.0f);

Matrix4::Matrix4(void)
{
	m00=0.0f;
	m01=0.0f;
	m02=0.0f;
	m03=0.0f;

    m10=0.0f;
	m11=0.0f;
	m12=0.0f;
	m13=0.0f;

	m20=0.0f;
	m21=0.0f;
	m22=0.0f;
	m23=0.0f;
}

Matrix4::~Matrix4(void)
{
}

Matrix4& Matrix4::SetToIdentity()
{
	m00=1.0f;
	m01=0.0f;
	m02=0.0f;
	m03=0.0f;

	m10=0.0f;
	m11=1.0f;
	m12=0.0f;
	m13=0.0f;

	m20=0.0f;
	m21=0.0f;
	m22=1.0f;
	m23=0.0f;

	return *this;
}

Matrix4::Matrix4(float mm00,float mm01,float mm02,
				 float mm03,float mm10,float mm11,
				 float mm12,float mm13,float mm20, 
				 float mm21,float mm22,float mm23) 
{
	m00=mm00;
	m01=mm01;
	m02=mm02;
	m03=mm03;
	m10=mm10;
	m11=mm11;
	m12=mm12;
	m13=mm13;
	m20=mm20;
	m21=mm21;
	m22=mm22;
	m23=mm23;
}

Matrix4::Matrix4(float m[],BOOL rowMajor)
{
    set(m,rowMajor);
}

void Matrix4::set(float mm00,float mm01,float mm02,
				  float mm03,float mm10,float mm11,
				  float mm12,float mm13,float mm20, 
				  float mm21,float mm22,float mm23)
{
	m00=mm00;
	m01=mm01;
	m02=mm02;
	m03=mm03;
	m10=mm10;
	m11=mm11;
	m12=mm12;
	m13=mm13;
	m20=mm20;
	m21=mm21;
	m22=mm22;
	m23=mm23;
}

void Matrix4::set(float m[],BOOL rowMajor)
{
	if(rowMajor) 
	{
		m00=m[0];
		m01=m[1];
		m02=m[2];
		m03=m[3];
		m10=m[4];
		m11=m[5];
		m12=m[6];
		m13=m[7];
		m20=m[8];
		m21=m[9];
		m22=m[10];
		m23=m[11];	
	} 
	else 
	{
		m00=m[0];
		m01=m[4];
		m02=m[8];
		m03=m[12];
		m10=m[1];
		m11=m[5];
		m12=m[9];
		m13=m[13];
		m20=m[2];
		m21=m[6];
		m22=m[10];
		m23=m[14];	
	}
}

BOOL Matrix4::isIndentity() const
{
	return equals(IDENTITY);
}

BOOL Matrix4::equals(const Matrix4&m) const
{
	if(m==Matrix4()) return FALSE;
	if(this==&m) return TRUE;

	return m00==m.m00 && m01==m.m01 && m02==m.m02 
		&& m03==m.m03 && m10==m.m10 && m11==m.m11 
		&& m12==m.m12 && m13==m.m13 && m20==m.m20 
		&& m21==m.m21 && m22==m.m22 && m23==m.m23;
}

vector<float> Matrix4::asRowMajor() const
{
    vector<float> m;
	m.resize(12);
	m[0]=m00; m[1]=m01; m[2]=m02; m[3]=m03;
    m[4]=m10; m[5]=m11; m[6]=m12; m[7]=m13;
	m[8]=m20; m[9]=m21; m[10]=m22; m[11]=m23;	

	return m;
}

vector<float> Matrix4::asColMajor() const
{
	vector<float> m;
	m.resize(12);
	m[0]=m00; m[1]=m10; m[2]=m20; m[3]=0.0f;
	m[4]=m01; m[5]=m11; m[6]=m21; m[7]=0.0f;
	m[8]=m02; m[9]=m12; m[10]=m22; m[11]=0.0f;	
	
	return m;
}

float Matrix4::determinant() const
{
	float A0=m00*m11-m01*m10;
	float A1=m00*m12-m02*m10;
	float A3=m01*m12-m02*m11;

	return A0*m22-A1*m21+A3*m20;
}

Matrix4 Matrix4::inverse() const
{
	float A0=m00*m11-m01*m10;
	float A1=m00*m12-m02*m10;
	float A3=m01*m12-m02*m11;
	float det=A0*m22-A1*m21+A3*m20;
	if( fabs(det)<1e-12f )
		return IDENTITY;

	float invDet=1.0f/det;
	float A2=m00*m13-m03*m10;
	float A4=m01*m13-m03*m11;
	float A5=m02*m13-m03*m12;
	Matrix4 inv;
	inv.m00=(+m11*m22-m12*m21)*invDet;
	inv.m10=(-m10*m22+m12*m20)*invDet;
	inv.m20=(+m10*m21-m11*m20)*invDet;
	inv.m01=(-m01*m22+m02*m21)*invDet;
	inv.m11=(+m00*m22-m02*m20)*invDet;
	inv.m21=(-m00*m21+m01*m20)*invDet;
	inv.m02=+A3*invDet;
	inv.m12=-A1*invDet;
	inv.m22=+A0*invDet;
	inv.m03=(-m21*A5+m22*A4-m23*A3)*invDet;
	inv.m13=(+m20*A5-m22*A2+m23*A1)*invDet;
	inv.m23=(-m20*A4+m21*A2-m23*A0)*invDet;

	return inv;
}


Matrix4 Matrix4::multiply(const Matrix4&m) const
{
	float rm00=m00*m.m00+m01*m.m10+m02*m.m20;
	float rm01=m00*m.m01+m01*m.m11+m02*m.m21;
	float rm02=m00*m.m02+m01*m.m12+m02*m.m22;
	float rm03=m00*m.m03+m01*m.m13+m02*m.m23+m03;

	float rm10=m10*m.m00+m11*m.m10+m12*m.m20;
	float rm11=m10*m.m01+m11*m.m11+m12*m.m21;
	float rm12=m10*m.m02+m11*m.m12+m12*m.m22;
	float rm13=m10*m.m03+m11*m.m13+m12*m.m23+m13;

	float rm20=m20*m.m00+m21*m.m10+m22*m.m20;
	float rm21=m20*m.m01+m21*m.m11+m22*m.m21;
	float rm22=m20*m.m02+m21*m.m12+m22*m.m22;
	float rm23=m20*m.m03+m21*m.m13+m22*m.m23+m23;

	return Matrix4(rm00,rm01,rm02,rm03,rm10,rm11,
		rm12,rm13,rm20,rm21,rm22,rm23);
}

BoundingBox Matrix4::transform(const BoundingBox&b) const
{
	if(b.isEmpty())
		return BoundingBox(0.0f);	
	BoundingBox rb(transformP(b.getMinimum()));
	rb.include(transformP(b.getMaximum()));
	for(int i=1; i<7; i++)
		rb.include(transformP(b.getCorner(i)));

	return rb;
}

Vector3 Matrix4::transformV(const Vector3&v) const
{
	Vector3 rv;
	rv.x=m00*v.x+m01*v.y+m02*v.z;
	rv.y=m10*v.x+m11*v.y+m12*v.z;
	rv.z=m20*v.x+m21*v.y+m22*v.z;

	return rv;
}

Vector3 Matrix4::transformTransposeV(const Vector3&v) const
{
	Vector3 rv;
	rv.x=m00*v.x+m10*v.y+m20*v.z;
	rv.y=m01*v.x+m11*v.y+m21*v.z;
	rv.z=m02*v.x+m12*v.y+m22*v.z;

	return rv;
}

Point3 Matrix4::transformP(const Point3&p) const
{
	Point3 rp;
	rp.x=m00*p.x+m01*p.y+m02*p.z+m03;
	rp.y=m10*p.x+m11*p.y+m12*p.z+m13;
	rp.z=m20*p.x+m21*p.y+m22*p.z+m23;

	return rp;
}

float Matrix4::transformVX(float x,float y,float z) const
{
	return m00*x+m01*y+m02*z;
}

float Matrix4::transformVY(float x,float y,float z) const
{
	return m10*x+m11*y+m12*z;
}

float Matrix4::transformVZ(float x,float y,float z) const
{
	return m20*x+m21*y+m22*z;
}

float Matrix4::transformTransposeVX(float x,float y,float z) const
{
	return m00*x+m10*y+m20*z;
}

float Matrix4::transformTransposeVY(float x,float y,float z) const
{
	return m01*x+m11*y+m21*z;
}

float Matrix4::transformTransposeVZ(float x,float y,float z) const
{
	return m02*x+m12*y+m22*z;
}

float Matrix4::transformPX(float x,float y,float z) const
{
	return m00*x+m01*y+m02*z+m03;
}

float Matrix4::transformPY(float x,float y,float z) const
{
	return m10*x+m11*y+m12*z+m13;
}

float Matrix4::transformPZ(float x,float y,float z) const
{
	return m20*x+m21*y+m22*z+m23;
}

Matrix4 Matrix4::translation(float x,float y,float z) 
{
	Matrix4 m;
	m.m00=m.m11=m.m22=1.0f;
	m.m03=x;
	m.m13=y;	
	m.m23=z;

	return m;
}

Matrix4 Matrix4::rotateX(float theta) 
{
	Matrix4 m;
	float s=(float)sin(theta);
	float c=(float)cos(theta);
	m.m00=1.0f;
	m.m11=m.m22=c;
	m.m12=-s;
	m.m21=+s;

	return m;
}

Matrix4 Matrix4::rotateY(float theta) 
{
	Matrix4 m;
	float s=(float)sin(theta);
	float c=(float)cos(theta);
	m.m11=1.0f;
	m.m00=m.m22=c;
	m.m02=+s;
	m.m20=-s;

	return m;
}

Matrix4 Matrix4::rotateZ(float theta) 
{
	Matrix4 m;
	float s=(float)sin(theta);
	float c=(float)cos(theta);
	m.m22=1.0f;
	m.m00=m.m11=c;
	m.m01=-s;
	m.m10=+s;

	return m;
}

Matrix4 Matrix4::rotate(float x,float y,float z,float theta) 
{
	Matrix4 m;
	float invLen=1.0f/sqrt(x*x+y*y+z*z);
	x*=invLen;
	y*=invLen;
	z*=invLen;
	float s=(float)sin(theta);
	float c=(float)cos(theta);
	float t=1.0f-c;
	m.m00=t*x*x+c;
	m.m11=t*y*y+c;
	m.m22=t*z*z+c;
	float txy=t*x*y;
	float sz=s*z;
	m.m01=txy-sz;
	m.m10=txy+sz;
	float txz=t*x*z;
	float sy=s*y;
	m.m02=txz+sy;
	m.m20=txz-sy;
	float tyz=t*y*z;
	float sx=s*x;
	m.m12=tyz-sx;
	m.m21=tyz+sx;

	return m;
}

Matrix4 Matrix4::scale(float s) 
{
	Matrix4 m;
	m.m00=m.m11=m.m22=s;

	return m;
}

Matrix4 Matrix4::scale(float sx,float sy,float sz) 
{
	Matrix4 m;
	m.m00=sx;
	m.m11=sy;
	m.m22=sz;

	return m;
}

Matrix4& Matrix4::operator=(const Matrix4&m)
{
	if(this==&m) return *this;

	m00=m.m00;
    m01=m.m01;
    m02=m.m02;
	m03=m.m03;
	m10=m.m10;
	m11=m.m11;
	m12=m.m12;
	m13=m.m13;
	m20=m.m20;
	m21=m.m21;
	m22=m.m22;
	m23=m.m23;

	return *this;
}

BOOL Matrix4::operator!=(const Matrix4&m) const
{
	const float eps=1.0e-6;

	if(fabs(m00-m.m00)>eps) return TRUE;
	if(fabs(m01-m.m01)>eps) return TRUE;
	if(fabs(m02-m.m02)>eps) return TRUE;
	if(fabs(m03-m.m03)>eps) return TRUE;
	if(fabs(m10-m.m10)>eps) return TRUE;
	if(fabs(m11-m.m11)>eps) return TRUE;
	if(fabs(m12-m.m12)>eps) return TRUE;
	if(fabs(m13-m.m13)>eps) return TRUE;
	if(fabs(m20-m.m20)>eps) return TRUE;
	if(fabs(m21-m.m21)>eps) return TRUE;
	if(fabs(m22-m.m22)>eps) return TRUE;
	if(fabs(m23-m.m23)>eps) return TRUE;

	return FALSE;
}

BOOL Matrix4::operator ==(const Matrix4&m)const
{
	const float eps=1.0e-6;

	if(fabs(m00-m.m00)>eps) return FALSE;
	if(fabs(m01-m.m01)>eps) return FALSE;
	if(fabs(m02-m.m02)>eps) return FALSE;
	if(fabs(m03-m.m03)>eps) return FALSE;
	if(fabs(m10-m.m10)>eps) return FALSE;
	if(fabs(m11-m.m11)>eps) return FALSE;
	if(fabs(m12-m.m12)>eps) return FALSE;
	if(fabs(m13-m.m13)>eps) return FALSE;
	if(fabs(m20-m.m20)>eps) return FALSE;
	if(fabs(m21-m.m21)>eps) return FALSE;
	if(fabs(m22-m.m22)>eps) return FALSE;
	if(fabs(m23-m.m23)>eps) return FALSE;

	return TRUE;
}

Matrix4 Matrix4::fromBasis(const OrthoNormalBasis&basis) 
{
	Matrix4 m;
	Vector3 u=basis.transform(Vector3(1,0,0));
	Vector3 v=basis.transform(Vector3(0,1,0));
	Vector3 w=basis.transform(Vector3(0,0,1));
	m.m00=u.x;
	m.m01=v.x;
	m.m02=w.x;
	m.m10=u.y;
	m.m11=v.y;
	m.m12=w.y;
	m.m20=u.z;
	m.m21=v.z;
	m.m22=w.z;

	return m;
}

Matrix4 Matrix4::lookAt(const Point3&eye,const Point3&target,const Vector3&up) 
{
	Matrix4 m=Matrix4::fromBasis(OrthoNormalBasis::makeFromWV(
		Point3::sub(eye,target,Vector3()),up));

	return Matrix4::translation(eye.x,eye.y,eye.z).multiply(m);
}

Matrix4 Matrix4::blend(const Matrix4&m0,const Matrix4&m1,float t) 
{
	Matrix4 m;
	m.m00=(1.0f-t)*m0.m00+t*m1.m00;
	m.m01=(1.0f-t)*m0.m01+t*m1.m01;
	m.m02=(1.0f-t)*m0.m02+t*m1.m02;
	m.m03=(1.0f-t)*m0.m03+t*m1.m03;

	m.m10=(1.0f-t)*m0.m10+t*m1.m10;
	m.m11=(1.0f-t)*m0.m11+t*m1.m11;
	m.m12=(1.0f-t)*m0.m12+t*m1.m12;
	m.m13=(1.0f-t)*m0.m13+t*m1.m13;

	m.m20=(1.0f-t)*m0.m20+t*m1.m20;
	m.m21=(1.0f-t)*m0.m21+t*m1.m21;
	m.m22=(1.0f-t)*m0.m22+t*m1.m22;
	m.m23=(1.0f-t)*m0.m23+t*m1.m23;

	return m;
}
