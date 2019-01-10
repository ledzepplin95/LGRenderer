// pkpm3dLG.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "pkpm3dLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cpkpm3dLGApp

BEGIN_MESSAGE_MAP(Cpkpm3dLGApp, CWinApp)
END_MESSAGE_MAP()


// Cpkpm3dLGApp construction

Cpkpm3dLGApp::Cpkpm3dLGApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only Cpkpm3dLGApp object

Cpkpm3dLGApp theApp;


// Cpkpm3dLGApp initialization

BOOL Cpkpm3dLGApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



