#pragma once

class Timer
{
public:
	Timer(void);
	~Timer(void);

	void start();
	void end() ;
	long nanos()const;
	double seconds()const;
	CString toString()const;

	static CString toString(long nanos);
	static CString toString(double seconds);

private:
	long startTime,endTime;
};
