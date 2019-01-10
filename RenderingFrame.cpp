#include "StdAfx.h"
#include "RenderingFrame.h"

#include "strutil.h"
#include "imgpipedisplay.h"
#include "FileDisplay.h"
#include "Framedisplay.h"
#include "lgapi.h"
#include "TriangleMesh.h"
#include "kdtree.h"
#include "ThreadThing.h"

void usage(BOOL verbose) 
{
	StrUtil::PrintPrompt("用法: LG [选项] 场景文件");
	if(verbose) 
	{
		StrUtil::PrintPrompt("LG v"+LGAPI::VERSION+"文字模式");
		StrUtil::PrintPrompt("渲染指定的场景文件");
		StrUtil::PrintPrompt("选项:");
		StrUtil::PrintPrompt("  -o 文件名        输出按指定的文件名存盘(png, hdr, tga)");
		StrUtil::PrintPrompt("                   #'s扩展为当前帧号");
		StrUtil::PrintPrompt("  -nogui           不打开帧显示渲染过程");
		StrUtil::PrintPrompt("  -ipr             使用progressive算法渲染");
		StrUtil::PrintPrompt("  -sampler type    使用指定算法渲染");
		StrUtil::PrintPrompt("  -threads n       使用n个线程渲染");
		StrUtil::PrintPrompt("  -lopri           设置线程优先级为低(缺省)");
		StrUtil::PrintPrompt("  -hipri           设置线程优先级为高");
		StrUtil::PrintPrompt("  -smallmesh       使用内存使用优化的三角形载入三角形mesh");
		StrUtil::PrintPrompt("  -dumpkd          转储KDTree为obj文件供显示用");
		StrUtil::PrintPrompt("  -buildonly       载入场景后不调用渲染方法");
		StrUtil::PrintPrompt("  -showaa          为bucket renderer显示每个像素的采样水平");
		StrUtil::PrintPrompt("  -nogi            关闭场景中的任何全局照明引擎");
		StrUtil::PrintPrompt("  -nocaustics      关闭场景中的任何焦散引擎");
		StrUtil::PrintPrompt("  -pathgi n        使用采样为n的路径跟踪来渲染全局照明");
		StrUtil::PrintPrompt("  -quick_ambocc d  应用环境遮挡于有指定最大距离的场景");
		StrUtil::PrintPrompt("  -quick_uvs       应用surface uv visualization shader于整个场景");
		StrUtil::PrintPrompt("  -quick_normals   应用surface normal visualization shader于整个场景");
		StrUtil::PrintPrompt("  -quick_id        每个instance使用唯一颜色进行渲染");
		StrUtil::PrintPrompt("  -quick_prims     每个图素使用唯一颜色进行渲染");
		StrUtil::PrintPrompt("  -quick_gray      使用plain gray diffuse shader进行渲染");
		StrUtil::PrintPrompt("  -quick_wire      使用wireframe shader进行渲染");
		StrUtil::PrintPrompt("  -resolution w h  改变渲染分辨率为指定的宽度和高度(按像素)");
		StrUtil::PrintPrompt("  -aa min max      覆盖图像反锯齿深度");
		StrUtil::PrintPrompt("  -samples n       覆盖图像采样数(影响bucket和multipass采样器)");
		StrUtil::PrintPrompt("  -bucket n order  改变缺省的bucket大小为n个像素以及缺省的次序");
		StrUtil::PrintPrompt("  -bake name       为指定的Instance烘焙光影贴图");
		StrUtil::PrintPrompt("  -bakedir dir     选择光影贴图烘焙的类型: dir=view或ortho");
		StrUtil::PrintPrompt("  -filter type     选择要用的图像过滤器");
		StrUtil::PrintPrompt("  -bench           运行几个内建场景做基准测试用");
		StrUtil::PrintPrompt("  -rtbench         运行实时光线跟踪基准测试");
		StrUtil::PrintPrompt("  -frame n         设置帧数为指定值");
		StrUtil::PrintPrompt("  -anim n1 n2      渲染两个指定值之间的所有帧(包含端点)");
		StrUtil::PrintPrompt("  -translate file  解释输入场景为指定文件名");
		StrUtil::PrintPrompt("  -v verbosity     设置信息详细层次: 0=none,1=errors,2=warnings,3=info,4=detailed");
		StrUtil::PrintPrompt("  -h               打印此消息");
	}

	exit(1);
}

BOOL renderFrame(const vector<CString>&args)
{
    if(mvcCount>0) return FALSE;
	mvcCount=1;

	StrUtil::PrintPrompt("***LG渲染器***");
	StrUtil::PrintPrompt("渲染...");
	if(args.size()>0) 
	{
		BOOL showFrame=TRUE;
		CString sampler;
		BOOL noRender=FALSE;
		CString fileName;
		CString input="";
		int i=0;
		int threads=1;
		boolean lowPriority=TRUE;
		boolean showAA=TRUE;
		BOOL noGI=FALSE;
		BOOL noCaustics=FALSE;
		int pathGI=0;
		float maxDist=0.0f;
		CString shaderOverride;
		int resolutionW=0,resolutionH=0; 
		int aaMin=-5,aaMax=-5;
		int samples=-1;
		int bucketSize=0;
		CString bucketOrder;
		CString bakingName;
		BOOL bakeViewdep=FALSE;
		CString filterType;
		BOOL runBenchmark=FALSE;
		BOOL runRTBenchmark=FALSE;
		CString translateFilename;
		int frameStart=1,frameStop=1;

		while(i<args.size())
		{
			if(args[i]=="-o")
			{
				if(i>args.size()-2)
					usage(FALSE);
				fileName=args[i+1];
				i+=2;
			} 
			else if(args[i]=="-nogui") 
			{
				showFrame=FALSE;
				i++;
			} 
			else if(args[i]=="-ipr") 
			{
				sampler="ipr";
				i++;
			} 
			else if(args[i]=="-threads") 
			{
				if(i>args.size()-2)
					usage(FALSE);
				threads=atoi(args[i+1]);
				i+=2;
			} 
			else if(args[i]=="-lopri")
			{
				lowPriority=TRUE;
				i++;
			} 
			else if(args[i]=="-hipri") 
			{
				lowPriority=FALSE;
				i++;
			} 
			else if(args[i]=="-sampler")
			{
				if(i>args.size()-2)
					usage(FALSE);
				sampler=args[i+1];
				i+=2;
			} 
			else if(args[i]=="-smallmesh") 
			{
				TriangleMesh::setSmallTriangles(TRUE);
				i++;
			} 
			else if(args[i]=="-dumpkd") 
			{
				KDTree::setDumpMode(TRUE,"kdtree");
				i++;
			} 
			else if(args[i]=="-buildonly") 
			{
				noRender=TRUE;
				i++;
			} 
			else if(args[i]=="-showaa")
			{
				showAA=TRUE;
				i++;
			}
			else if(args[i]=="-nogi") 
			{
				noGI=TRUE;
				i++;
			} 
			else if(args[i]=="-nocaustics") 
			{
				noCaustics=TRUE;
				i++;
			}
			else if(args[i]=="-pathgi")
			{
				if(i>args.size()-2)
					usage(FALSE);
				pathGI=atoi(args[i+1]);
				i+=2;
			} 
			else if(args[i]=="-quick_ambocc") 
			{
				if(i>args.size()-2)
					usage(FALSE);
				maxDist=atof(args[i+1]);
				shaderOverride="ambient_occlusion";				
				i+=2;
			} 
			else if(args[i]=="-quick_uvs")
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="show_uvs";
				i++;
			} 
			else if(args[i]=="-quick_normals")
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="show_normals";
				i++;
			} 
			else if(args[i]=="-quick_id")
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="show_instance_id";
				i++;
			}
			else if(args[i]=="-quick_prims") 
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="show_primitive_id";
				i++;
			} 
			else if(args[i]=="-quick_gray")
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="quick_gray";
				i++;
			} 
			else if(args[i]=="-quick_wire")
			{
				if(i>args.size()-1)
					usage(FALSE);
				shaderOverride="wireframe";
				i++;
			} 
			else if(args[i]=="-resolution")
			{
				if(i>args.size()-3)
					usage(FALSE);
				resolutionW=atoi(args[i+1]);
				resolutionH=atoi(args[i+2]);
				i+=3;
			}
			else if(args[i]=="-aa")
			{
				if(i>args.size()-3)
					usage(FALSE);
				aaMin=atoi(args[i+1]);
				aaMax=atoi(args[i+2]);
				i+=3;
			} 
			else if(args[i]=="-samples")
			{
				if(i>args.size()-2)
					usage(FALSE);
				samples=atoi(args[i+1]);
				i+=2;
			} 
			else if(args[i]=="-bucket")
			{
				if(i>args.size()-3)
					usage(FALSE);
				bucketSize=atoi(args[i+1]);
				bucketOrder=args[i+2];
				i+=3;
			} 
			else if(args[i]=="-bake")
			{
				if(i>args.size()-2)
					usage(FALSE);
				bakingName=args[i+1];
				i+=2;
			}
			else if(args[i]=="-bakedir")
			{
				if(i>args.size()-2)
					usage(FALSE);
				CString baketype=args[i+1];
				if(baketype=="view")
					bakeViewdep=TRUE;
				else if(baketype=="ortho")
					bakeViewdep=FALSE;
				else
					usage(FALSE);
				i+=2;
			}
			else if(args[i]=="-filter") 
			{
				if(i>args.size()-2)
					usage(FALSE);
				filterType=args[i+1];
				i+=2;
			}
			else if(args[i]=="-bench")
			{
				runBenchmark=TRUE;
				i++;
			} 
			else if(args[i]=="-rtbench")
			{
				runRTBenchmark=TRUE;
				i++;
			} 
			else if(args[i]=="-frame")
			{
				if(i>args.size()-2)
					usage(FALSE);
				frameStart=frameStop=atoi(args[i+1]);
				i+=2;
			} 
			else if(args[i]=="-anim") 
			{
				if(i>args.size()-3)
					usage(FALSE);
				frameStart=atoi(args[i+1]);
				frameStop=atoi(args[i+2]);
				i+=3;
			}
			else if(args[i]=="-v")
			{
				if(i>args.size()-2)
					usage(FALSE);				
				i+=2;
			} 
			else if(args[i]=="-translate")
			{
				if(i>args.size()-2)
					usage(FALSE);
				translateFilename=args[i+1];
				i+=2;
			} 
			else if( args[i]=="-h" || args[i]=="-help" )
			{
				usage(TRUE);
			} 
			else
			{
				if(!input.IsEmpty())
					usage(FALSE);
				input=args[i];
				i++;
			}
		}

		if(input.IsEmpty())
			usage(FALSE);		
		if( !translateFilename.IsEmpty() )
		{
			LGAPI::translate(input,translateFilename);
			return FALSE;
		}
		if( frameStart<frameStop && showFrame )
		{
			StrUtil::PrintPrompt("渲染动画需打开-nogui开关，强制关闭GUI");
			showFrame=FALSE;
		}
		if( frameStart<=frameStop && fileName.IsEmpty() )
		{
			fileName="output.#.png";
			StrUtil::PrintPrompt("未指定动画输出文件，缺省为:\"%s\"",fileName);
		}
		for(int frameNumber=frameStart;	frameNumber<=frameStop; frameNumber++) 
		{
			LGAPI* api=LGAPI::create(input,frameNumber);
			if(noRender) continue;
			if( resolutionW>0 && resolutionH>0 ) 
			{
				api->parameter("resolutionX",resolutionW);
				api->parameter("resolutionY",resolutionH);
			}
			if( aaMin!=-5 || aaMax!=-5 ) 
			{
				api->parameter("aa.min",aaMin);
				api->parameter("aa.max",aaMax);
			}
			if( samples>=0 )
				api->parameter("aa.samples",samples);
			if( bucketSize>0 )
				api->parameter("bucket.size",bucketSize);
			if( !bucketOrder.IsEmpty() )
				api->parameter("bucket.order",bucketOrder);
			api->parameter("aa.display",showAA);
			api->parameter("threads",threads);
			api->parameter("threads.lowPriority",lowPriority);
			if( !bakingName.IsEmpty() ) 
			{
				api->parameter("baking.instance",bakingName);
				api->parameter("baking.viewdep",bakeViewdep);
			}
			if( !filterType.IsEmpty() )
				api->parameter("filter",filterType);
			if(noGI)
				api->parameter("gi.engine","none");
			else if(pathGI>0) 
			{
				api->parameter("gi.engine","path");
				api->parameter("gi.path.samples",pathGI);
			}
			if(noCaustics)
				api->parameter("caustics","none");
			if(!sampler.IsEmpty())
				api->parameter("sampler",sampler);
			api->options(LGAPI::DEFAULT_OPTIONS);
			if(!shaderOverride.IsEmpty())
			{
				if(shaderOverride=="ambient_occlusion")
					api->parameter("maxdist",maxDist);
				api->shader("cmdline_override",shaderOverride);
				api->parameter("override.shader","cmdline_override");
				api->parameter("override.photons",TRUE);
				api->options(LGAPI::DEFAULT_OPTIONS);
			}

			Display* display=NULL;
			CString str;
			str.Format(_T("%04d"),frameNumber);
			CString r_fileName=fileName;
			r_fileName.Replace("#",str);
			CString currentFilename=(!fileName.IsEmpty())?r_fileName:CString();
			if(showFrame) 
			{ 
                display=new FrameDisplay(api);
			}
			else 
			{
				if( !currentFilename.IsEmpty()
					&& currentFilename=="imgpipe" ) 
				{
					display=new ImgPipeDisplay();
				}
				else
				{
					display=new FileDisplay(currentFilename);			
				}
				api->render(LGAPI::DEFAULT_OPTIONS,display);				
				LGMemoryAllocator::lg_mem_reset();
				if(display) delete display;
				delete api;	
			}				
		}
	}

	return TRUE;
}