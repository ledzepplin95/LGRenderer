#pragma once
#include "display.h"

class FrameDisplay :
	public Display
{
public:
	FrameDisplay(void);
	FrameDisplay(LGAPI*a);
	virtual ~FrameDisplay(void);

	virtual void imageBegin(int w,int h,int bucketSize);
	virtual void imagePrepare(int x,int y,int w,int h,int id);	
	virtual void imageUpdate(int x,int y,int w,int h, 
		vector<Color>&data,vector<float>&alpha);
	virtual void imageFill(int x,int y,int w,int h,Color&c,float alpha);
	virtual void imageEnd();	

private:
	CString m_fileName;
    CSingleDocTemplate*m_pDocTemplate; 
	class CRenderDoc*m_doc;
	class CRenderFrameWnd*m_fr;
	class CRenderZoomView2*m_view;
	class CxImage*image;	
};

