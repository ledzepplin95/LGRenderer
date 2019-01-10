#pragma once
#include "beziermesh.h"

class Teapot :
	public BezierMesh
{
	LG_DECLARE_DYNCREATE(Teapot);

public:
	Teapot();

private:
	static float PATCHES[][48];
};
