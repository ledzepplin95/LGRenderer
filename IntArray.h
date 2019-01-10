#pragma once

class IntArray
{
public:
	IntArray();
    IntArray(int capacity);

    void add(int i);
	void set(int index,int v);
	vector<int> trim();

	int get(int index)const;
	int getSize()const;

private:
	vector<int> a;
	int s;
};
