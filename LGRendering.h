#pragma once

extern "C" __declspec( dllexport ) void renderBegin();
extern "C" __declspec( dllexport ) BOOL LGRender(const char*path);
extern "C" __declspec( dllexport ) void renderEnd();
extern "C" __declspec( dllexport ) void benchMarkTest();
extern "C" __declspec( dllexport ) void realtimeRenderTest();