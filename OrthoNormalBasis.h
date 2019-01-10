#pragma once

#include "vector3.h"

class OrthoNormalBasis
{
public:	
	OrthoNormalBasis(void);
	~OrthoNormalBasis(void);

	void flipU();
	void flipV();
	void flipW();
	void swapUV();
	void swapVW();
	void swapWU();
	OrthoNormalBasis& operator=(const OrthoNormalBasis&onb);

    Vector3 transform(const Vector3&a,Vector3&dest)const;
	Vector3 transform(Vector3&a)const;
	Vector3 untransform(const Vector3&a,Vector3&dest)const;
	Vector3 untransform(Vector3&a)const;
	float untransformX(const Vector3&a)const;
	float untransformY(const Vector3&a)const;
	float untransformZ(const Vector3&a)const;
	BOOL operator==(const OrthoNormalBasis&onb)const;
	BOOL operator!=(const OrthoNormalBasis&onb)const;

	static OrthoNormalBasis makeFromW(const Vector3&vw);
	static OrthoNormalBasis makeFromWV(const Vector3&w,const Vector3&v);

private:
	Vector3 u,v,w;
};
