#pragma once

#include "benchmarktest.h"
#include "UserInterface.h"
#include "display.h"
#include "LGAPI.h"

class Benchmark :
	public BenchmarkTest, public UserInterface, public Display
{
	class BenchmarkScene : public LGAPI
	{
	public:
		BenchmarkScene();
		BenchmarkScene(int res,BOOL sao,int t,BOOL sw);

        void build();

	private:
		void buildCornellBox();
		void sphere(CString name,CString shaderName, 
			float x,float y,float z,float radius);

	private:
		BOOL showWindow;
		BOOL saveOutput;
        int threads;
		int resolution;
	};

public:
	Benchmark();
	Benchmark(int res,BOOL sho,BOOL sbo,BOOL sao);
	Benchmark(int res,BOOL sho,BOOL sbo,BOOL sao,int t,BOOL sw);

	void set(int res,BOOL sho,BOOL sbo,BOOL sao);
	void set(int res,BOOL sho,BOOL sbo,BOOL sao,int t,BOOL sw);	
	void execute();
	void kernelBegin();
	void kernelMain();
	void kernelEnd();
	void print(CString s);
	void taskStart(CString s,int min,int max);
	void taskStop();
	void taskUpdate(int current);
	void imageBegin(int w,int h,int bucketSize);
	void imageEnd();
	void imageFill(int x,int y,int w,int h,Color&c,float alpha);
	void imagePrepare(int x,int y,int w,int h,int id);
	void imageUpdate(int x,int y,int w,int h,
		vector<Color>&data,vector<float>&alpha);

	static void Run(const vector<CString>&args);

private:
	int resolution;
	BOOL showOutput;
	BOOL showBenchmarkOutput;
	BOOL saveOutput;
	BOOL showWindow;
	int threads;
	vector<int> referenceImage;
	vector<int> validationImage;
	int errorThreshold;
};
