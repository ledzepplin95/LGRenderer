#pragma once
#include "imagesampler.h"
#include "color.h"

class BucketRenderer :
	public ImageSampler
{  
	LG_DECLARE_DYNCREATE(BucketRenderer);

private:
	class ImageSample
	{
	public:
		float rx,ry;
		int i,n;
		Color c;
		float alpha;
		Instance* instance;
		Shader* shader;
		float nx,ny,nz;

		ImageSample();
		ImageSample(const ImageSample&is);
		ImageSample(float xx,float yy,int ii);

		void set(float xx,float yy,int ii);
        void set(const ShadingState&state);
		void add(const ShadingState&state);
		void scale(float s);

        BOOL processed()const;
		BOOL sampled()const;
		BOOL isDifferent(const ImageSample&sample,float thresh)const;

        ImageSample& operator=(const ImageSample&sample);

		static ImageSample bilerp(const ImageSample&i00,const ImageSample&i01,
			const ImageSample&i10,const ImageSample&i11,float dx,float dy,
			ImageSample&result);
	};

public:
	BucketRenderer();

	BOOL prepare(Options*o,Scene*s,int w,int h);
    void render(Display*dis);

public:
	Scene* scene;
	Display* display;
	
	int imageWidth;
	int imageHeight;
	
	CString bucketOrderName;
	BucketOrder* bucketOrder;
	int bucketSize;
	int bucketCounter;
	vector<int> bucketCoords;
	BOOL dumpBuckets;

	int minAADepth;
	int maxAADepth;
	int superSampling;
	float contrastThreshold;
	boolean jitter;
	boolean displayAA;

	double invSuperSampling;
	int subPixelSize;
	int minStepSize;
	int maxStepSize;
	int sigmaOrder;
	int sigmaLength;
	float thresh;
	BOOL useJitter;
	
	CString filterName;
	Filter* filter;
	int fs;
	float fhs;

	CString aaDepthToString(int depth)const;
	void renderBucket(Display*dis,int bx,int by,int threadID, 
		IntersectionState&iState);
	void computeSubPixel(IntersectionState&iState,ImageSample&sample)const;
	void refineSamples(int sbw,int x,int y,int stepSize,
		float thresh,IntersectionState&iState,vector<ImageSample>&samples)const;
};
