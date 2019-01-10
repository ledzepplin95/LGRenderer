// RenderDoc.cpp : implementation file
//

#include "stdafx.h"
#include "pkpm3dLG.h"
#include "RenderDoc.h"


// CRenderDoc

IMPLEMENT_DYNCREATE(CRenderDoc, CDocument)

CRenderDoc::CRenderDoc()
{
}

BOOL CRenderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CRenderDoc::~CRenderDoc()
{
}


BEGIN_MESSAGE_MAP(CRenderDoc, CDocument)
END_MESSAGE_MAP()


// CRenderDoc diagnostics

#ifdef _DEBUG
void CRenderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CRenderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CRenderDoc serialization

void CRenderDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif




// CRenderDoc commands
