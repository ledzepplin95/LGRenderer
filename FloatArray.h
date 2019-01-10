#pragma once

class FloatArray
{
public:
	FloatArray();
	FloatArray(int capacity);

	void add(float f);
	void set(int index,float v);	
	vector<float> trim();

	float get(int index)const;
	int getSize()const;

private:
	vector<float> a;
	int s;	
};
