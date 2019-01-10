#include "StdAfx.h"

#include "FileMesh.h"
#include "MathUtil.h"
#include "StrUtil.h"
#include "FloatArray.h"
#include "IntArray.h"
#include "Memory.h"
#include "BoundingBox.h"
#include "Trianglemesh.h" 
#include "lgapi.h"
#include "vector3.h"

LG_IMPLEMENT_DYNCREATE(FileMesh,Tesselatable)

FileMesh::FileMesh()
{
	smoothNormals=FALSE;
}

BoundingBox FileMesh::getWorldBounds(const Matrix4&o2w) const
{
	return BoundingBox(0.0f);
}

PrimitiveList* FileMesh::tesselate()const
{
	if(fileName.Find(".ra3")==fileName.GetLength()-4) 
	{
		try 
		{
			StrUtil::PrintPrompt("RA3 - 读取几何数据:\"%s\"...",fileName);
			CFile s;
			if( !s.Open(fileName,CFile::modeRead | CFile::typeBinary) )
				return NULL;
			int numVerts,numTris;
			s.Read(&numVerts,sizeof(numVerts));
			s.Read(&numTris,sizeof(numTris));
			StrUtil::PrintPrompt("RA3 -   * 读取%d个顶点...",numVerts);
			vector<float> verts;
			verts.resize(3*numVerts);
			s.Read(&verts[0],sizeof(float)*verts.size());
			StrUtil::PrintPrompt("RA3 -   * 读取%d个三角形...",numTris);
			vector<int> tris;
			tris.resize(3*numTris);
			s.Read(&tris[0],sizeof(int)*tris.size());		
			s.Close();
			StrUtil::PrintPrompt("RA3 -   * 创建mesh...");

			return generate(tris,verts,smoothNormals);
		} 
		catch(...)
		{			
			StrUtil::PrintPrompt("不能读取mesh文件 \"%s\" -文件未找到",fileName);
		} 		
	} 
	else if(fileName.Find(".obj")==fileName.GetLength()-4) 
	{
		int lineNumber=1;
		try 
		{
			StrUtil::PrintPrompt("OBJ - 读取几何数据: \"%s\"...",fileName);

			FloatArray verts;
			IntArray tris;		
			CFile file;
			if( !file.Open(fileName,CFile::modeRead | CFile::typeBinary) )
				return NULL;
			CArchive bf(&file,CArchive::load);
			CString line;
			while( bf.ReadString(line) )
			{
				if(line.Find("v")==0)
				{
					vector<CString> v=StrUtil::split(line);
					verts.add(atof(v[1]));
					verts.add(atof(v[2]));
					verts.add(atof(v[3]));
				} 
				else if(line.Find("f")==0) 
				{
					vector<CString> f=StrUtil::split(line);
					if(f.size()==5) 
					{
						tris.add(atoi(f[1])-1);
						tris.add(atoi(f[2])-1);
						tris.add(atoi(f[3])-1);
						tris.add(atoi(f[1])-1);
						tris.add(atoi(f[3])-1);
						tris.add(atoi(f[4])-1);
					} 
					else if(f.size()==4)
					{
						tris.add(atoi(f[1])-1);
						tris.add(atoi(f[2])-1);
						tris.add(atoi(f[3])-1);
					}
				}
				if(lineNumber%100000==0)
					StrUtil::PrintPrompt("OBJ -   * 已解析 %7d 行 ...",lineNumber);
				lineNumber++;
			}
			bf.Flush();
			bf.Close();
			file.Close();
			StrUtil::PrintPrompt("OBJ -   * 创建mesh ...");

			return generate(tris.trim(),verts.trim(),smoothNormals);
		} 
		catch(...)
		{
			StrUtil::PrintPrompt("不能读mesh文件 \"%s\" - 文件未找到",fileName);
		} 	
	} 
	else if(fileName.Find(".stl")==fileName.GetLength()-4) 
	{
		try 
		{
			StrUtil::PrintPrompt("STL - 读取几何数据: \"%s\"...",fileName);

			CFile s;
			if( !s.Open(fileName,CFile::modeRead | CFile::typeBinary) )
				return NULL;			
			s.Seek(80,CFile::begin);	
			int intTmp;
			s.Read(&intTmp,sizeof(intTmp));
			int numTris=getLittleEndianInt(intTmp);
			StrUtil::PrintPrompt("STL -   * 读取%d三角形...",numTris);
			long filesize=s.GetLength();
			if( filesize!=(84+50*numTris) ) 
			{
				StrUtil::PrintPrompt("STL - 文件大小不匹配(期望%s, 发现 %s)", 
					Memory::bytesToString(84+14*numTris),Memory::bytesToString(filesize));

				return NULL;
			}

			vector<int> tris;
			tris.resize(3*numTris);
			vector<float> verts;
			verts.resize(9*numTris);
			for(int i=0,i3=0,index=0; i<numTris; i++,i3+=3)
			{			
				s.Read(&intTmp,sizeof(intTmp));
				s.Read(&intTmp,sizeof(intTmp));
				s.Read(&intTmp,sizeof(intTmp));
				for(int j=0; j<3; j++, index+=3 )
				{
					tris[i3+j]=i3+j;
					s.Read(&intTmp,sizeof(intTmp));
					verts[index+0]=getLittleEndianFloat(intTmp);
					s.Read(&intTmp,sizeof(intTmp));
					verts[index+1]=getLittleEndianFloat(intTmp);
					s.Read(&intTmp,sizeof(intTmp));
					verts[index+2]=getLittleEndianFloat(intTmp);
				}
				short shortTmp;
				s.Read(&shortTmp,sizeof(shortTmp));
				if( (i+1)%100000==0 )
					StrUtil::PrintPrompt("STL -   * 已解析%7d个三角形...",i+1);
			}
			s.Close();
			
			StrUtil::PrintPrompt("STL -   * 创建mesh ...");
			if(smoothNormals)
				StrUtil::PrintPrompt("STL - 格式不支持共享顶点- 法向光滑关闭");

			return generate(tris,verts,FALSE);
		} 
		catch(...) 
		{		
			StrUtil::PrintPrompt("不能读mesh文件 \"%s\" - 文件未找到",fileName);
		}	
	}
	else
		StrUtil::PrintPrompt("不能读mesh文件 \"%s\" - 未知格式",fileName);

    return NULL;
}

TriangleMesh* FileMesh::generate(const vector<int>&tris,const vector<float>&verts,
								 BOOL smoothNormals)const 
{
	ParameterList pl;
	pl.addIntegerArray("triangles",tris);
	pl.addPoints("points",ParameterList::IT_VERTEX,verts);
	if(smoothNormals) 
	{
		vector<float> normals;
		normals.resize(verts.size());
		Point3 p0;
		Point3 p1;
		Point3 p2;
		Vector3 n;
		for(int i3=0; i3<tris.size(); i3+=3)
		{
			int v0=tris[i3+0];
			int v1=tris[i3+1];
			int v2=tris[i3+2];
			p0.set(verts[3*v0+0],verts[3*v0+1],verts[3*v0+2]);
			p1.set(verts[3*v1+0],verts[3*v1+1],verts[3*v1+2]);
			p2.set(verts[3*v2+0],verts[3*v2+1],verts[3*v2+2]);
			Point3::normal(p0,p1,p2,n);
		
			normals[3*v0+0]+=n.x;
			normals[3*v0+1]+=n.y;
			normals[3*v0+2]+=n.z;
			normals[3*v1+0]+=n.x;
			normals[3*v1+1]+=n.y;
			normals[3*v1+2]+=n.z;
			normals[3*v2+0]+=n.x;
			normals[3*v2+1]+=n.y;
			normals[3*v2+2]+=n.z;
		}	
		for(int i3=0; i3<normals.size(); i3+=3)
		{
			n.set(normals[i3+0],normals[i3+1],normals[i3+2]);
			n.normalize();
			normals[i3+0]=n.x;
			normals[i3+1]=n.y;
			normals[i3+2]=n.z;
		}
		pl.addVectors("normals",ParameterList::IT_VERTEX,normals);
	}		
	TriangleMesh* m=new TriangleMesh();	
	LGMemoryAllocator::lg_mem_pointer(m);
	if(m->update(pl,LGAPI()))
		return m;
	
	return NULL;
}

BOOL FileMesh::update(ParameterList&pl,LGAPI&api)
{
	CString file=pl.getString("filename",CString());
	if(!file.IsEmpty())
		fileName=api.resolveIncludeFilename(file);
	smoothNormals=pl.getBoolean("smooth_normals",smoothNormals);

	return !fileName.IsEmpty();
}

int FileMesh::getLittleEndianInt(int i) 
{
	return ((unsigned int)i>>24) | (((unsigned int)i>>8)&0xFF00) 
		| ((i<<8)&0xFF0000) | (i<<24);
}

float FileMesh::getLittleEndianFloat(int i)
{	
	return MathUtil::intBitsToFloat(getLittleEndianInt(i));
}