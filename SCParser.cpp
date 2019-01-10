#include "StdAfx.h"

#include "SCParser.h"
#include "mathutil.h"
#include "LGAPI.h"
#include "PluginRegistry.h"
#include "ColorFactory.h"
#include "Timer.h"
#include "StrUtil.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "lgapiinterface.h"
#include "Vector3.h"
#include "point3.h"
#include "color.h"
#include "matrix4.h"

int SCParser::instanceCounter=0;

LG_IMPLEMENT_DYNCREATE(SCParser,SceneParser)

SCParser::SCParser() 
{	
	instanceCounter++;
	instanceNumber=instanceCounter;
	width=640;
	height=480;
}

CString SCParser::generateUniqueName(CString prefix)
{	
	int index=1;
	int v;	
	if(IsValueIn(prefix,v))
	{
		index=v;
		objectNames.insert(CI_PAIR(prefix,index+1));
	}
	else 
        objectNames.insert(CI_PAIR(prefix,index+1));

	CString str;
	str.Format(_T("@lg_%d::%s_%d"),instanceNumber,prefix,index);

	return str;
}

BOOL SCParser::IsValueIn(CString name,int&v)const
{
	hash_map<CString,int,
		hash_compare<const char*,CharLess>>::const_iterator iter;
	iter=objectNames.find(name);
	if(iter!=objectNames.end())
	{
		v=iter->second;
		return TRUE;
	}

	return FALSE;
}

BOOL SCParser::parse(CString fileName,LGAPIInterface&api) 
{	
	CString localDir=StrUtil::GetFileDir(fileName);
	
	numLightSamples=1;
	Timer timer;
	timer.start();
	StrUtil::PrintPrompt("解析\"%s\"...",fileName);
	try
	{
		p.Set(fileName);
		while(TRUE) 
		{
			CString token=p.getNextToken();			
			if(token.IsEmpty()) break;
			token.MakeLower();
			if(token=="image")
			{
				StrUtil::PrintPrompt("读取图像设置...");
				parseImageBlock(api);
			} 
			else if(token=="background") 
			{
				StrUtil::PrintPrompt("读取背景...");
				parseBackgroundBlock(api);
			} 
			else if(token=="accel")
			{
				StrUtil::PrintPrompt("读取加速类型...");
				p.getNextToken();
				StrUtil::PrintPrompt("不推荐设置的加速类型，忽略");
			} 
			else if(token=="filter") 
			{
				StrUtil::PrintPrompt("读取图像过滤器类型...");
				parseFilter(api);
			} 
			else if(token=="bucket") 
			{
				StrUtil::PrintPrompt("读取bucket设置...");
				api.parameter("bucket.size",p.getNextInt());
				api.parameter("bucket.order",p.getNextToken());
				api.options(LGAPI::DEFAULT_OPTIONS);
			} 
			else if(token=="photons") 
			{
				StrUtil::PrintPrompt("读取光子设置...");
				parsePhotonBlock(api);
			} 
			else if(token=="gi")
			{
				StrUtil::PrintPrompt("读取全局照明设置...");
				parseGIBlock(api);
			} 
			else if(token=="lightserver")
			{
				StrUtil::PrintPrompt("读取light server设置...");
				parseLightserverBlock(api);
			}
			else if(token=="trace-depths") 
			{
				StrUtil::PrintPrompt("读取跟踪深度...");
				parseTraceBlock(api);
			} 
			else if(token=="camera") 
			{
				parseCamera(api);
			} 
			else if(token=="shader") 
			{
				if(!parseShader(api))
					return FALSE;
			}
			else if(token=="modifier")
			{
				if(!parseModifier(api))
					return FALSE;
			} 
			else if(token=="override") 
			{
				api.parameter("override.shader",p.getNextToken());
				api.parameter("override.photons",p.getNextBoolean());
				api.options(LGAPI::DEFAULT_OPTIONS);
			} 
			else if(token=="object")
			{
				parseObjectBlock(api);
			}
			else if(token=="instance") 
			{
				parseInstanceBlock(api);
			} 
			else if(token=="light")
			{
				parseLightBlock(api);
			} 
			else if(token=="texturepath") 
			{
				CString path=p.getNextToken();
				if(!PathFileExists(path))
					path=localDir+path;
				api.searchPath("texture",path);
			} 
			else if(token=="includepath") 
			{
				CString path=p.getNextToken();
				if(!PathFileExists(path))
					path=localDir+path;
				api.searchPath("include",path);
			}
			else if(token=="include") 
			{
				CString file=p.getNextToken();
				StrUtil::PrintPrompt("包含:\"%s\"...",file);
				api.include(file);
			} 
			else
			{
				StrUtil::PrintPrompt("未知符号%s",token);
			}
		}
		p.close();
	} 
	catch(...) 
	{
		StrUtil::PrintPrompt("错误...");		
		return FALSE;
	} 

	timer.end();
	StrUtil::PrintPrompt("解析完成");
	StrUtil::PrintPrompt("解析时间:%s",timer.toString());

	return TRUE;
}

void SCParser::parseImageBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	if(p.peekNextToken("resolution")) 
	{
        width=p.getNextInt();
		height=p.getNextInt();
		api.parameter("resolutionX",width);
		api.parameter("resolutionY",height);
	}
	if(p.peekNextToken("sampler"))
		api.parameter("sampler",p.getNextToken());
	if(p.peekNextToken("aa")) 
	{
		api.parameter("aa.min",p.getNextInt());
		api.parameter("aa.max",p.getNextInt());
	}
	if(p.peekNextToken("samples"))
		api.parameter("aa.samples",p.getNextInt());
	if(p.peekNextToken("contrast"))
		api.parameter("aa.contrast",p.getNextFloat());
	if(p.peekNextToken("filter"))
		api.parameter("filter",p.getNextToken());
	if(p.peekNextToken("jitter"))
		api.parameter("aa.jitter",p.getNextBoolean());
	if(p.peekNextToken("show-aa"))
	{
		StrUtil::PrintPrompt("已废弃:忽略show-aa");
		p.getNextBoolean();
	}
	if(p.peekNextToken("cache"))
		api.parameter("aa.cache",p.getNextBoolean());
	if(p.peekNextToken("output"))
	{
		StrUtil::PrintPrompt("已废弃:忽略output语句");
		p.getNextToken();
	}
	api.options(LGAPI::DEFAULT_OPTIONS);
	p.checkNextToken("}");
}

void SCParser::parseBackgroundBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("color");
	api.parameter("color",CString(),parseColor().getRGB());
	api.shader("background.shader","constant");
	api.geometry("background","background");
	api.parameter("shaders","background.shader");
    api.parameter("transform",Matrix4::IDENTITY);
	api.instance("background.instance","background");
	p.checkNextToken("}");
}

void SCParser::parseFilter(LGAPIInterface&api) 
{
	StrUtil::PrintPrompt("已废弃的关键字\"filter\"，在image块中设置此选项");
	CString name=p.getNextToken();
	api.parameter("filter",name);
	api.options(LGAPI::DEFAULT_OPTIONS);
	BOOL hasSizeParams= name=="box" || name=="gaussian" 
		|| name=="blackman-harris" 
		|| name=="sinc" || name=="triangle";
	if(hasSizeParams)
	{
		p.getNextFloat();
		p.getNextFloat();
	}
}

void SCParser::parsePhotonBlock(LGAPIInterface&api) 
{
	int numEmit=0;
	BOOL globalEmit=FALSE;
	p.checkNextToken("{");
	if(p.peekNextToken("emit")) 
	{
		StrUtil::PrintPrompt("共享的光子发射值已废弃，指定每个贴图的光子发射值");
		numEmit=p.getNextInt();
		globalEmit=TRUE;
	}
	if(p.peekNextToken("global")) 
	{
		StrUtil::PrintPrompt("全局光子贴图设置属于gi块，忽略");
		if(!globalEmit)
			p.getNextInt();
		p.getNextToken();
		p.getNextInt();
		p.getNextFloat();
	}
	p.checkNextToken("caustics");
	if(!globalEmit)
		numEmit=p.getNextInt();
	api.parameter("caustics.emit",numEmit);
	api.parameter("caustics",p.getNextToken());
	api.parameter("caustics.gather",p.getNextInt());
	api.parameter("caustics.radius",p.getNextFloat());
	api.options(LGAPI::DEFAULT_OPTIONS);
	p.checkNextToken("}");
}

void SCParser::parseGIBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("type");
	if(p.peekNextToken("irr-cache")) 
	{
		api.parameter("gi.engine",(CString)"irr-cache");
		p.checkNextToken("samples");
		api.parameter("gi.irr-cache.samples",p.getNextInt());
		p.checkNextToken("tolerance");
		api.parameter("gi.irr-cache.tolerance",p.getNextFloat());
		p.checkNextToken("spacing");
		api.parameter("gi.irr-cache.min_spacing",p.getNextFloat());
		api.parameter("gi.irr-cache.max_spacing",p.getNextFloat());		
		if(p.peekNextToken("global")) 
		{
			api.parameter("gi.irr-cache.gmap.emit",p.getNextInt());
			api.parameter("gi.irr-cache.gmap",p.getNextToken());
			api.parameter("gi.irr-cache.gmap.gather",p.getNextInt());
			api.parameter("gi.irr-cache.gmap.radius",p.getNextFloat());
		}
	}
	else if(p.peekNextToken("path")) 
	{
		api.parameter("gi.engine",(CString)"path");
		p.checkNextToken("samples");
		api.parameter("gi.path.samples",p.getNextInt());
		if(p.peekNextToken("bounces")) 
		{
			StrUtil::PrintPrompt("已废弃的设置: bounces，使用diffuse trace depth替代");
			p.getNextInt();
		}
	}
	else if(p.peekNextToken("fake"))
	{
		api.parameter("gi.engine",(CString)"fake");
		p.checkNextToken("up");
		api.parameter("gi.fake.up",parseVector());
		p.checkNextToken("sky");
		api.parameter("gi.fake.sky",CString(),parseColor().getRGB());
		p.checkNextToken("ground");
		api.parameter("gi.fake.ground",CString(),parseColor().getRGB());
	}
	else if(p.peekNextToken("igi")) 
	{
		api.parameter("gi.engine",(CString)"igi");
		p.checkNextToken("samples");
		api.parameter("gi.igi.samples",p.getNextInt());
		p.checkNextToken("sets");
		api.parameter("gi.igi.sets",p.getNextInt());
		if(!p.peekNextToken("b"))
			p.checkNextToken("c");
		api.parameter("gi.igi.c",p.getNextFloat());
		p.checkNextToken("bias-samples");
		api.parameter("gi.igi.bias_samples",p.getNextInt());
	}
	else if(p.peekNextToken("ambocc")) 
	{
		api.parameter("gi.engine",(CString)"ambocc");
		p.checkNextToken("bright");
		api.parameter("gi.ambocc.bright",CString(),parseColor().getRGB());
		p.checkNextToken("dark");
		api.parameter("gi.ambocc.dark",CString(),parseColor().getRGB());
		p.checkNextToken("samples");
		api.parameter("gi.ambocc.samples",p.getNextInt());
		if(p.peekNextToken("maxdist"))
			api.parameter("gi.ambocc.maxdist",p.getNextFloat());
	} 
	else if(p.peekNextToken("none") || p.peekNextToken("null")) 
	{		
		api.parameter("gi.engine",(CString)"none");
	}
	else
		StrUtil::PrintPrompt("未知的gi engine类型\"%s\"，忽略",p.getNextToken());
	api.options(LGAPI::DEFAULT_OPTIONS);
	p.checkNextToken("}");
}

void SCParser::parseLightserverBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	if(p.peekNextToken("shadows")) 
	{
		StrUtil::PrintPrompt("已废弃: 忽略阴影设置");
		p.getNextBoolean();
	}
	if(p.peekNextToken("direct-samples")) 
	{
		StrUtil::PrintPrompt("已废弃: 在面光定义中使用采样samples关键字");
		numLightSamples=p.getNextInt();
	}
	if(p.peekNextToken("glossy-samples")) 
	{
		StrUtil::PrintPrompt("已废弃: 在glossy shader中使用samples关键字");
		p.getNextInt();
	}
	if(p.peekNextToken("max-depth"))
	{
		StrUtil::PrintPrompt("已废弃: max-depth设置，使用trace-depths块替代");
		int d=p.getNextInt();
		api.parameter("depths.diffuse",1);
		api.parameter("depths.reflection",d-1);
		api.parameter("depths.refraction",0);
		api.options(LGAPI::DEFAULT_OPTIONS);
	}
	if(p.peekNextToken("global")) 
	{
		StrUtil::PrintPrompt("已废弃: 忽略global设置，使用photons块替代");
		p.getNextBoolean();
		p.getNextInt();
		p.getNextInt();
		p.getNextInt();
		p.getNextFloat();
	}
	if(p.peekNextToken("caustics")) 
	{
		StrUtil::PrintPrompt("已废弃: 忽略caustics设置，使用photons块替代");
		p.getNextBoolean();
		p.getNextInt();
		p.getNextFloat();
		p.getNextInt();
		p.getNextFloat();
	}
	if(p.peekNextToken("irr-cache")) 
	{
		StrUtil::PrintPrompt("已废弃: 忽略irradiance cache设置，使用gi块替代");
		p.getNextInt();
		p.getNextFloat();
		p.getNextFloat();
		p.getNextFloat();
	}
	p.checkNextToken("}");
}

void SCParser::parseTraceBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	if(p.peekNextToken("diff"))
		api.parameter("depths.diffuse",p.getNextInt());
	if(p.peekNextToken("refl"))
		api.parameter("depths.reflection",p.getNextInt());
	if(p.peekNextToken("refr"))
		api.parameter("depths.refraction",p.getNextInt());
	p.checkNextToken("}");
	api.options(LGAPI::DEFAULT_OPTIONS);
}

void SCParser::parseCamera(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("type");
	CString t=p.getNextToken();
	StrUtil::PrintPrompt("读取%s相机...",t);
	if(p.peekNextToken("shutter"))
	{
		api.parameter("shutter.open",p.getNextFloat());
		api.parameter("shutter.close",p.getNextFloat());
	}
	parseCameraTransform(api);
	CString name=generateUniqueName("camera");
	if(t=="pinhole")
	{
		p.checkNextToken("fov");
		api.parameter("fov",p.getNextFloat());
		p.checkNextToken("aspect");
		api.parameter("aspect",p.getNextFloat());
		if(p.peekNextToken("shift")) 
		{
			api.parameter("shift.x",p.getNextFloat());
			api.parameter("shift.y",p.getNextFloat());
		}
		api.camera(name,"pinhole");
	}
	else if(t=="thinlens")
	{
		p.checkNextToken("fov");
		api.parameter("fov",p.getNextFloat());
		p.checkNextToken("aspect");
		api.parameter("aspect",p.getNextFloat());
		if(p.peekNextToken("shift")) 
		{
			api.parameter("shift.x",p.getNextFloat());
			api.parameter("shift.y",p.getNextFloat());
		}
		p.checkNextToken("fdist");
		api.parameter("focus.distance",p.getNextFloat());
		p.checkNextToken("lensr");
		api.parameter("lens.radius",p.getNextFloat());
		if(p.peekNextToken("sides"))
			api.parameter("lens.sides",p.getNextInt());
		if(p.peekNextToken("rotation"))
			api.parameter("lens.rotation",p.getNextFloat());
		api.camera(name,"thinlens");
	}
	else if(t=="spherical")
	{	
		api.camera(name,"spherical");
	} 
	else if(t=="fisheye")
	{		
		api.camera(name,"fisheye");
	} 
	else
	{
		StrUtil::PrintPrompt("未知的相机类型: %s",p.getNextToken());
		p.checkNextToken("}");
		return;
	}
	p.checkNextToken("}");
	if(!name.IsEmpty())
	{
		api.parameter("camera",name);
		api.options(LGAPI::DEFAULT_OPTIONS);
	}
}

void SCParser::parseCameraTransform(LGAPIInterface&api) 
{
	if(p.peekNextToken("steps"))
	{		
		int n=p.getNextInt();
		api.parameter("transform.steps",n);		
		p.checkNextToken("times");
		float t0=p.getNextFloat();
		float t1=p.getNextFloat();
		vector<float> data;
		data.push_back(t0);
		data.push_back(t1);		
		api.parameter("transform.times","float","none",data);
		for(int i=0; i<n; i++)
			parseCameraMatrix(i,api);
	} 
	else
		parseCameraMatrix(-1,api);
}

void SCParser::parseCameraMatrix(int index,LGAPIInterface&api) 
{
	CString str;
	str.Format(_T("[%d]"),index);
	CString offset=index<0?"":str;
	if(p.peekNextToken("transform"))
	{
		str.Format(_T("transform%s"),offset);
		api.parameter(str,parseMatrix());
	} 
	else 
	{
		if(index>=0)
			p.checkNextToken("{");		
		p.checkNextToken("eye");
		Point3 eye=parsePoint();
		p.checkNextToken("target");
		Point3 target=parsePoint();
		p.checkNextToken("up");
		Vector3 up=parseVector();
		str.Format(_T("transform%s"),offset);
		api.parameter(str,Matrix4::lookAt(eye,target,up));
		if(index>=0)
			p.checkNextToken("}");
	}
}

BOOL SCParser::parseShader(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("name");
	CString name=p.getNextToken();
	StrUtil::PrintPrompt("读取shader: %s...",name);
	p.checkNextToken("type");
	if(p.peekNextToken("diffuse"))
	{
		if(p.peekNextToken("diff"))
		{
			api.parameter("diffuse",CString(), parseColor().getRGB());
			api.shader(name,"diffuse");
		}
		else if(p.peekNextToken("texture")) 
		{
			api.parameter("texture",p.getNextToken());
			api.shader(name,"textured_diffuse");
		} 
		else
			StrUtil::PrintPrompt("在diffuse shader块中未知的选项: %s",p.getNextToken());
	}
	else if(p.peekNextToken("phong")) 
	{
		CString tex;
		if(p.peekNextToken("texture"))
			api.parameter("texture",tex=p.getNextToken());
		else 
		{
			p.checkNextToken("diff");
			api.parameter("diffuse",CString(),parseColor().getRGB());
		}
		p.checkNextToken("spec");
		api.parameter("specular",CString(),parseColor().getRGB());
		api.parameter("power",p.getNextFloat());
		if(p.peekNextToken("samples"))
			api.parameter("samples",p.getNextInt());
		if(!tex.IsEmpty())
			api.shader(name,"textured_phong");
		else
			api.shader(name,"phong");
	}
	else if( p.peekNextToken("amb-occ") || p.peekNextToken("amb-occ2") )
	{
		CString tex;
		if(p.peekNextToken("diff") || p.peekNextToken("bright"))
			api.parameter("bright",CString(),parseColor().getRGB());
		else if(p.peekNextToken("texture"))
			api.parameter("texture",tex=p.getNextToken());
		if(p.peekNextToken("dark"))
		{
			api.parameter("dark",CString(),parseColor().getRGB());
			p.checkNextToken("samples");
			api.parameter("samples",p.getNextInt());
			p.checkNextToken("dist");
			api.parameter("maxdist",p.getNextFloat());
		}
		if(tex.IsEmpty())
			api.shader(name,"ambient_occlusion");
		else
			api.shader(name,"textured_ambient_occlusion");
	} 
	else if(p.peekNextToken("mirror")) 
	{
		p.checkNextToken("refl");
		api.parameter("color",CString(),parseColor().getRGB());
		api.shader(name,"mirror");
	} 
	else if(p.peekNextToken("glass"))
	{
		p.checkNextToken("eta");
		api.parameter("eta",p.getNextFloat());
		p.checkNextToken("color");
		api.parameter("color",CString(),parseColor().getRGB());
		if( p.peekNextToken("absorption.distance") || p.peekNextToken("absorbtion.distance") )
			api.parameter("absorption.distance", p.getNextFloat());
		if( p.peekNextToken("absorption.color") || p.peekNextToken("absorbtion.color") )
			api.parameter("absorption.color",CString(),parseColor().getRGB());
		api.shader(name,"glass");
	} 
	else if(p.peekNextToken("shiny"))
	{
		CString tex;
		if(p.peekNextToken("texture"))
			api.parameter("texture",tex=p.getNextToken());
		else 
		{
			p.checkNextToken("diff");
			api.parameter("diffuse",CString(),parseColor().getRGB());
		}
		p.checkNextToken("refl");
		api.parameter("shiny",p.getNextFloat());
		if(tex.IsEmpty())
			api.shader(name,"shiny_diffuse");
		else
			api.shader(name,"textured_shiny_diffuse");
	} 
	else if(p.peekNextToken("ward"))
	{
		CString tex;
		if(p.peekNextToken("texture"))
			api.parameter("texture",tex=p.getNextToken());
		else 
		{
			p.checkNextToken("diff");
			api.parameter("diffuse",CString(),parseColor().getRGB());
		}
		p.checkNextToken("spec");
		api.parameter("specular",CString(),parseColor().getRGB());
		p.checkNextToken("rough");
		api.parameter("roughnessX",p.getNextFloat());
		api.parameter("roughnessY",p.getNextFloat());
		if(p.peekNextToken("samples"))
			api.parameter("samples",p.getNextInt());
		if(!tex.IsEmpty())
			api.shader(name,"textured_ward");
		else
			api.shader(name,"ward");
	} 
	else if(p.peekNextToken("view-caustics")) 
	{
		api.shader(name,"view_caustics");
	} 
	else if(p.peekNextToken("view-irradiance")) 
	{
		api.shader(name,"view_irradiance");
	}
	else if(p.peekNextToken("view-global")) 
	{
		api.shader(name,"view_global");
	}
	else if(p.peekNextToken("constant")) 
	{		
		p.peekNextToken("color");
		api.parameter("color",CString(),parseColor().getRGB());
		api.shader(name,"constant");
	} 
	else if(p.peekNextToken("id"))
	{
		api.shader(name,"show_instance_id");
	} 
	else if(p.peekNextToken("uber"))
	{
		if(p.peekNextToken("diff"))
			api.parameter("diffuse",CString(),parseColor().getRGB());
		if(p.peekNextToken("diff.texture"))
			api.parameter("diffuse.texture",p.getNextToken());
		if(p.peekNextToken("diff.blend"))
			api.parameter("diffuse.blend",p.getNextFloat());
		if(p.peekNextToken("refl") || p.peekNextToken("spec"))
			api.parameter("specular",CString(),parseColor().getRGB());
		if(p.peekNextToken("texture"))
		{			
			StrUtil::PrintPrompt("已废弃的uber shader参数\"texture\"，请使用 \"diffuse.texture\" 和 \"diffuse.blend\"替代");
			api.parameter("diffuse.texture",p.getNextToken());
			api.parameter("diffuse.blend",p.getNextFloat());
		}
		if(p.peekNextToken("spec.texture"))
			api.parameter("specular.texture",p.getNextToken());
		if(p.peekNextToken("spec.blend"))
			api.parameter("specular.blend",p.getNextFloat());
		if(p.peekNextToken("glossy"))
			api.parameter("glossyness",p.getNextFloat());
		if(p.peekNextToken("samples"))
			api.parameter("samples",p.getNextInt());
		api.shader(name,"uber");
	} 
	else
		StrUtil::PrintPrompt("未知的shader类型: %s",p.getNextToken());
	p.checkNextToken("}");

	return TRUE;
}

BOOL SCParser::parseModifier(LGAPIInterface&api)
{
	p.checkNextToken("{");
	p.checkNextToken("name");
	CString name=p.getNextToken();
	StrUtil::PrintPrompt("读取modifier: %s...",name);
	p.checkNextToken("type");
	if(p.peekNextToken("bump"))
	{
		p.checkNextToken("texture");
		api.parameter("texture",p.getNextToken());
		p.checkNextToken("scale");
		api.parameter("scale",p.getNextFloat());
		api.modifier(name,"bump_map");
	}
	else if(p.peekNextToken("normalmap")) 
	{
		p.checkNextToken("texture");
		api.parameter("texture",p.getNextToken());
		api.modifier(name,"normal_map");
	} 
	else if(p.peekNextToken("perlin")) 
	{
		p.checkNextToken("function");
		api.parameter("function",p.getNextInt());
		p.checkNextToken("size");
		api.parameter("size",p.getNextFloat());
		p.checkNextToken("scale");
		api.parameter("scale",p.getNextFloat());
		api.modifier(name,"perlin");
	} 
	else
	{
		StrUtil::PrintPrompt("未知的modifier类型: %s",p.getNextToken());
	}
	p.checkNextToken("}");

	return TRUE;
}

void SCParser::parseObjectBlock(LGAPIInterface&api)
{
	p.checkNextToken("{");
	BOOL noInstance=FALSE;
	vector<Matrix4> transform;
	float transformTime0=0,transformTime1=0;
	CString name;
	vector<CString> shaders;
	vector<CString> modifiers;
	if(p.peekNextToken("noinstance"))
	{		
		noInstance=TRUE;
	} 
	else
	{		
		if(p.peekNextToken("shaders")) 
		{
			int n=p.getNextInt();
			shaders.resize(n);
			for(int i=0; i<n; i++)
				shaders[i]=p.getNextToken();
		} 
		else 
		{
			p.checkNextToken("shader");
			shaders.push_back(p.getNextToken());
		}
		if(p.peekNextToken("modifiers")) 
		{
			int n=p.getNextInt();
			modifiers.resize(n);
			for(int i=0; i<n; i++)
				modifiers[i]=p.getNextToken();
		} 
		else if(p.peekNextToken("modifier"))
			modifiers.push_back(p.getNextToken());
		if(p.peekNextToken("transform")) 
		{
			if(p.peekNextToken("steps"))
			{
				transform.resize(p.getNextInt());
				p.checkNextToken("times");
				transformTime0=p.getNextFloat();
				transformTime1=p.getNextFloat();
				for(int i=0; i<transform.size(); i++)
					transform[i]=parseMatrix();
			} 
			else
				transform.push_back(parseMatrix());
		}		
	}
	if(p.peekNextToken("accel"))
		api.parameter("accel",p.getNextToken());
	p.checkNextToken("type");
	CString t=p.getNextToken();
	if(p.peekNextToken("name"))
		name=p.getNextToken();
	else
		name=generateUniqueName(t);
	if(t=="mesh")
	{		
		StrUtil::PrintPrompt("读取mesh: %s...",name);
		int numVertices=p.getNextInt();
		int numTriangles=p.getNextInt();
		vector<float> points;
		points.resize(numVertices*3);
		vector<float> normals;
		normals.resize(numVertices*3);
		vector<float> uvs;
		uvs.resize(numVertices*2);
		for(int i=0; i<numVertices; i++)
		{
			p.checkNextToken("v");
			points[3*i+0]=p.getNextFloat();
			points[3*i+1]=p.getNextFloat();
			points[3*i+2]=p.getNextFloat();
			normals[3*i+0]=p.getNextFloat();
			normals[3*i+1]=p.getNextFloat();
			normals[3*i+2]=p.getNextFloat();
			uvs[2*i+0]=p.getNextFloat();
			uvs[2*i+1]=p.getNextFloat();
		}
		vector<int> triangles;
		triangles.resize(numTriangles*3);
		for(int i=0; i<numTriangles; i++)
		{
			p.checkNextToken("t");
			triangles[i*3+0]=p.getNextInt();
			triangles[i*3+1]=p.getNextInt();
			triangles[i*3+2]=p.getNextInt();
		}		
		api.parameter("triangles",triangles);
		api.parameter("points","point","vertex",points);
		api.parameter("normals","vector","vertex",normals);
		api.parameter("uvs","texcoord","vertex",uvs);
		api.geometry(name,"triangle_mesh");
	} 
	else if(t=="flat-mesh")
	{		
		StrUtil::PrintPrompt("读取flat mesh: %s...",name);
		int numVertices=p.getNextInt();
		int numTriangles=p.getNextInt();
		vector<float> points;
		points.resize(numVertices*3);
		vector<float> uvs;
		uvs.resize(numVertices*2);
		for(int i=0; i<numVertices; i++)
		{
			p.checkNextToken("v");
			points[3*i+0]=p.getNextFloat();
			points[3*i+1]=p.getNextFloat();
			points[3*i+2]=p.getNextFloat();
			p.getNextFloat();
			p.getNextFloat();
			p.getNextFloat();
			uvs[2*i+0]=p.getNextFloat();
			uvs[2*i+1]=p.getNextFloat();
		}
		vector<int> triangles;
		triangles.resize(numTriangles*3);
		for(int i=0; i<numTriangles; i++)
		{
			p.checkNextToken("t");
			triangles[i*3+0]=p.getNextInt();
			triangles[i*3+1]=p.getNextInt();
			triangles[i*3+2]=p.getNextInt();
		}		
		api.parameter("triangles",triangles);
		api.parameter("points","point","vertex",points);
		api.parameter("uvs","texcoord","vertex",uvs);
		api.geometry(name,"triangle_mesh");
	} 
	else if(t=="sphere") 
	{
		StrUtil::PrintPrompt("读取sphere...");
		api.geometry(name,"sphere");
		if(transform.empty() && !noInstance) 
		{			
			p.checkNextToken("c");
			float x=p.getNextFloat();
			float y=p.getNextFloat();
			float z=p.getNextFloat();
			p.checkNextToken("r");
			float radius=p.getNextFloat();
			api.parameter("transform", 
				Matrix4::translation(x,y,z).multiply(Matrix4::scale(radius)));
			api.parameter("shaders",shaders);
			if(!modifiers.empty())
				api.parameter("modifiers",modifiers);
			api.instance(name+".instance",name);			
			noInstance=TRUE;
		}
	}
	else if(t=="cylinder")
	{
		StrUtil::PrintPrompt("读取cylinder...");
		api.geometry(name,"cylinder");
	}
	else if(t=="banchoff") 
	{
		StrUtil::PrintPrompt("读取banchoff...");
		api.geometry(name,"banchoff");
	} 
	else if(t=="torus")
	{
		StrUtil::PrintPrompt("读取torus...");
		p.checkNextToken("r");
		api.parameter("radiusInner",p.getNextFloat());
		api.parameter("radiusOuter",p.getNextFloat());
		api.geometry(name,"torus");
	} 
	else if(t=="sphereflake")
	{
		StrUtil::PrintPrompt("读取sphereflake...");
		if(p.peekNextToken("level"))
			api.parameter("level",p.getNextInt());
		if(p.peekNextToken("axis"))
			api.parameter("axis",parseVector());
		if(p.peekNextToken("radius"))
			api.parameter("radius",p.getNextFloat());
		api.geometry(name,"sphereflake");
	} 
	else if(t=="plane") 
	{
		StrUtil::PrintPrompt("读取plane...");
		p.checkNextToken("p");
		api.parameter("center",parsePoint());
		if(p.peekNextToken("n")) 
		{
			api.parameter("normal",parseVector());
		} 
		else 
		{
			p.checkNextToken("p");
			api.parameter("point1",parsePoint());
			p.checkNextToken("p");
			api.parameter("point2",parsePoint());
		}
		api.geometry(name,"plane");
	}
	else if(t=="generic-mesh") 
	{
		StrUtil::PrintPrompt("读取generic mesh: %s...",name);		
		p.checkNextToken("points");
		int np=p.getNextInt();
		api.parameter("points","point","vertex",parseFloatArray(np*3));		
		p.checkNextToken("triangles");
		int nt=p.getNextInt();
		api.parameter("triangles",parseIntArray(nt*3));	
		p.checkNextToken("normals");
		if(p.peekNextToken("vertex"))
			api.parameter("normals","vector","vertex",parseFloatArray(np*3));
		else if(p.peekNextToken("facevarying"))
			api.parameter("normals","vector","facevarying",parseFloatArray(nt*9));
		else
			p.checkNextToken("none");	
		p.checkNextToken("uvs");
		if(p.peekNextToken("vertex"))
			api.parameter("uvs","texcoord","vertex",parseFloatArray(np*2));
		else if(p.peekNextToken("facevarying"))
			api.parameter("uvs","texcoord","facevarying",parseFloatArray(nt*6));
		else
			p.checkNextToken("none");
		if(p.peekNextToken("face_shaders"))
			api.parameter("faceshaders",parseIntArray(nt));
		api.geometry(name,"triangle_mesh");
	} 
	else if(t=="hair")
	{
		StrUtil::PrintPrompt("读取hair曲线: %s...",name);
		p.checkNextToken("segments");
		api.parameter("segments",p.getNextInt());
		p.checkNextToken("width");
		api.parameter("widths",p.getNextFloat());
		p.checkNextToken("points");
		api.parameter("points","point","vertex",parseFloatArray(p.getNextInt()));
		api.geometry(name,"hair");
	}
	else if(t=="tesselatable") 
	{
		StrUtil::PrintPrompt("读取过程图素: %s...",name);
		CString tt=p.peekNextToken("typename")?
			p.getNextToken():PluginRegistry::shaderPlugins.generateUniqueName("shader");
		if(!PluginRegistry::tesselatablePlugins.registerPlugin(tt,p.getNextCodeBlock()))
			noInstance=TRUE;
		else
			api.geometry(name,tt);
	} 
	else if(t=="teapot")
	{
		StrUtil::PrintPrompt("读取teapot: %s...",name);
		if(p.peekNextToken("subdivs"))
			api.parameter("subdivs",p.getNextInt());
		if(p.peekNextToken("smooth"))
			api.parameter("smooth",p.getNextBoolean());
		api.geometry(name,"teapot");
	} 
	else if(t=="gumbo") 
	{
		StrUtil::PrintPrompt("读取gumbo: %s...",name);
		if(p.peekNextToken("subdivs"))
			api.parameter("subdivs",p.getNextInt());
		if(p.peekNextToken("smooth"))
			api.parameter("smooth",p.getNextBoolean());
		api.geometry(name,"gumbo");
	} 
	else if(t=="julia") 
	{
		StrUtil::PrintPrompt("读取julia分形: %s...",name);
		if(p.peekNextToken("q")) 
		{
			api.parameter("cw",p.getNextFloat());
			api.parameter("cx",p.getNextFloat());
			api.parameter("cy",p.getNextFloat());
			api.parameter("cz",p.getNextFloat());
		}
		if(p.peekNextToken("iterations"))
			api.parameter("iterations",p.getNextInt());
		if(p.peekNextToken("epsilon"))
			api.parameter("epsilon",p.getNextFloat());
		api.geometry(name,"julia");
	} 
	else if(t=="particles" || t=="dlasurface") 
	{
		if(t=="dlasurface")
			StrUtil::PrintPrompt("已废弃的对象类型:\"dlasurface\"，请使用\"particles\"替代");
		vector<float> data;
		if(p.peekNextToken("filename"))
		{			
			CString fileName=p.getNextToken();
			BOOL littleEndian=FALSE;
			if(p.peekNextToken("little_endian"))
				littleEndian=TRUE;
			StrUtil::PrintPrompt("载入particle文件: %s",fileName);
			CFile s;
			if( !s.Open(fileName,CFile::modeRead | CFile::typeBinary) )
				return;
            float dataFloat;
			while(s.Read(&dataFloat,sizeof(dataFloat))<s.GetLength())			
				data.push_back(dataFloat);
			s.Close();
		} 
		else 
		{
			p.checkNextToken("points");
			int n=p.getNextInt();
			data=parseFloatArray(n*3);
		}
		api.parameter("particles","point","vertex",data);
		if(p.peekNextToken("num"))
			api.parameter("num",p.getNextInt());
		else
			api.parameter("num",(int)data.size()/3);
		p.checkNextToken("radius");
		api.parameter("radius",p.getNextFloat());
		api.geometry(name,"particles");
	} 
	else if(t=="file-mesh")
	{
		StrUtil::PrintPrompt("读取file mesh: %s...",name);
		p.checkNextToken("filename");
		api.parameter("filename",p.getNextToken());
		if(p.peekNextToken("smooth_normals"))
			api.parameter("smooth_normals",p.getNextBoolean());
		api.geometry(name,"file_mesh");
	}
	else if(t=="bezier-mesh") 
	{
		StrUtil::PrintPrompt("读取bezier mesh: %s...",name);
		p.checkNextToken("n");
		int nu,nv;
		api.parameter("nu",nu=p.getNextInt());
		api.parameter("nv",nv=p.getNextInt());
		if(p.peekNextToken("wrap"))
		{
			api.parameter("uwrap",p.getNextBoolean());
			api.parameter("vwrap",p.getNextBoolean());
		}
		p.checkNextToken("points");
		vector<float> points;
		points.resize(3*nu*nv);
		for(int i=0; i<points.size(); i++)
			points[i]=p.getNextFloat();
		api.parameter("points","point","vertex",points);
		if(p.peekNextToken("subdivs"))
			api.parameter("subdivs",p.getNextInt());
		if(p.peekNextToken("smooth"))
			api.parameter("smooth",p.getNextBoolean());
		api.geometry(name,"bezier_mesh");
	}
	else if(t=="cornellbox")
	{
        parseCornellBox(api);
		api.geometry(name,"cornell_box");
	}
	else 
	{
		StrUtil::PrintPrompt("未知的对象类型: %s",p.getNextToken());
		noInstance=TRUE;
	}
	if(!noInstance) 
	{		
		api.parameter("shaders",shaders);
		if( !modifiers.empty() )
			api.parameter("modifiers",modifiers);
		if( !transform.empty() )
		{
			if(transform.size()==1)
				api.parameter("transform",transform[0]);
			else 
			{
				api.parameter("transform.steps",(int)transform.size());
                vector<float> data;
				data.push_back(transformTime0);
				data.push_back(transformTime1);
				api.parameter("transform.times","float","none",data);
				for(int i=0; i<transform.size(); i++)
				{
					CString str;
					str.Format(_T("transform[%d]"),i);
					api.parameter(str,transform[i]);
				}
			}
		}
		else
		{		
			api.parameter("transform",Matrix4::IDENTITY);
		}
		api.instance(name+".instance",name);
	}
	p.checkNextToken("}");
}

void SCParser::parseInstanceBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("name");
	CString name=p.getNextToken();
	StrUtil::PrintPrompt("读取instance: %s...",name);
	p.checkNextToken("geometry");
	CString geoname=p.getNextToken();
	p.checkNextToken("transform");
	if(p.peekNextToken("steps"))
	{
		int n=p.getNextInt();
		api.parameter("transform.steps",n);
		p.checkNextToken("times");
		vector<float> times;
	    times.resize(2);
		times[0]=p.getNextFloat();
		times[1]=p.getNextFloat();
		api.parameter("transform.times","float","none",times);
		for(int i=0; i<n; i++)
		{
			CString str;
			str.Format(_T("transform[%d]"),i);
			api.parameter(str,parseMatrix());
		}
	} 
	else
		api.parameter("transform",parseMatrix());
	vector<CString> shaders;
	if(p.peekNextToken("shaders")) 
	{
		int n=p.getNextInt();
		shaders.resize(n);
		for(int i=0; i<n; i++)
			shaders[i]=p.getNextToken();
	}
	else
	{
		p.checkNextToken("shader");
		shaders.push_back(p.getNextToken());
	}
	api.parameter("shaders",shaders);
	vector<CString> modifiers;
	if(p.peekNextToken("modifiers")) 
	{
		int n=p.getNextInt();
		modifiers.resize(n);
		for(int i=0; i<n; i++)
			modifiers[i]=p.getNextToken();
	}
	else if(p.peekNextToken("modifier"))
		modifiers.push_back(p.getNextToken());
	if(!modifiers.empty())
		api.parameter("modifiers",modifiers);
	api.instance(name,geoname);
	p.checkNextToken("}");
}

void SCParser::parseLightBlock(LGAPIInterface&api) 
{
	p.checkNextToken("{");
	p.checkNextToken("type");
	if(p.peekNextToken("mesh")) 
	{
		StrUtil::PrintPrompt("已废弃的light类型: mesh");
		p.checkNextToken("name");
		CString name=p.getNextToken();
		StrUtil::PrintPrompt("读取light mesh: %s...",name);
		p.checkNextToken("emit");
		api.parameter("radiance",CString(),parseColor().getRGB());
		int samples=numLightSamples;
		if(p.peekNextToken("samples"))
			samples=p.getNextInt();
		else
			StrUtil::PrintPrompt("未发现samples关键字，缺省为%d",samples);
		api.parameter("samples",samples);
		int numVertices=p.getNextInt();
		int numTriangles=p.getNextInt();
		vector<float> points;
		points.resize(3*numVertices);
		vector<int> triangles;
		triangles.resize(3*numTriangles);
		for(int i=0; i<numVertices; i++)
		{
			p.checkNextToken("v");
			points[3*i+0]=p.getNextFloat();
			points[3*i+1]=p.getNextFloat();
			points[3*i+2]=p.getNextFloat();		
		}
		for(int i=0; i<numTriangles; i++) 
		{
			p.checkNextToken("t");
			triangles[3*i+0]=p.getNextInt();
			triangles[3*i+1]=p.getNextInt();
			triangles[3*i+2]=p.getNextInt();
		}
		api.parameter("points","point","vertex",points);
		api.parameter("triangles",triangles);
		api.light(name,"triangle_mesh");
	}
	else if(p.peekNextToken("point")) 
	{
		StrUtil::PrintPrompt("读取point light...");
		Color pow;
		if(p.peekNextToken("color"))
		{
			pow=parseColor();
			p.checkNextToken("power");
			float po=p.getNextFloat();
			pow.mul(po);
		} 
		else 
		{
			StrUtil::PrintPrompt("已废弃的颜色指定，请使用color和power替代");
			p.checkNextToken("power");
			pow=parseColor();
		}
		p.checkNextToken("p");
		api.parameter("center",parsePoint());
		api.parameter("power",CString(),pow.getRGB());
		api.light(generateUniqueName("pointlight"),"point");
	} 
	else if(p.peekNextToken("spot"))
	{
		StrUtil::PrintPrompt("读取spot light...");
		p.checkNextToken("intensity");
        Color intensity=parseColor();
        p.checkNextToken("position");
		Point3 pos=parsePoint();
		p.checkNextToken("direction");
        Vector3 dir=parseVector();
        p.checkNextToken("minAngle");
		float minAngle=p.getNextFloat();
		p.checkNextToken("maxAngle");
		float maxAngle=p.getNextFloat();
		api.parameter("position",pos);
		api.parameter("direction",dir);
		api.parameter("intensity",CString(),intensity.getRGB());
		api.parameter("minAngle",minAngle);
		api.parameter("maxAngle",maxAngle);
        api.light(generateUniqueName("spotlight"),"spot");
	}
	else if(p.peekNextToken("spherical"))
	{
		StrUtil::PrintPrompt("读取spherical light...");
		p.checkNextToken("color");
		Color pow=parseColor();
		p.checkNextToken("radiance");
		pow.mul(p.getNextFloat());
		api.parameter("radiance",CString(),pow.getRGB());
		p.checkNextToken("center");
		api.parameter("center",parsePoint());
		p.checkNextToken("radius");
		api.parameter("radius",p.getNextFloat());
		p.checkNextToken("samples");
		api.parameter("samples",p.getNextInt());
		api.light(generateUniqueName("spherelight"),"sphere");
	}
	else if(p.peekNextToken("directional")) 
	{
		StrUtil::PrintPrompt("读取directional light...");
		p.checkNextToken("source");
		Point3 s=parsePoint();
		api.parameter("source",s);
		p.checkNextToken("target");
		Point3 t=parsePoint();
		api.parameter("dir",Point3::sub(t,s,Vector3()));
		p.checkNextToken("radius");
		api.parameter("radius",p.getNextFloat());
		p.checkNextToken("emit");
		Color e=parseColor();
		if(p.peekNextToken("intensity"))
		{
			float i=p.getNextFloat();
			e.mul(i);
		} 
		else
			StrUtil::PrintPrompt("已废弃的颜色指定，请使用emit和intensity替代");
		api.parameter("radiance",CString(),e.getRGB());
		api.light(generateUniqueName("dirlight"),"directional");
	} 
	else if(p.peekNextToken("ibl")) 
	{
		StrUtil::PrintPrompt("读取image based light...");
		p.checkNextToken("image");
		api.parameter("texture",p.getNextToken());
		p.checkNextToken("center");
		api.parameter("center", parseVector());
		p.checkNextToken("up");
		api.parameter("up",parseVector());
		p.checkNextToken("lock");
		api.parameter("fixed",p.getNextBoolean());
		int samples=numLightSamples;
		if(p.peekNextToken("samples"))
			samples=p.getNextInt();
		else
			StrUtil::PrintPrompt("未发现Samples关键字，缺省为%d",samples);
		api.parameter("samples",samples);
		if(p.peekNextToken("lowsamples"))
			api.parameter("lowsamples",p.getNextInt());
		else
			api.parameter("lowsamples",samples);
		api.light(generateUniqueName("ibl"),"ibl");
	}
	else if(p.peekNextToken("meshlight")) 
	{
		p.checkNextToken("name");
		CString name=p.getNextToken();
		StrUtil::PrintPrompt("读取meshlight: %s...",name);
		p.checkNextToken("emit");
		Color e=parseColor();
		if(p.peekNextToken("radiance")) 
		{
			float r=p.getNextFloat();
			e.mul(r);
		} 
		else
			StrUtil::PrintPrompt("已废弃的颜色指定，请使用emit和radiance替代");
		api.parameter("radiance",CString(),e.getRGB());
		int samples=numLightSamples;
		if(p.peekNextToken("samples"))
			samples=p.getNextInt();
		else
			StrUtil::PrintPrompt("未发现samples关键字，缺省为%d",samples);
		api.parameter("samples",samples);		
		p.checkNextToken("points");
		int np=p.getNextInt();
		api.parameter("points","point","vertex",parseFloatArray(np*3));		
		p.checkNextToken("triangles");
		int nt=p.getNextInt();
		api.parameter("triangles",parseIntArray(nt*3));
		api.light(name,"triangle_mesh");
	} 
	else if(p.peekNextToken("sunsky"))
	{
		p.checkNextToken("up");
		api.parameter("up",parseVector());
		p.checkNextToken("east");
		api.parameter("east",parseVector());
		p.checkNextToken("sundir");
		api.parameter("sundir",parseVector());
		p.checkNextToken("turbidity");
		api.parameter("turbidity",p.getNextFloat());
		if(p.peekNextToken("samples"))
			api.parameter("samples",p.getNextInt());
		if(p.peekNextToken("ground.extendsky"))
			api.parameter("ground.extendsky",p.getNextBoolean());
		else if(p.peekNextToken("ground.color"))
			api.parameter("ground.color",CString(),parseColor().getRGB());
		api.light(generateUniqueName("sunsky"),"sunsky");
	} 
	else if(p.peekNextToken("cornellbox"))
	{
	    parseCornellBox(api);
		api.light(generateUniqueName("cornellbox"),"cornell_box");	
	}
	else
		StrUtil::PrintPrompt("未知的对象类型: %s",p.getNextToken());
	p.checkNextToken("}");
}

void SCParser::parseCornellBox(LGAPIInterface&api)
{
	StrUtil::PrintPrompt("读取cornell box...");
	p.checkNextToken("corner0");
	api.parameter("corner0",parsePoint());
	p.checkNextToken("corner1");
	api.parameter("corner1",parsePoint());
	p.checkNextToken("left");
	api.parameter("leftColor",CString(),parseColor().getRGB());
	p.checkNextToken("right");
	api.parameter("rightColor",CString(),parseColor().getRGB());
	p.checkNextToken("top");
	api.parameter("topColor",CString(),parseColor().getRGB());
	p.checkNextToken("bottom");
	api.parameter("bottomColor",CString(),parseColor().getRGB());
	p.checkNextToken("back");
	api.parameter("backColor",CString(),parseColor().getRGB());
	p.checkNextToken("emit");
	api.parameter("radiance",CString(),parseColor().getRGB());
	if(p.peekNextToken("samples"))
		api.parameter("samples",p.getNextInt());	
}

Color SCParser::parseColor()
{
	if(p.peekNextToken("{")) 
	{
		CString space=p.getNextToken();
		int req=ColorFactory::getRequiredDataValues(space);
		if(req==-2) 
		{
			StrUtil::PrintPrompt("未知的颜色空间: %s",space);
			return Color();
		} 
		else if(req==-1) 
		{			
			req=p.getNextInt();
		}
		vector<float> data=parseFloatArray(req);        
		Color c=ColorFactory::createColor(space,data);
		p.checkNextToken("}");

		return c;
	} 
	else 
	{
		float r=p.getNextFloat();
		float g=p.getNextFloat();
		float b=p.getNextFloat();
		vector<float> data;
		data.push_back(r); data.push_back(g); data.push_back(b);

		return ColorFactory::createColor(CString(),data);
	}
}

Point3 SCParser::parsePoint()
{
	float x=p.getNextFloat();
	float y=p.getNextFloat();
	float z=p.getNextFloat();

	return Point3(x,y,z);
}

Vector3 SCParser::parseVector()
{
	float x=p.getNextFloat();
	float y=p.getNextFloat();
	float z=p.getNextFloat();

	return Vector3(x,y,z);
}

vector<int> SCParser::parseIntArray(int size) 
{
	vector<int> data;
	data.resize(size);
	for(int i=0; i<size; i++)
		data[i]=p.getNextInt();

	return data;
}

vector<float> SCParser::parseFloatArray(int size)
{
	vector<float> data;
	data.resize(size);
	for(int i=0; i<size; i++)
		data[i]=p.getNextFloat();

	return data;
}

Matrix4 SCParser::parseMatrix()
{
	if(p.peekNextToken("row")) 
	{
		vector<float> data=parseFloatArray(12);

		return Matrix4(&data[0],TRUE);
	} 
	else if(p.peekNextToken("col")) 
	{
		vector<float> data=parseFloatArray(12);

		return Matrix4(&data[0],FALSE);
	}
	else 
	{
		Matrix4 m=Matrix4::IDENTITY;
		p.checkNextToken("{");
		while(!p.peekNextToken("}")) 
		{
			Matrix4 t;
			if(p.peekNextToken("translate")) 
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				t=Matrix4::translation(x,y,z);
			} 
			else if(p.peekNextToken("scaleu"))
			{
				float s=p.getNextFloat();
				t=Matrix4::scale(s);
			} 
			else if(p.peekNextToken("scale")) 
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				t=Matrix4::scale(x,y,z);
			}
			else if(p.peekNextToken("rotatex"))
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateX((float)DegToRad(angle));
			} 
			else if(p.peekNextToken("rotatey"))
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateY((float)DegToRad(angle));
			}
			else if(p.peekNextToken("rotatez")) 
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateZ((float)DegToRad(angle));
			}
			else if(p.peekNextToken("rotate")) 
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				float angle=p.getNextFloat();
				t=Matrix4::rotate(x,y,z,(float)DegToRad(angle));
			} 
			else
				StrUtil::PrintPrompt("未知的变换类型: %s",p.getNextToken());
			if(t!=Matrix4())
			    m=t.multiply(m);
		}

		return m;
	}
}
