#include "StdAfx.h"

#include "PluginRegistry.h"

class vinitPluginRegistry
{
public:
	vinitPluginRegistry(void);		
};
	

Plugins<PrimitiveList> PluginRegistry::primitivePlugins;
Plugins<Tesselatable> PluginRegistry::tesselatablePlugins;
Plugins<Shader> PluginRegistry::shaderPlugins;
Plugins<Modifier> PluginRegistry::modifierPlugins;
Plugins<LightSource> PluginRegistry::lightSourcePlugins;
Plugins<CameraLens> PluginRegistry::cameraLensPlugins;

Plugins<AccelerationStructure> PluginRegistry::accelPlugins;
Plugins<BucketOrder> PluginRegistry::bucketOrderPlugins;
Plugins<Filter> PluginRegistry::filterPlugins;
Plugins<GIEngine> PluginRegistry::giEnginePlugins;
Plugins<CausticPhotonMapInterface> PluginRegistry::causticPhotonMapPlugins;
Plugins<GlobalPhotonMapInterface> PluginRegistry::globalPhotonMapPlugins;
Plugins<ImageSampler> PluginRegistry::imageSamplerPlugins;
Plugins<SceneParser> PluginRegistry::parserPlugins;
Plugins<BitmapReader> PluginRegistry::bitmapReaderPlugins;
Plugins<BitmapWriter> PluginRegistry::bitmapWriterPlugins;
static vinitPluginRegistry ini;

vinitPluginRegistry::vinitPluginRegistry()
{
	PluginRegistry::initial();
}

void PluginRegistry::initial()
{
	primitivePlugins.registerPlugin("triangle_mesh","TriangleMesh");
	primitivePlugins.registerPlugin("sphere","Sphere");
	primitivePlugins.registerPlugin("cylinder","Cylinder");
	primitivePlugins.registerPlugin("box","Box");
	primitivePlugins.registerPlugin("banchoff","BanchoffSurface");
	primitivePlugins.registerPlugin("hair","Hair");
	primitivePlugins.registerPlugin("julia","JuliaFractal");
	primitivePlugins.registerPlugin("particles","ParticleSurface");
	primitivePlugins.registerPlugin("plane","Plane");
	primitivePlugins.registerPlugin("quad_mesh","QuadMesh");
	primitivePlugins.registerPlugin("torus","Torus");
	primitivePlugins.registerPlugin("background","Background");
	primitivePlugins.registerPlugin("sphereflake","SphereFlake");

	tesselatablePlugins.registerPlugin("bezier_mesh","BezierMesh");
	tesselatablePlugins.registerPlugin("file_mesh","FileMesh");
	tesselatablePlugins.registerPlugin("gumbo","Gumbo");
    tesselatablePlugins.registerPlugin("teapot","Teapot");

	shaderPlugins.registerPlugin("ambient_occlusion","AmbientOcclusionShader");
	shaderPlugins.registerPlugin("constant","ConstantShader");
	shaderPlugins.registerPlugin("diffuse","DiffuseShader");
	shaderPlugins.registerPlugin("glass","GlassShader");
	shaderPlugins.registerPlugin("mirror","MirrorShader");
	shaderPlugins.registerPlugin("phong","PhongShader");
	shaderPlugins.registerPlugin("shiny_diffuse","ShinyDiffuseShader");
	shaderPlugins.registerPlugin("uber","UberShader");
	shaderPlugins.registerPlugin("ward","AnisotropicWardShader");
	shaderPlugins.registerPlugin("wireframe","WireframeShader");

	shaderPlugins.registerPlugin("textured_ambient_occlusion","TexturedAmbientOcclusionShader");
	shaderPlugins.registerPlugin("textured_diffuse","TexturedDiffuseShader");
	shaderPlugins.registerPlugin("textured_phong","TexturedPhongShader");
	shaderPlugins.registerPlugin("textured_shiny_diffuse","TexturedShinyDiffuseShader");
	shaderPlugins.registerPlugin("textured_ward","TexturedWardShader");

	shaderPlugins.registerPlugin("quick_gray","QuickGrayShader");
	shaderPlugins.registerPlugin("simple","SimpleShader");
	shaderPlugins.registerPlugin("show_normals","NormalShader");
	shaderPlugins.registerPlugin("show_uvs","UVShader");
	shaderPlugins.registerPlugin("show_instance_id","IDShader");
	shaderPlugins.registerPlugin("show_primitive_id","PrimIDShader");
	shaderPlugins.registerPlugin("view_caustics","ViewCausticsShader");
	shaderPlugins.registerPlugin("view_global","ViewGlobalPhotonsShader");
	shaderPlugins.registerPlugin("view_irradiance","ViewIrradianceShader");

	modifierPlugins.registerPlugin("bump_map","BumpMappingModifier");
	modifierPlugins.registerPlugin("normal_map","NormalMapModifier");
	modifierPlugins.registerPlugin("perlin","PerlinModifier");

	lightSourcePlugins.registerPlugin("directional","DirectionalSpotLight");
	lightSourcePlugins.registerPlugin("ibl","ImageBasedLight");
	lightSourcePlugins.registerPlugin("point","PointLight");
	lightSourcePlugins.registerPlugin("spot","SpotLight");
	lightSourcePlugins.registerPlugin("sphere","SphereLight");
	lightSourcePlugins.registerPlugin("sunsky","SunSkyLight");
	lightSourcePlugins.registerPlugin("triangle_mesh","TriangleMeshLight");
	lightSourcePlugins.registerPlugin("cornell_box","CornellBox");

	cameraLensPlugins.registerPlugin("pinhole","PinholeLens");
	cameraLensPlugins.registerPlugin("thinlens","ThinLens");
	cameraLensPlugins.registerPlugin("fisheye","FisheyeLens");
	cameraLensPlugins.registerPlugin("spherical","SphericalLens");

	accelPlugins.registerPlugin("bih","BoundingIntervalHierarchy");
	accelPlugins.registerPlugin("kdtree","KDTree");
	accelPlugins.registerPlugin("null","NullAccelerator");
	accelPlugins.registerPlugin("uniformgrid","UniformGrid");

	bucketOrderPlugins.registerPlugin("column","ColumnBucketOrder");
	bucketOrderPlugins.registerPlugin("diagonal","DiagonalBucketOrder");
	bucketOrderPlugins.registerPlugin("hilbert","HilbertBucketOrder");
	bucketOrderPlugins.registerPlugin("random","RandomBucketOrder");
	bucketOrderPlugins.registerPlugin("row","RowBucketOrder");
	bucketOrderPlugins.registerPlugin("spiral","SpiralBucketOrder");

	filterPlugins.registerPlugin("blackman-harris","BlackmanHarrisFilter");
	filterPlugins.registerPlugin("box","BoxFilter");
	filterPlugins.registerPlugin("catmull-rom","CatmullRomFilter");
	filterPlugins.registerPlugin("gaussian","GaussianFilter");
	filterPlugins.registerPlugin("lanczos","LanczosFilter");
	filterPlugins.registerPlugin("mitchell","MitchellFilter");
	filterPlugins.registerPlugin("sinc","SincFilter");
	filterPlugins.registerPlugin("triangle","TriangleFilter");
	filterPlugins.registerPlugin("bspline","CubicBSpline");

	giEnginePlugins.registerPlugin("ambocc","AmbientOcclusionGIEngine");
	giEnginePlugins.registerPlugin("fake","FakeGIEngine");
	giEnginePlugins.registerPlugin("igi","InstantGI");
	giEnginePlugins.registerPlugin("irr-cache","IrradianceCacheGIEngine");
	giEnginePlugins.registerPlugin("path","PathTracingGIEngine");

	causticPhotonMapPlugins.registerPlugin("kd","CausticPhotonMap");

	globalPhotonMapPlugins.registerPlugin("grid","GridPhotonMap");
	globalPhotonMapPlugins.registerPlugin("kd","GlobalPhotonMap");

	imageSamplerPlugins.registerPlugin("bucket","BucketRenderer");
	imageSamplerPlugins.registerPlugin("ipr","ProgressiveRenderer");
	imageSamplerPlugins.registerPlugin("fast","SimpleRenderer");
	imageSamplerPlugins.registerPlugin("multipass","MultipassRenderer");

	parserPlugins.registerPlugin("lg","SCParser");
	parserPlugins.registerPlugin("sc","SCParser");
	parserPlugins.registerPlugin("sca","SCAsciiParser");
	parserPlugins.registerPlugin("scb","SCBinaryParser");
	parserPlugins.registerPlugin("rib","ShaveRibParser");
	parserPlugins.registerPlugin("ra2","RA2Parser");
	parserPlugins.registerPlugin("ra3","RA3Parser");

	bitmapReaderPlugins.registerPlugin("hdr","HDRBitmapReader");
	bitmapReaderPlugins.registerPlugin("tga","TGABitmapReader");
	bitmapReaderPlugins.registerPlugin("png","PNGBitmapReader");
	bitmapReaderPlugins.registerPlugin("jpg","JPGBitmapReader");
	bitmapReaderPlugins.registerPlugin("bmp","BMPBitmapReader");
	bitmapReaderPlugins.registerPlugin("igi","IGIBitmapReader");
	
	bitmapWriterPlugins.registerPlugin("png","PNGBitmapWriter");
	bitmapWriterPlugins.registerPlugin("hdr","HDRBitmapWriter");
	bitmapWriterPlugins.registerPlugin("tga","TGABitmapWriter");
	bitmapWriterPlugins.registerPlugin("exr","EXRBitmapWriter");
	bitmapWriterPlugins.registerPlugin("igi","IGIBitmapWriter");	
}