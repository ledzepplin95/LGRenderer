#include "StdAfx.h"

#include "LGRendering.h"
#include "Benchmark.h"
#include "RenderingFrame.h"
#include "realtimebenchmark.h"

extern "C" __declspec( dllexport ) void  renderBegin()
{	
}



extern "C" __declspec( dllexport ) BOOL LGRender(const char*path)
{
	vector<CString> args; 	
	args.push_back("-samples");
	args.push_back("1");
	args.push_back("-showaa");
	args.push_back(path);
	args.push_back("-threads");
	args.push_back("2");

	return renderFrame(args);
}

extern "C" __declspec( dllexport ) void renderEnd()
{
}

DWORD WINAPI bm_run(LPVOID lpParam) 
{	
	vector<CString> args;
	args.push_back("-regen");
	args.push_back("2");
	args.push_back("512");
	Benchmark::Run(args);

	return 0;
}

extern "C" __declspec( dllexport ) void benchMarkTest()
{
	DWORD threadID;
	HANDLE thrd=::CreateThread(NULL,0,bm_run,NULL,0,&threadID);  
	while(thrd==NULL) 
	{ 		
		thrd=::CreateThread(NULL,0,bm_run,NULL,0,&threadID);
		if(thrd!=NULL) break;
	}	
}

DWORD WINAPI rt_run(LPVOID lpParam) 
{	
    RealtimeBenchmark rtbc(2);

	return 0;
}

extern "C" __declspec( dllexport ) void realtimeRenderTest()
{
	DWORD threadID;
	HANDLE thrd=::CreateThread(NULL,0,rt_run,NULL,0,&threadID);  
	while(thrd==NULL) 
	{ 		
		thrd=::CreateThread(NULL,0,rt_run,NULL,0,&threadID);
		if(thrd!=NULL) break;
	}
}