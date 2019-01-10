#pragma once

class Matrix4
{
public:
	Matrix4(void);
	~Matrix4(void);
	Matrix4(float mm00,float mm01,float mm02,
		float mm03,float mm10,float mm11,
		float mm12,float mm13,float mm20, 
		float mm21,float mm22,float mm23);
	Matrix4(float m[],BOOL rowMajor);	

	void set(float mm00,float mm01,float mm02,
		float mm03,float mm10,float mm11,
		float mm12,float mm13,float mm20, 
		float mm21,float mm22,float mm23);
	void set(float m[],BOOL rowMajor);
	Matrix4& operator=(const Matrix4&m);
	Matrix4& SetToIdentity();

	float determinant()const;
	Matrix4 inverse()const;
	Matrix4 multiply(const Matrix4&m)const;
	vector<float> asRowMajor()const;
	vector<float> asColMajor()const;
	BOOL isIndentity()const;
	BOOL equals(const Matrix4&m)const;
    BoundingBox transform(const BoundingBox&b) const;
	Vector3 transformV(const Vector3&v)const;
	Vector3 transformTransposeV(const Vector3&v)const;
	Point3 transformP(const Point3&p)const;
	float transformVX(float x,float y,float z)const;
	float transformVY(float x,float y,float z)const;
	float transformVZ(float x,float y,float z)const;
	float transformTransposeVX(float x,float y,float z)const;
	float transformTransposeVY(float x,float y,float z)const;
	float transformTransposeVZ(float x,float y,float z)const;
	float transformPX(float x,float y,float z)const;
	float transformPY(float x,float y,float z)const;
	float transformPZ(float x,float y,float z)const;
	BOOL operator!=(const Matrix4&m)const;
	BOOL operator==(const Matrix4&m)const;	

	static Matrix4 translation(float x,float y,float z);
	static Matrix4 rotateX(float theta);
	static Matrix4 rotateY(float theta);
	static Matrix4 rotateZ(float theta);
	static Matrix4 rotate(float x,float y,float z,float theta);
	static Matrix4 scale(float s);
	static Matrix4 scale(float sx,float sy,float sz);
	static Matrix4 fromBasis(const OrthoNormalBasis&basis);
	static Matrix4 lookAt(const Point3&eye,const Point3&target,const Vector3&up);
	static Matrix4 blend(const Matrix4&m0,const Matrix4&m1,float t);

	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;

private:
	float m00;
	float m01;
	float m02;
	float m03;
	float m10;
	float m11;
	float m12;
	float m13;
	float m20;
	float m21;
	float m22;
	float m23;	
};
