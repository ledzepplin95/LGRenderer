#pragma once

class UserInterface
{
public:
	virtual void print(CString s);
	virtual void taskStart(CString s,int min,int max);
    virtual void taskUpdate(int current);
	virtual void taskStop();	
};
