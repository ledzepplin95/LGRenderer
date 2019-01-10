#pragma once

#include "Plugins.h"
#include "PrimitiveList.h"
#include "CameraLens.h"
#include "GIEngine.h"
#include "Filter.h"
#include "AccelerationStructure.h"
#include "BucketOrder.h"
#include "Tesselatable.h"
#include "Shader.h"
#include "modifier.h"
#include "lightsource.h"
#include "CausticPhotonMapInterface.h"
#include "GlobalPhotonMapInterface.h"
#include "ImageSampler.h"
#include "SceneParser.h"
#include "BitmapReader.h"
#include "BitmapWriter.h"

class PluginRegistry
{
public:
	static Plugins<PrimitiveList> primitivePlugins;
	static Plugins<Tesselatable> tesselatablePlugins;
	static Plugins<Shader> shaderPlugins;
	static Plugins<Modifier> modifierPlugins;
	static Plugins<LightSource> lightSourcePlugins;
	static Plugins<CameraLens> cameraLensPlugins;

	static Plugins<AccelerationStructure> accelPlugins;
	static Plugins<BucketOrder> bucketOrderPlugins;
	static Plugins<Filter> filterPlugins;
	static Plugins<GIEngine> giEnginePlugins;
	static Plugins<CausticPhotonMapInterface> causticPhotonMapPlugins;
	static Plugins<GlobalPhotonMapInterface> globalPhotonMapPlugins;
	static Plugins<ImageSampler> imageSamplerPlugins;
	static Plugins<SceneParser> parserPlugins;
	static Plugins<BitmapReader> bitmapReaderPlugins;
	static Plugins<BitmapWriter> bitmapWriterPlugins;
	static void initial();

};
