#include "StdAfx.h"

#include "LGAPI.h"
#include "ColorFactory.h"
#include "Scene.h"
#include "PluginRegistry.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "timer.h"
#include "AsciiFileLGAPI.h"
#include "BinaryFileLGAPI.h"
#include "options.h"
#include "instance.h"
#include "BoundingBox.h"
#include "vector3.h"
#include "point2.h"
#include "Camera.h"
#include "Color.h"
#include "TriangleMesh.h"

CString LGAPI::VERSION("1.0.0.1");
CString LGAPI::DEFAULT_OPTIONS("::options");

LGAPI::LGAPI()
{
	reset();
}

LGAPI::~LGAPI()
{  
	if(scene) delete scene;

}

void LGAPI::reset() 
{
	scene=new Scene();
	includeSearchPath.Set("include");
	textureSearchPath.Set("texture");
	cFrame=1;	
}

void LGAPI::plugin(CString t,CString name,CString code) 
{
	if(t=="primitive")
		PluginRegistry::primitivePlugins.registerPlugin(name,code);
	else if(t=="tesselatable")
		PluginRegistry::tesselatablePlugins.registerPlugin(name,code);
	else if(t=="shader")
		PluginRegistry::shaderPlugins.registerPlugin(name,code);
	else if(t=="modifier")
		PluginRegistry::modifierPlugins.registerPlugin(name,code);
	else if(t=="camera_lens")
		PluginRegistry::cameraLensPlugins.registerPlugin(name,code);
	else if(t=="light")
		PluginRegistry::lightSourcePlugins.registerPlugin(name,code);
	else if(t=="accel")
		PluginRegistry::accelPlugins.registerPlugin(name,code);
	else if(t=="bucket_order")
		PluginRegistry::bucketOrderPlugins.registerPlugin(name,code);
	else if(t=="filter")
		PluginRegistry::filterPlugins.registerPlugin(name,code);
	else if(t=="gi_engine")
		PluginRegistry::giEnginePlugins.registerPlugin(name,code);
	else if(t=="caustic_photon_map")
		PluginRegistry::causticPhotonMapPlugins.registerPlugin(name,code);
	else if(t=="global_photon_map")
		PluginRegistry::globalPhotonMapPlugins.registerPlugin(name,code);
	else if(t=="image_sampler")
		PluginRegistry::imageSamplerPlugins.registerPlugin(name,code);
	else if(t=="parser")
		PluginRegistry::parserPlugins.registerPlugin(name,code);
	else if(t=="bitmap_reader")
		PluginRegistry::bitmapReaderPlugins.registerPlugin(name,code);
	else if(t=="bitmap_writer")
		PluginRegistry::bitmapWriterPlugins.registerPlugin(name,code);
	else
		StrUtil::PrintPrompt("不认识的插件类型:\"%s\" -忽略其声明\"%s\"",t,name);
}

void LGAPI::parameter(CString name,CString v) 
{
	parameterList.addString(name,v);
}

void LGAPI::parameter(CString name,boolean v) 
{
	parameterList.addBoolean(name,v);
}

void LGAPI::parameter(CString name,int v)
{
	parameterList.addInteger(name,v);
}

void LGAPI::parameter(CString name,float v)
{
	parameterList.addFloat(name,v);
}

void LGAPI::parameter(CString name,CString colorSpace,const vector<float>&data) 
{
	try 
	{
		parameterList.addColor(name,ColorFactory::createColor(colorSpace,data));
	}
	catch(...) 
	{
		StrUtil::PrintPrompt("不能指定颜色, 忽略参数\"%s\"",name);
	}
}

void LGAPI::parameter(CString name,const Point3&v) 
{
	vector<float> data;
	data.push_back(v.x); 
	data.push_back(v.y);
	data.push_back(v.z);
	parameterList.addPoints(name,ParameterList::IT_NONE,data);
}

void LGAPI::parameter(CString name,const Vector3&v)
{
	vector<float> data;
	data.push_back(v.x); 
	data.push_back(v.y);
	data.push_back(v.z);
	parameterList.addVectors(name,ParameterList::IT_NONE,data);	
}

void LGAPI::parameter(CString name,const Point2&v) 
{
	vector<float> data;
	data.push_back(v.x); 
	data.push_back(v.y);	
	parameterList.addTexCoords(name,ParameterList::IT_NONE,data);
}

void LGAPI::parameter(CString name,const Matrix4&v) 
{
	parameterList.addMatrices(name,ParameterList::IT_NONE,v.asRowMajor());
}

void LGAPI::parameter(CString name,const vector<int>&v) 
{
	parameterList.addIntegerArray(name,v);
}

void LGAPI::parameter(CString name,const vector<CString>&v)
{
	parameterList.addStringArray(name,v);
}

void LGAPI::parameter(CString name,CString t,CString interpolation,const vector<float>&data)
{
	ParameterList::InterpolationType interp;
	try
	{
		interp=ParameterList::GetInterpolationType(interpolation);			
	} 
	catch(...)
	{
		StrUtil::PrintPrompt("未知的插值类型: %s--忽略参数\"%s\"",interpolation,name);
		return;
	}
	t.MakeLower();
	if(t=="float")
		parameterList.addFloats(name,interp,data);
	else if(t=="point")
		parameterList.addPoints(name,interp,data);
	else if(t=="vector")
		parameterList.addVectors(name,interp,data);
	else if(t=="texcoord")
		parameterList.addTexCoords(name,interp,data);
	else if(t=="matrix")
		parameterList.addMatrices(name,interp,data);
	else
		StrUtil::PrintPrompt("未知的参数类型: %s--忽略参数\"%s\"",t,name);
}

void LGAPI::remove(CString name) 
{
	renderObjects.remove(name);
}

BOOL LGAPI::update(CString name) 
{
	BOOL success=renderObjects.update(name,parameterList,*this);
	parameterList.clear(success);

	return success;
}

void LGAPI::searchPath(CString t,CString path) 
{
	CString str1="include";
	CString str2="texture";
	if(t==str1)
		includeSearchPath.addSearchPath(path);
	else if(t==str2)
		textureSearchPath.addSearchPath(path);
	else
		StrUtil::PrintPrompt("无效的搜索路径类型: \"%s\"",t);
}

CString LGAPI::resolveTextureFilename(CString fileName) 
{
	return textureSearchPath.resolvePath(fileName);
}

CString LGAPI::resolveIncludeFilename(CString fileName) 
{
	return includeSearchPath.resolvePath(fileName);
}

void LGAPI::shader(CString name,CString shaderType)
{
	if(!isIncremental(shaderType))
	{	
		if(renderObjects.has(name))
		{
			StrUtil::PrintPrompt("不能声明shader\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}
		Shader *shader=PluginRegistry::shaderPlugins.createObject(shaderType,2);
		if(!shader) 
		{
			StrUtil::PrintPrompt("不能创建shader类型\"%s\"",shaderType);
			return;
		}
		renderObjects.put(name,shader);
	}	
	if(lookupShader(name)!=NULL)
		update(name);
	else 
	{
		StrUtil::PrintPrompt("不能更新shader\"%s\" -未发现shader对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::modifier(CString name,CString modifierType) 
{
	if(!isIncremental(modifierType)) 
	{		
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明modifier\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}
		Modifier* modifier=PluginRegistry::modifierPlugins.createObject(modifierType,2);
		if(!modifier) 
		{
			StrUtil::PrintPrompt("不能创建modifier类型\"%s\"",modifierType);
			return;
		}
		renderObjects.put(name,modifier);
	}	
	if(lookupModifier(name)!=NULL)
		update(name);
	else
	{
		StrUtil::PrintPrompt("不能更新modifier\"%s\" - 未发现modifier对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::geometry(CString name,CString typeName)
{
	if(!isIncremental(typeName))
	{
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明geometry\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}		
		if(PluginRegistry::tesselatablePlugins.hasType(typeName))
		{
			Tesselatable* tesselatable=
				PluginRegistry::tesselatablePlugins.createObject(typeName,2);
			if(tesselatable==NULL)
			{
				StrUtil::PrintPrompt("不能创建可三角化对象类型\"%s\"",typeName);
				return;
			}
			renderObjects.put(name,tesselatable);
		} 
		else
		{
			PrimitiveList* primitives=NULL;
			if(typeName=="triangle_mesh")
			{
				void*lgp=LGMemoryAllocator::lg_malloc(sizeof(TriangleMesh));
				TriangleMesh*tri=new (lgp) TriangleMesh();
				LGMemoryAllocator::lg_mem_triangle_pointer(tri);
				primitives=(PrimitiveList*)tri;
			}
			else
			{
				primitives=PluginRegistry::primitivePlugins.createObject(typeName,2);
			}			

			if(primitives==NULL)
			{
				StrUtil::PrintPrompt("不能创建图素类型\"%s\"",typeName);
				return;
			}
			renderObjects.put(name,primitives);
		}
	}
	if(lookupGeometry(name)!=NULL)
		update(name);
	else
	{
		StrUtil::PrintPrompt("不能更新几何体\"%s\" - 未发现几何对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::instance(CString name,CString geoName) 
{
	if(!isIncremental(geoName)) 
	{		
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明instance\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}
		parameter("geometry",geoName);		
		Instance*p=new Instance();
		LGMemoryAllocator::lg_mem_pointer(p);
		renderObjects.put(name,p);
	}
	if(lookupInstance(name)!=NULL)
		update(name);
	else 
	{
		StrUtil::PrintPrompt("不能更新instance\"%s\" -未发现instance对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::light(CString name,CString lightType) 
{
	if(!isIncremental(lightType))
	{		
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明光源\"%s\",名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}
		LightSource* light
			=PluginRegistry::lightSourcePlugins.createObject(lightType,2);
		if(light==NULL)
		{
			StrUtil::PrintPrompt("不能创建光源类型\"%s\"",lightType);
			return;
		}
		renderObjects.put(name,light);
	}
	if(lookupLight(name)!=NULL)
		update(name);
	else 
	{
		StrUtil::PrintPrompt("不能更新instance\"%s\" -未发现instance对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::camera(CString name,CString lensType) 
{
	if(!isIncremental(lensType))
	{
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明camera\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}
		CameraLens* lens
			=PluginRegistry::cameraLensPlugins.createObject(lensType);
		if(lens==NULL)
		{
			StrUtil::PrintPrompt("不能创建camera lens的类型\"%s\"",lensType);
			return;
		}		
		Camera*p=new Camera(lens);
		LGMemoryAllocator::lg_mem_pointer(p);
		renderObjects.put(name,p);
	}
	if(lookupCamera(name)!=NULL)
		update(name);
	else 
	{
		StrUtil::PrintPrompt("不能更新camera\"%s\" -未发现camera对象",name);
		parameterList.clear(TRUE);
	}
}

void LGAPI::options(CString name) 
{ 
	if(lookupOptions(name)==NULL) 
	{	
		if(renderObjects.has(name)) 
		{
			StrUtil::PrintPrompt("不能声明options\"%s\", 名字已使用",name);
			parameterList.clear(TRUE);
			return;
		}			
		Options*p=new Options();
		LGMemoryAllocator::lg_mem_pointer(reinterpret_cast<LGObject*>(p));
		renderObjects.put(name,p);
	}
    ASSERT(lookupOptions(name)!=NULL);
    update(name);
}

BOOL LGAPI::isIncremental(CString typeName) const
{
	return typeName.IsEmpty() || typeName=="incremental";
}

Geometry* LGAPI::lookupGeometry(CString name) const
{
	return renderObjects.lookupGeometry(name);
}

Instance* LGAPI::lookupInstance(CString name) const
{
	return renderObjects.lookupInstance(name);
}

Camera* LGAPI::lookupCamera(CString name) const
{
	return renderObjects.lookupCamera(name);
}

Options* LGAPI::lookupOptions(CString name) const
{
	return renderObjects.lookupOptions(name);
}

Shader* LGAPI::lookupShader(CString name) const
{
	return renderObjects.lookupShader(name);
}

Modifier* LGAPI::lookupModifier(CString name) const
{
	return renderObjects.lookupModifier(name);
}

LightSource* LGAPI::lookupLight(CString name) const
{
	return renderObjects.lookupLight(name);
}

void LGAPI::render(CString optionsName,Display*display)
{
	renderObjects.updateScene(scene);
	Options* opt=lookupOptions(optionsName);
	if(opt==NULL)
	{	
		opt=new Options();
		LGMemoryAllocator::lg_mem_pointer(reinterpret_cast<LGObject*>(opt));
	}
	scene->setCamera(lookupCamera(opt->getString("camera",CString())));
	
	CString shaderOverrideName=opt->getString("override.shader","none");
	BOOL overridePhotons=opt->getBoolean("override.photons",FALSE);

	if(shaderOverrideName=="none")
		scene->setShaderOverride(NULL,FALSE);
	else 
	{
		Shader*shader=lookupShader(shaderOverrideName);
		if(shader==NULL)
			StrUtil::PrintPrompt("不能找到覆盖的shader\"%s\"，禁用",shaderOverrideName);
		scene->setShaderOverride(shader,overridePhotons);
	}
	
	CString bakingInstanceName=opt->getString("baking.instance",CString());
	if(!bakingInstanceName.IsEmpty())
	{
		Instance* bakingInstance=lookupInstance(bakingInstanceName);
		if(bakingInstance==NULL)
		{
			StrUtil::PrintPrompt("不能烘焙instance\"%s\" -未找到",bakingInstanceName);
			return;
		}
		scene->setBakingInstance(bakingInstance);
	} 
	else
		scene->setBakingInstance(NULL);

	ImageSampler* sampler
		=PluginRegistry::imageSamplerPlugins.createObject(
		opt->getString("sampler","bucket"),2);
	scene->render(opt,sampler,display);
}

BoundingBox LGAPI::getBounds()const 
{
	return scene->getBounds();
}

BOOL LGAPI::include(CString fileName)
{
	if(fileName.IsEmpty()) return FALSE;
	fileName=includeSearchPath.resolvePath(fileName);
	CString extension=StrUtil::trimExtensionLeft(
		FileUtil::getExtension(fileName));	
	SceneParser*parser=PluginRegistry::parserPlugins.createObject(extension,2);
	if(parser==NULL) 
	{
		StrUtil::PrintPrompt("不能发现文件\"%s\"合适的parser，(扩展: %s)", 
			fileName,extension);
		return FALSE;
	}
	CString currentFolder=StrUtil::GetFileDir(fileName);
	includeSearchPath.addSearchPath(currentFolder);
	textureSearchPath.addSearchPath(currentFolder);

	return parser->parse(fileName,*this);	
}

LGAPI* LGAPI::create(CString fileName,int frameNumber) 
{
	if(fileName.IsEmpty()) return new LGAPI();
	LGAPI* api=new LGAPI();
	if(!api->include(fileName))
	{
		delete api;
		return new LGAPI();
	}

	return api;
}

BOOL LGAPI::translate(CString fileName,CString outputFilename) 
{
	FileLGAPI*api=NULL;
	try 
	{
		if(outputFilename.Find(".sca")==outputFilename.GetLength()-4)
			api=new AsciiFileLGAPI(outputFilename);
		else if(outputFilename.Find(".scb")==outputFilename.GetLength()-4)
			api=new BinaryFileLGAPI(outputFilename);
		else 
		{
			StrUtil::PrintPrompt("不能确定输出文件类型: \"%s\"",outputFilename);
			return FALSE;
		}
	} 
	catch(...) 
	{
		StrUtil::PrintPrompt("不能创建输出文件");
		return FALSE;
	}
	std::string s(fileName);
	int start=s.find_last_of('.')+1;
	CString extension=StrUtil::trimExtensionLeft(
		StrUtil::subString(fileName,start));
	SceneParser* p=PluginRegistry::parserPlugins.createObject(extension);
	if(p==NULL) 
	{
		StrUtil::PrintPrompt("不能为文件: \"%s\"找到合适的parser",fileName);
		return FALSE;
	}
	try
	{
		return p->parse(fileName,*api);
	}
	catch(...) 
	{		
		StrUtil::PrintPrompt("解释过程中发生错误");
		return FALSE;
	}
	api->close();
	if(!api) delete api;

	return TRUE;
}

int LGAPI::currentFrame()const 
{
	return cFrame;
}

void LGAPI::currentFrame(int cf)
{
	cFrame=cf;
}

LGAPI& LGAPI::operator=(const LGAPI&api)
{
	if(this==&api) return *this;

    scene=api.scene;
	includeSearchPath=api.includeSearchPath;
	textureSearchPath=api.textureSearchPath;
	parameterList=api.parameterList;
	renderObjects=api.renderObjects;
    cFrame=api.cFrame;

	return *this;
}

