#include "StdAfx.h"

#include "Benchmark.h"
#include "LGdef.h"
#include "ImgPipeDisplay.h"
#include "FileDisplay.h"
#include "strutil.h"
#include "BenchmarkFramework.h"
#include "ximage.h"
#include "FindCxImageFileType.h"
#include "point3.h"
#include "Vector3.h"
#include "color.h"
#include "LGMemoryAllocator.h"

Benchmark::BenchmarkScene::BenchmarkScene() 
{	
}

Benchmark::BenchmarkScene::BenchmarkScene(int res,BOOL sao,int t,BOOL sw)
{
	resolution=res;
	saveOutput=sao;
    threads=t;
	showWindow=sw;

	build();
	CString str;
	str.Format(_T("golden_%04X.png"),resolution);
	str=StrUtil::GetFileDir(StrUtil::GetModuleFilePath())+str;
	Display*dis=showWindow?
		(Display*)new ImgPipeDisplay():(Display*)new FileDisplay(str);
	render(LGAPI::DEFAULT_OPTIONS,dis);	
	delete dis;
	LGMemoryAllocator::lg_mem_reset();
}

void Benchmark::BenchmarkScene::build() 
{
	parameter("threads",threads);

	boolean tPriority=FALSE; 
	parameter("threads.lowPriority",tPriority);
	parameter("resolutionX",resolution);
	parameter("resolutionY",resolution);
	parameter("depths.diffuse",2);
	parameter("depths.reflection",2);
	parameter("depths.refraction",2);
	parameter("bucket.order","hilbert");
	parameter("bucket.size",32);
	parameter("aa.min",-1);
	parameter("aa.max",1);
	parameter("filter","triangle");
	parameter("gi.engine","igi");
	parameter("gi.igi.samples",90);
	parameter("gi.igi.c",0.000008f);	
	options(LGAPI::DEFAULT_OPTIONS);

	buildCornellBox();
}

void Benchmark::BenchmarkScene::buildCornellBox()
{
	Matrix4 m=Matrix4::lookAt(
		Point3(0,0,-600),Point3(0,0,0),Vector3(0,1,0));
	parameter("transform",m);
	parameter("fov",45.0f);
	camera("main_camera","pinhole");
	parameter("camera","main_camera");
	options(LGAPI::DEFAULT_OPTIONS);

	float minX=-200.0f;
	float maxX=200.0f;
	float minY=-160.0f;
	float maxY=minY+400.0f;
	float minZ=-250.0f;
	float maxZ=200.0f;
	vector<float> data;
	data.resize(3);
	vector<int> intTmp;	

	data[0]=0.7f; data[1]=0.7f; data[2]=0.7f;
	parameter("diffuse",CString(),data);
	shader("gray_shader","diffuse");
	data[0]=0.8f; data[1]=0.25f; data[2]=0.25f;
	parameter("diffuse",CString(),data);
	shader("red_shader","diffuse");
	data[0]=0.25f; data[1]=0.25f; data[2]=0.8f;
	parameter("diffuse",CString(),data);
	shader("blue_shader","diffuse");	

	vector<float> verts;
	verts.push_back(minX); verts.push_back(minY); verts.push_back(minZ);
	verts.push_back(maxX); verts.push_back(minY); verts.push_back(minZ);
	verts.push_back(maxX); verts.push_back(minY); verts.push_back(maxZ);
	verts.push_back(minX); verts.push_back(minY); verts.push_back(maxZ);
	verts.push_back(minX); verts.push_back(maxY); verts.push_back(minZ);
	verts.push_back(maxX); verts.push_back(maxY); verts.push_back(minZ);
	verts.push_back(maxX); verts.push_back(maxY); verts.push_back(maxZ);
	verts.push_back(minX); verts.push_back(maxY); verts.push_back(maxZ);
    vector<int> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3); indices.push_back(0);
	indices.push_back(4); indices.push_back(5); indices.push_back(6);
	indices.push_back(6); indices.push_back(7); indices.push_back(4);
	indices.push_back(1); indices.push_back(2); indices.push_back(5);
	indices.push_back(5); indices.push_back(6); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(6);
	indices.push_back(6); indices.push_back(7); indices.push_back(3);
	indices.push_back(0); indices.push_back(3); indices.push_back(4);
	indices.push_back(4); indices.push_back(7); indices.push_back(3);	
	parameter("triangles",indices);
	parameter("points","point","vertex",verts);	
	intTmp.push_back(0); intTmp.push_back(0); intTmp.push_back(0);
	intTmp.push_back(0); intTmp.push_back(1); intTmp.push_back(1);
	intTmp.push_back(0); intTmp.push_back(0); intTmp.push_back(2);
	intTmp.push_back(2);
	parameter("faceshaders",intTmp);	
	geometry("walls","triangle_mesh");
	vector<CString> CStringTmp;
	CStringTmp.push_back("gray_shader");
	CStringTmp.push_back("red_shader");
	CStringTmp.push_back("blue_shader");		
	parameter("shaders",CStringTmp);
	parameter("transform",Matrix4::IDENTITY);
	instance("walls.instance","walls");
	
	vector<float> floatTmp;
    floatTmp.push_back(-50); floatTmp.push_back(maxY-1); floatTmp.push_back(-50);
    floatTmp.push_back(50); floatTmp.push_back(maxY-1); floatTmp.push_back(-50);
	floatTmp.push_back(50); floatTmp.push_back(maxY-1); floatTmp.push_back(50);
	floatTmp.push_back(-50); floatTmp.push_back(maxY-1); floatTmp.push_back(50);
	parameter("points","point","vertex",floatTmp);
	intTmp.clear();
	intTmp.push_back(0); intTmp.push_back(1); intTmp.push_back(2);
	intTmp.push_back(2); intTmp.push_back(3); intTmp.push_back(0);
	parameter("triangles",intTmp);
	data[0]=15.0f; data[1]=15.0f; data[2]=15.0f;
	parameter("radiance",CString(),data);
	parameter("samples",8);
	light("light","triangle_mesh");

	parameter("eta",1.6f);	
	shader("Glass","glass");
	sphere("glass_sphere","Glass",-120,minY+55,-150,50);
	data[0]=0.7f; data[1]=0.7f; data[2]=0.7f;
	parameter("color",CString(),data);
	shader("Mirror","mirror");
	sphere("mirror_sphere","Mirror",100,minY+60,-50,50);
}

void Benchmark::BenchmarkScene::sphere(CString name,CString shaderName,
									   float x,float y,float z,float radius)
{
	geometry(name,"sphere");
	parameter("transform",Matrix4::translation(x,y,z).multiply(Matrix4::scale(radius)));
	vector<CString> para;
	para.push_back(shaderName);
	parameter("shaders",para);
	instance(name+".instance",name);
}

void Benchmark::Run(const vector<CString>&args) 
{
	if(args.empty())
	{
		StrUtil::PrintPrompt("Benchmark选项:");
		StrUtil::PrintPrompt("  -regen                        重新生成各式大小的参照图像");
		StrUtil::PrintPrompt("  -bench [threads] [resolution] 采用指定线程数和图像分辨率运行benchmark的单一迭代");
		StrUtil::PrintPrompt("                                缺省: threads=0 (自动侦测cpu), resolution=256");		
	} 
	else if(args[0]=="-regen") 
	{
		vector<int> sizes;      
		//sizes.push_back(64); 
		//sizes.push_back(96); sizes.push_back(128); 
		//sizes.push_back(256);sizes.push_back(384);
		sizes.push_back(512);
		int threads=1;
		if(args.size()>=2) threads=atoi(args[1]);
		for(int i=0; i<sizes.size(); i++)	
		{			
			Benchmark b(sizes[i],TRUE,FALSE,TRUE,threads,FALSE);
			b.kernelMain();
		}
	} 
	else if(args[0]=="-bench") 
	{
		int t=0,resolution=256;
		if(args.size()>1)
			t=atoi(args[1]);
		if(args.size()>2)
			resolution=atoi(args[2]);
		Benchmark benchmark(resolution,FALSE,TRUE,FALSE,t,FALSE);
		benchmark.kernelBegin();
		benchmark.kernelMain();
		benchmark.kernelEnd();
	} 
}

Benchmark::Benchmark() 
{
	set(384,FALSE,TRUE,FALSE);
}

Benchmark::Benchmark(int res,BOOL sho,BOOL sbo,BOOL sao) 
{
	set(res,sho,sbo,sao,1,FALSE);
}

Benchmark::Benchmark(int res,BOOL sho,BOOL sbo,
				 BOOL sao,int t,BOOL sw)
{
	set(res,sho,sbo,sao,t,sw);
}

void Benchmark::set(int res,BOOL sho,BOOL sbo,BOOL sao)
{
    set(res,sho,sbo,sao,1,FALSE);
}

void Benchmark::set(int res,BOOL sho,BOOL sbo,
					BOOL sao,int t,BOOL sw)
{
	resolution=res;
	showOutput=sho;
	showBenchmarkOutput=sbo;
	saveOutput=sao;
	showWindow=sw;
	threads=t;
	errorThreshold=6;
	if(saveOutput) return;

	CString str;
	str.Format("resources\\golden_%04X.png",resolution);
	str=StrUtil::GetFileDir(StrUtil::GetModuleFilePath())+str;
	StrUtil::PrintPrompt("从%s中载入参照图像",str);
	try 
	{
		CxImage bi(str,FindType(str));		
		if(bi.GetWidth()!=resolution || bi.GetHeight()!=resolution)
			StrUtil::PrintPrompt("参照图像分辨率无效!期望是%dx%d，发现为%dx%d",
			resolution,resolution,bi.GetWidth(),bi.GetHeight());
		referenceImage.resize(resolution*resolution);
		for(int y=0, i=0; y<resolution; y++)
		{
			for(int x=0; x<resolution; x++, i++)
			{
				RGBQUAD argb=bi.GetPixelColor(x,resolution-1-y);
				referenceImage[i]=argb.rgbBlue&&0xff;
				referenceImage[i]=argb.rgbGreen&&0xff00;
				referenceImage[i]=argb.rgbRed&&0xff0000;
			}
		}
	}
	catch(...)
	{
		StrUtil::PrintPrompt("不能载入参照帧!");
	}
}

void Benchmark::execute() 
{	
	BenchmarkFramework framework(10,600);
	framework.execute(*this);
}

void Benchmark::kernelBegin() 
{	
	validationImage.resize(resolution*resolution);
}

void Benchmark::kernelMain() 
{
	BenchmarkScene ss(resolution,saveOutput,threads,showWindow);	
}

void Benchmark::kernelEnd() 
{	
	int diff=0;
	if( !referenceImage.empty() && validationImage.size()==referenceImage.size() ) 
	{
		for(int i=0; i<validationImage.size(); i++) 
		{
			diff+=abs((validationImage[i]&0xFF)-(referenceImage[i] & 0xFF));
			diff+=abs(((validationImage[i]>>8)&0xFF)-((referenceImage[i]>>8)&0xFF));
			diff+=abs(((validationImage[i]>>16)&0xFF)-((referenceImage[i]>>16)&0xFF));
		}
		if(diff>errorThreshold)
			StrUtil::PrintPrompt("图像检查未通过! - #错误: %d",diff);
		else
			StrUtil::PrintPrompt("图像检查通过!");
	} 
	else
		StrUtil::PrintPrompt("图像检查失败! -参照图像不能比较");
}

void Benchmark::print(CString s) 
{
	if(showOutput || showBenchmarkOutput)
		StrUtil::PrintPrompt(s);	
}

void Benchmark::taskStart(CString s,int min,int max) 
{	
}

void Benchmark::taskStop() 
{	
}

void Benchmark::taskUpdate(int current) 
{	
}

void Benchmark::imageBegin(int w,int h,int bucketSize)
{
}

void Benchmark::imageEnd() 
{	
}

void Benchmark::imageFill(int x,int y,int w,int h,Color&c,float alpha) 
{	
}

void Benchmark::imagePrepare(int x,int y,int w,int h,int id) 
{	
}

void Benchmark::imageUpdate(int x,int y,int w,int h,vector<Color>&data,vector<float>&alpha)
{
	for(int j=0, index=0; j<h; j++, y++)
		for(int i=0, offset=x+resolution*(resolution-1-y); i<w; i++, index++, offset++)
			validationImage[offset]=data[index].copy().toNonLinear().toRGB();
}