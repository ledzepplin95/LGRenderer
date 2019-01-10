#pragma once


// CRenderFrameWnd frame


class CRenderFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CRenderFrameWnd)
protected:
	CRenderFrameWnd();           // protected constructor used by dynamic creation
	virtual ~CRenderFrameWnd();

public:
	void RenderingBack(LGAPI*a,Display*d);
	void SetImage(class CxImage*i);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);  
	
	virtual void PostNcDestroy();
	void LoadNewIcon(UINT nIconID);

private:
	LGAPI* api;
	Display*dis;
	HANDLE thrd;
	DWORD threadExitID;
	class CxImage*image;

	void ClearThread();
	
public:
	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnFilesave();	
};
