// RenderZoomView2.cpp : implementation file
//

#include "stdafx.h"
#include "RenderZoomView2.h"
#include "ximage.h"
#include "ThreadThing.h"

// CRenderZoomView2

IMPLEMENT_DYNCREATE(CRenderZoomView2, CView)

CRenderZoomView2::CRenderZoomView2()
{
    m_imageWidth=m_viewWidth=640;
	m_imageHeight=m_viewHeight=480;
	m_bucketSize=32;
	image=NULL;

	m_bucketX=0;
	m_bucketY=0;
	m_bucketWidth=32;
	m_bucketHeight=32;
	viewFactor=1.0f;
}

CRenderZoomView2::~CRenderZoomView2()
{
}

BEGIN_MESSAGE_MAP(CRenderZoomView2, CView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CRenderZoomView2 drawing

void CRenderZoomView2::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	
	CDC MemDC;          
	CBitmap MemBitmap; 	
	MemDC.CreateCompatibleDC( pDC );
	MemBitmap.CreateCompatibleBitmap(pDC,m_imageWidth,m_imageHeight);
	CBitmap *pOldBit=MemDC.SelectObject( &MemBitmap );
	if(image!=NULL)
	{	
	    image->Draw(MemDC.GetSafeHdc(),0,0,m_imageWidth,m_imageHeight);
		if(!isDone) 
			drawCurrentBucketCaret(&MemDC);
		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),
			&MemDC,0,0,m_imageWidth,m_imageHeight,SRCCOPY );		
	}
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();		
}

void CRenderZoomView2::drawCurrentBucketCaret(CDC*pDC)
{
    CPen p(PS_SOLID,0,RGB(255,255,255));
	pDC->SelectObject(&p);

	pDC->MoveTo(m_bucketX+m_bucketWidth/4,m_bucketY);
    pDC->LineTo(m_bucketX,m_bucketY);
    pDC->LineTo(m_bucketX,m_bucketY+m_bucketHeight/4);
	pDC->MoveTo(m_bucketX,m_bucketY+(3*m_bucketHeight)/4);
	pDC->LineTo(m_bucketX,m_bucketY+m_bucketHeight);
	pDC->LineTo(m_bucketX+m_bucketWidth/4,m_bucketY+m_bucketHeight);
	pDC->MoveTo(m_bucketX+(3*m_bucketWidth)/4,m_bucketY+m_bucketHeight);
	pDC->LineTo(m_bucketX+m_bucketWidth,m_bucketY+m_bucketHeight);
	pDC->LineTo(m_bucketX+m_bucketWidth,m_bucketY+(3*m_bucketHeight)/4);
	pDC->MoveTo(m_bucketX+m_bucketWidth,m_bucketY+m_bucketHeight/4);
	pDC->LineTo(m_bucketX+m_bucketWidth,m_bucketY);
	pDC->LineTo(m_bucketX+(3*m_bucketWidth)/4,m_bucketY);
}

void CRenderZoomView2::SetImage(CxImage *i)
{
    image=i;
}

void CRenderZoomView2::SetImageSize(int w,int h)
{
	m_imageWidth=m_viewWidth=w;
	m_imageHeight=m_viewHeight=h;	
}

void CRenderZoomView2::SetBucketSize(int bucketSize)
{
	m_bucketSize=bucketSize;
}

void CRenderZoomView2::SetBucketInfo(int bucketX, int bucketY, 
									 int bucketWidth,int bucketHeight)
{
	m_bucketX=bucketX;
	m_bucketY=bucketY;
	m_bucketWidth=bucketWidth;
	m_bucketHeight=bucketHeight;
}

// CRenderZoomView2 diagnostics

#ifdef _DEBUG
void CRenderZoomView2::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CRenderZoomView2::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRenderZoomView2 message handlers

BOOL CRenderZoomView2::OnEraseBkgnd(CDC* pDC)
{
    Update(pDC);

	return TRUE;
}

void CRenderZoomView2::Update(CDC*pDC)
{
	CRect r;
	pDC->GetClipBox(r);
	CBrush brush;
	brush.CreateSolidBrush(RGB(192,192,192));	
	pDC->FillRect(r,&brush);
	int w=r.right-r.left;
	int h=r.bottom-r.top;
	rect=CRect((w-m_viewWidth)/2.0,(h-m_viewHeight)/2.0,
		(w+m_viewWidth)/2.0,(m_viewHeight+h)/2.0);   
	CBrush brush2;
	brush2.CreateSolidBrush(RGB(0,0,0));	
	pDC->FillRect(rect,&brush2);		
}

BOOL CRenderZoomView2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{	
    float factor=zDelta/WHEEL_DELTA;
	if(factor<0)
	{
		m_viewWidth/=2.0f;
        m_viewHeight/=2.0f;
		viewFactor*=0.5f;
	}
	else
	{
		m_viewWidth*=2.0f;
		m_viewHeight*=2.0f;
		viewFactor*=2.0f;
	}
	InvalidateRect(NULL);

	return TRUE;
}
