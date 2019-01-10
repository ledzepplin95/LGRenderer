#pragma once

#include "beziermesh.h"

class Gumbo :
	public BezierMesh
{
	LG_DECLARE_DYNCREATE(Gumbo);

public:
	Gumbo();

	static void generate(const vector<CString>&args);

private:
	static float PATCHES[][49];

	static vector<float> parseFloatArray(Parser&p);	
};
