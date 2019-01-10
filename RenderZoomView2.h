#pragma once


// CRenderZoomView2 view

class CRenderZoomView2 : public CView
{
	DECLARE_DYNCREATE(CRenderZoomView2)

public:
	void SetImage(class CxImage*i);
	void SetImageSize(int w,int h);
	void SetBucketSize(int bucketSize);
	void SetBucketInfo(int buckerX,int buckerY,int bucketWidth,int bucketHeight);

protected:
	CRenderZoomView2();           // protected constructor used by dynamic creation
	virtual ~CRenderZoomView2();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

private:
	int m_imageWidth;
	int m_imageHeight;
	int m_viewWidth;
	int m_viewHeight;
	int m_bucketSize;

	int m_bucketX;
	int m_bucketY;
	int m_bucketWidth;
	int m_bucketHeight;

	float viewFactor;
	CRect rect;
	class CxImage*image;

	void Update(CDC*pDC);
	void drawCurrentBucketCaret(CDC*pDC);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


