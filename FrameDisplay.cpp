#include "StdAfx.h"

#include "resource.h"
#include "FrameDisplay.h"
#include "RenderFrameWnd.h"
#include "Renderzoomview2.h"
#include "RenderDoc.h"
#include "lgapi.h"
#include "ximage.h"
#include "color.h"
#include "ThreadThing.h"

FrameDisplay::FrameDisplay(void)
{
	m_pDocTemplate=NULL;
	m_doc=NULL;
	m_view=NULL;
	m_fr=NULL;
	image=NULL;
}

FrameDisplay::FrameDisplay(LGAPI*api)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pDocTemplate=new CSingleDocTemplate(IDR_MENU1,
		RUNTIME_CLASS(CRenderDoc),
		RUNTIME_CLASS(CRenderFrameWnd),
		RUNTIME_CLASS(CRenderZoomView2));
	m_doc=(CRenderDoc*)m_pDocTemplate->CreateNewDocument();
	m_fr=(CRenderFrameWnd*)m_pDocTemplate->CreateNewFrame(m_doc,NULL);	
	if(NULL==m_fr) return;	
	CString ss="LGäÖÈ¾Æ÷";	
	m_doc->SetPathName(ss);	
	m_fr->SetTitle(LGAPI::VERSION);
	m_fr->MoveWindow(0,0,800,600);
	m_fr->CenterWindow(CWnd::GetDesktopWindow());
	m_pDocTemplate->InitialUpdateFrame(m_fr,m_doc);
	m_view=(CRenderZoomView2*)m_fr->GetActiveView();
	m_pDocTemplate->RemoveDocument(m_doc);
	m_fr->RenderingBack(api,this);
}

FrameDisplay::~FrameDisplay(void)
{
	if(m_pDocTemplate) delete m_pDocTemplate;
}

void FrameDisplay::imageBegin(int w,int h,int bucketSize)
{	
	m_view->SetImageSize(w,h);	
	m_view->SetBucketSize(bucketSize);   
	image=new CxImage(w,h,32,CXIMAGE_FORMAT_BMP);
	m_view->SetImage(image);
	m_view->Invalidate(TRUE);
}

void FrameDisplay::imagePrepare(int x,int y,int w,int h,int id)
{
}

void FrameDisplay::imageFill(int x,int y,int w,int h,Color&c,float alpha)
{
}

void FrameDisplay::imageUpdate(int x, int y, int w, int h,
							   std::vector<Color> &data,std::vector<float> &alpha)
{	
	for(int j=0,index=0;j<h;j++)
	{
		for(int i=0;i<w;i++,index++)
		{
			COLORREF c=data[index].copy().mul(1.0f/alpha[index]).toNonLinear().toRGBA(alpha[index]);
			image->SetPixelColor(x+i,image->GetHeight()-(y+j),c);			 
		}
	}
	m_view->SetBucketInfo(x,y,w,h);
	CRect rect(x,y,x+w,y+h);
	m_view->Invalidate(FALSE);
}

void FrameDisplay::imageEnd()
{ 
	m_fr->SetImage(image);
}