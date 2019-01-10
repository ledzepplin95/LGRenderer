#pragma once 

class Vector3
{
public:
	Vector3(void);
	~Vector3(void);
	Vector3(float xx,float yy,float zz);
	Vector3(const Vector3&v);

	short encode()const;
	float get(int i)const;
	float length()const;
	float lengthSquared()const;
	Vector3 normalize(Vector3&dest)const;
	Vector3 div(float d,Vector3&dest)const;
	float dot(float vx,float vy,float vz)const;
	CString toString()const;
	BOOL operator==(const Vector3&v)const;
	BOOL operator!=(const Vector3&v)const;

	Vector3& negate();
	Vector3 negate(Vector3&dest);
	Vector3& mul(float s);
	Vector3 mul(float s,Vector3&dest);
	Vector3 div(float d);	
	float normalizeLength();
	Vector3& normalize();	
	Vector3& set(float xx,float yy,float zz);
	Vector3& set(const Vector3&v);
	Vector3& operator=(const Vector3&v);

	static Vector3 decode(short n,Vector3&dest);
	static Vector3 decode(short n);
	static float dot(const Vector3&v1,const Vector3&v2);
	static Vector3 cross(const Vector3&v1,const Vector3&v2,Vector3&dest);
	static Vector3 add(const Vector3&v1,const Vector3&v2,Vector3&dest);
	static Vector3 sub(const Vector3&v1,const Vector3&v2,Vector3&dest);
	static void initial();

private:
	static float COS_THETA[256];
	static float SIN_THETA[256];
	static float COS_PHI[256];
	static float SIN_PHI[256];

public:
	float x,y,z;
};
