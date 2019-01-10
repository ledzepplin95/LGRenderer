#include "StdAfx.h"

#include "TriParser.h"
#include "strutil.h"
#include "parser.h"
#include "lgapiinterface.h"

LG_IMPLEMENT_DYNCREATE(TriParser,SceneParser)

BOOL TriParser::parse(CString fileName,LGAPIInterface&api) 
{
	try 
	{
		StrUtil::PrintPrompt("TRI - 读取几何体数据: \"%s\"...",fileName);
		Parser p(fileName);
		vector<float> verts;
		verts.resize(3*p.getNextInt());
		for(int v=0; v<verts.size(); v+=3) 
		{
			verts[v+0]=p.getNextFloat();
			verts[v+1]=p.getNextFloat();
			verts[v+2]=p.getNextFloat();
			p.getNextToken();
			p.getNextToken();
		}
		vector<int> triangles;
		triangles.resize(p.getNextInt()*3);
		for(int t=0; t<triangles.size(); t+=3)
		{
			triangles[t+0]=p.getNextInt();
			triangles[t+1]=p.getNextInt();
			triangles[t+2]=p.getNextInt();
		}
		
		api.parameter("triangles",triangles);
		api.parameter("points","point","vertex",verts);
		api.geometry(fileName,"triangle_mesh");		
		api.shader(fileName+".shader","simple");
		api.parameter("shaders",fileName+".shader");		
		api.instance(fileName+".instance",fileName);
		p.close();
			
		CFile s;
		CString a_fileName=fileName;
		a_fileName.Replace(".tri",".ra3");       
		if( !s.Open(a_fileName,CFile::modeReadWrite | CFile::typeBinary) )
			return FALSE;
        int data;		
		data=verts.size()/3;
        s.Write(&data,sizeof(data));
		data=triangles.size()/3;
        s.Write(&data,sizeof(data));
		for(int i=0; i<verts.size(); i++)
		    s.Write(&verts[i],sizeof(float));
		for(int i=0; i<triangles.size(); i++)			
			s.Write(&triangles[i],sizeof(int));
	} 
	catch(...)
	{	
		return FALSE;
	}

	return TRUE;
}