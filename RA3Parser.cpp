#include "StdAfx.h"

#include "RA3Parser.h"
#include "strutil.h"
#include "lgapiinterface.h"

LG_IMPLEMENT_DYNCREATE(RA3Parser,SceneParser)

BOOL RA3Parser::parse(CString fileName,LGAPIInterface&api) 
{
	try 
	{
		StrUtil::PrintPrompt("RA3 -读取几何数据\"%s\"...",fileName);

	    CFile s;
		if(!s.Open(fileName,CFile::modeRead | CFile::typeBinary) )
			return FALSE;

		int numVerts;
		int numTris;
		s.Read(&numVerts,sizeof(numVerts));
		s.Read(&numTris,sizeof(numTris));
		StrUtil::PrintPrompt("RA3 -   * 读取 %d 顶点...",numVerts);
		
		vector<float> verts;
		verts.resize(3*numVerts);
		for(int i=0; i<verts.size(); i++)
			s.Read(&verts[i],sizeof(float));
		StrUtil::PrintPrompt("RA3 -   * 读取 %d 三角形...",numTris);
		vector<int> tris;
		tris.resize(3*numTris);
		for(int i=0; i<tris.size(); i++)
			s.Read(&tris[i],sizeof(int));
		s.Close();

		StrUtil::PrintPrompt("RA3 -   * 创建mesh...");	
		api.parameter("triangles",tris);
		api.parameter("points","point","vertex",verts);
		api.geometry(fileName,"triangle_mesh");
		
		api.shader("ra3shader","simple");		
		api.parameter("shaders","ra3shader");
		api.instance(fileName+".instance",fileName);
	}
	catch(...) 
	{	
		return FALSE;
	}

	return TRUE;
}