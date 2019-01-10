#include "StdAfx.h"

#include "RealtimeBenchmark.h"
#include "strutil.h"
#include "filedisplay.h"
#include "matrix4.h"
#include "point3.h"
#include "Vector3.h"
#include "MathUtil.h"
#include "Timer.h"
#include "LGMemoryAllocator.h"

RealtimeBenchmark::RealtimeBenchmark(unsigned int threads)
{
	CString str;
	str=_T("rt.png");
	str=StrUtil::GetFileDir(StrUtil::GetModuleFilePath())+str;
	Display*display=new FileDisplay(str);
	StrUtil::PrintPrompt("准备基准测试场景...");

	parameter("threads",(int)threads);	
	boolean threadPriority=FALSE;
	parameter("threads.lowPriority",threadPriority);
	parameter("resolutionX",512);
	parameter("resolutionY",512);	
	parameter("depths.diffuse",4);
	parameter("depths.reflection",3);
	parameter("depths.refraction",2);	
	parameter("bucket.order","hilbert");
	parameter("bucket.size",32);	
	parameter("filter","gaussian");	
	parameter("aa.min",1);
	parameter("aa.max",2);
	parameter("aa.samples",1);
	options(DEFAULT_OPTIONS);
	
	Point3 eye(30.0f,0.0f,10.967f);
	Point3 target(0.0f,0.0f,5.4f);
	Vector3 up(0.0f,0.0f,1.0f);
	Matrix4 m=Matrix4::lookAt(eye,target,up);
	parameter("transform",m);
	parameter("fov",45.0f);
	parameter("aspect",1.333333f);
	camera("camera","pinhole");
	parameter("camera","camera");
	options(DEFAULT_OPTIONS);

	createGeometry();

	StrUtil::PrintPrompt("渲染帧...");
	render(DEFAULT_OPTIONS,display);
	delete display;
	LGMemoryAllocator::lg_mem_reset();
}

void RealtimeBenchmark::createGeometry()
{
	vector<float> data;
	data.resize(3);

	parameter("source",Point3(-15.5945f,-30.0581f,45.967f));
	parameter("dir",Vector3(15.5945f,30.0581f,-45.967f));
	parameter("radius",60.0f);	
	data[0]=3; data[1]=3; data[2]=3;
	parameter("radiance",CString(),data);
	light("light","directional");

	parameter("gi.engine","fake");
	data[0]=0.25f; data[1]=0.25f; data[2]=0.25f;
	parameter("gi.fake.sky",CString(),data);
	data[0]=0.01f; data[1]=0.01f; data[2]=0.5f;
	parameter("gi.fake.ground",CString(),data);
	parameter("gi.fake.up",Vector3(0.0f,0.0f,1.0f));
	options(DEFAULT_OPTIONS);

	vector<CString> para;
	data[0]=0.5f; data[1]=0.5f; data[2]=0.5f;
	parameter("diffuse",CString(),data);
	parameter("shiny",0.2f);
	shader("refl","shiny_diffuse");
	parameter("center",Point3(0.0f,0.0f,0.0f));
	parameter("normal",Vector3(0.0f,0.0f,1.0f));
	geometry("ground","plane");	
	para.push_back("refl");
	parameter("shaders",para);
	parameter("transform",Matrix4::IDENTITY);
	instance("ground.instance","ground");

	data[0]=0.7f; data[1]=0.7f; data[2]=0.7f;
	parameter("color",CString(),data);
	shader("Mirror","mirror");
	geometry("sph","sphere");
	Matrix4 m=Matrix4::translation(-3.0f,3.0f,2.0f).multiply(Matrix4::scale(5.0f));
	parameter("transform",m);
	para.clear();
	para.push_back("Mirror");
	parameter("shaders",para);	
	instance("sph.instance","sph");
}