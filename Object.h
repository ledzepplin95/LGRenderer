#pragma once

class Object
{
public:
	enum o_type 
	{
		O_STRING,
		O_INT,
		O_BOOL,		
		O_FLOAT,
		O_POINT,
		O_VECTOR,
		O_TEXCOORD,
		O_MATRIX,
		O_COLOR,
	};
	o_type m_vt;
	int m_size;

    union
	{
		char* m_string[100];
		int *m_int;
		int m_i;
		bool m_bool;
        float *m_float;
		float m_f;    
        float m_color[3];		
	};

	Object();
	~Object();
	Object(const CString&v);
	Object(const vector<CString>&v);
	Object(int v);
	Object(const vector<int>&v);
	Object(bool v);
	Object(float v);
	Object(const vector<float>&v);
	Object(const Point3&v);
	Object(const Vector3&v);
	Object(const Point2&v);
	Object(const Matrix4&v);
	Object(const Color&v);

	Object&operator=(const CString&v);
	Object&operator=(const vector<CString>&v);
	Object&operator=(int v);
	Object&operator=(const vector<int>&v);
	Object&operator=(bool v);
	Object&operator=(float v);
	Object&operator=(const vector<float>&v);
	Object&operator=(const Point3&v);
	Object&operator=(const Vector3&v);
	Object&operator=(const Point2&v);
	Object&operator=(const Matrix4&v);
	Object&operator=(const Color&v);

	CString AsString()const;
	vector<CString> AsStrings()const;
	int AsInt()const;
	vector<int> AsInts()const;
	bool AsBool()const;
	float AsFloat()const;
	vector<float> AsFloats()const;
	Point3 AsPoint()const;
	Vector3 AsVector()const;
	Point2 AsTexCoord()const;
	Matrix4 AsMatrix()const;
	Color AsColor()const;
};
