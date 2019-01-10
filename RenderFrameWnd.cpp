// RenderFrameWnd.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "RenderFrameWnd.h"
#include "lgapi.h"
#include "display.h"
#include "ximage.h"
#include "FindCxImageFileType.h"
#include "ThreadThing.h"

// CRenderFrameWnd

IMPLEMENT_DYNCREATE(CRenderFrameWnd, CFrameWnd)

CRenderFrameWnd::CRenderFrameWnd()
{
	api=NULL;
	dis=NULL; 
	threadExitID=0;
	thrd=NULL;
	image=NULL;
}

CRenderFrameWnd::~CRenderFrameWnd()
{
	if(image) delete image;
}

BEGIN_MESSAGE_MAP(CRenderFrameWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EXIT, &CRenderFrameWnd::OnExit)
	ON_COMMAND(ID_FILESAVE, &CRenderFrameWnd::OnFilesave)
END_MESSAGE_MAP()


// CRenderFrameWnd message handlers

int CRenderFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1; 

	ModifyStyle(0,WS_MAXIMIZEBOX);
	ModifyStyle(0,WS_MINIMIZEBOX);   
	LoadNewIcon(IDI_ICON2);
	CenterWindow(CWnd::GetDesktopWindow());
   
	return 0;
}

void CRenderFrameWnd::LoadNewIcon(UINT nIconID)
{
	HICON hIconNew =AfxGetApp()->LoadIcon(nIconID);
	HICON hIconOld =(HICON)GetClassLong(m_hWnd,GCL_HICON);
	ASSERT(hIconNew);
	ASSERT(hIconOld);

	if (hIconNew !=hIconOld)
	{
		DestroyIcon(hIconOld);
		SetClassLong(m_hWnd,GCL_HICON,(long)hIconNew);
		RedrawWindow(NULL,NULL,RDW_FRAME | RDW_ERASE | RDW_INVALIDATE);
	}
}

BOOL CRenderFrameWnd::PreCreateWindow(CREATESTRUCT& cs)   
{
	return CFrameWnd::PreCreateWindow(cs);
}

void CRenderFrameWnd::PostNcDestroy()
{
	CFrameWnd::PostNcDestroy();
}

void CRenderFrameWnd::OnClose()
{ 
    ClearThread();
	CFrameWnd::OnClose();
}

void CRenderFrameWnd::OnExit()
{
    ClearThread();
	DestroyWindow();
}

void CRenderFrameWnd::ClearThread()
{
	if(!isDone)
	{
	}
	isExit=TRUE;
	if(GetExitCodeThread(thrd,&threadExitID))
	{
		if(threadExitID==STILL_ACTIVE)
		{			
			WaitForSingleObject(thrd,INFINITE);
		}			
	}	

	LGMemoryAllocator::lg_mem_reset();
	if(dis) delete dis;
	if(api) delete api;	
	dis=NULL;
	api=NULL;
	mvcCount=0;
}

void CRenderFrameWnd::OnFilesave()
{ 
	if(!isDone) return;
	CString strFilter = "Î»Í¼|*.bmp|JPEGÍ¼Ïñ|*.jpg|PNGÍ¼Ïñ|*.png||";

	CFileDialog dlg(FALSE,NULL,NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,strFilter);
	dlg.m_ofn.nFilterIndex = 3;
	HRESULT hResult = (int)dlg.DoModal();
	if (FAILED(hResult))
	{
		return;
	}

	CString fileName=dlg.m_ofn.lpstrFile;
	CString strExtension;
	if(dlg.m_ofn.nFileExtension == 0) 
	{
		switch (dlg.m_ofn.nFilterIndex)
		{
		case 1:
			strExtension = "bmp";
			break;
		case 2:
			strExtension = "jpg";
			break;	
		case 3:
			strExtension = "png";
			break;
		default:
			break;
		}

		fileName = fileName + '.' + strExtension;
	}
	
	image->Save(fileName,FindType(fileName));
}

typedef struct fdThreadParameter
{
	LGAPI*a;
	Display*d;

	fdThreadParameter()
	{
		a=NULL;
		d=NULL;
	}
	fdThreadParameter(LGAPI*_a,Display*_d)
	{
		a=_a;
		d=_d;
	}

	void set(LGAPI*_a,Display*_d)
	{
		a=_a;
		d=_d;
	}
} *pfdThreadParameter;

static fdThreadParameter fdtp;

DWORD WINAPI fd_run(LPVOID lpParam) 
{	
	pfdThreadParameter para=(pfdThreadParameter)lpParam;
	para->a->render(LGAPI::DEFAULT_OPTIONS,para->d);

	return 0;
}

void CRenderFrameWnd::RenderingBack(LGAPI*a,Display*d)
{
	api=a;
	dis=d;

	isExit=FALSE;
	isDone=FALSE;
	threadCount=0;
    fdtp.set(a,d);
	DWORD threadID;
	thrd=::CreateThread(NULL,0,fd_run,&fdtp,0,&threadID);  
	while(thrd==NULL) 
	{ 		
        thrd=::CreateThread(NULL,0,fd_run,&fdtp,0,&threadID);
		if(thrd!=NULL) break;
	}	
}

void CRenderFrameWnd::SetImage(CxImage*i)
{
	if(image) delete image;
	image=i;     
}




