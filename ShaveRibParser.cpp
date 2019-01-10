#include "StdAfx.h"

#include "ShaveRibParser.h"
#include "StrUtil.h"
#include "IntArray.h"
#include "FloatArray.h"
#include "parser.h"
#include "lgapiinterface.h"

LG_IMPLEMENT_DYNCREATE(ShaveRibParser,SceneParser)

BOOL ShaveRibParser::parse(CString fileName,LGAPIInterface&api)
{
	try
	{
		Parser p(fileName);
		p.checkNextToken("version");
		p.checkNextToken("3.04");
		p.checkNextToken("TransformBegin");

		if(p.peekNextToken("Procedural")) 
		{
			BOOL done=FALSE;
			while(!done) 
			{
				p.checkNextToken("DelayedReadArchive");
				p.checkNextToken("[");
				CString f=p.getNextToken();
				StrUtil::PrintPrompt("RIB - 读取像素: \"%s\"...",f);
				api.include(f);
				p.checkNextToken("]");
				while(TRUE)
				{
					CString t=p.getNextToken();
					if(t.IsEmpty() || t=="TransformEnd") 
					{
						done=TRUE;
						break;
					} 
					else if(t=="Procedural")
						break;
				}
			}
			return TRUE;
		}

		BOOL cubic=FALSE;
		if(p.peekNextToken("Basis"))
		{
			cubic=TRUE;
		
			p.checkNextToken("catmull-rom");
			p.checkNextToken("1");
			
			p.checkNextToken("catmull-rom");
			p.checkNextToken("1");
		}
		while(p.peekNextToken("Declare")) 
		{
			p.getNextToken(); 
			p.getNextToken();
		}
		int index=0;
		BOOL done=FALSE;
		p.checkNextToken("Curves");
		do 
		{
			if(cubic)
				p.checkNextToken("cubic");
			else
				p.checkNextToken("linear");
			vector<int> nverts=parseIntArray(p);
			for(int i=1; i<nverts.size(); i++) 
			{
				if (nverts[0]!=nverts[i]) 
				{
					StrUtil::PrintPrompt("RIB -发现hair段数变化");
					return FALSE;
				}
			}
			int nhairs=nverts.size();

			StrUtil::PrintPrompt("RIB -解析%d hair曲线", nhairs);

			api.parameter("segments",nverts[0]-1);

			p.checkNextToken("nonperiodic");
			p.checkNextToken("P");
			vector<float> points=parseFloatArray(p);
			if(points.size()!=3*nhairs*nverts[0]) 
			{
				StrUtil::PrintPrompt("RIB - 无效的点数 -期望%d - 发现 %d", 
					nhairs*nverts[0],points.size()/3);
				return FALSE;
			}
			api.parameter("points","point","vertex",points);

			StrUtil::PrintPrompt("RIB - 解析%d个hair顶点",points.size()/3);

			p.checkNextToken("width");
			vector<float> w=parseFloatArray(p);
			if(w.size()!=nhairs*nverts[0]) 
			{
				StrUtil::PrintPrompt("RIB - 无效的hair宽度数- 期望 %d - 发现 %d",
					nhairs*nverts[0],w.size());
				return FALSE;
			}
			api.parameter("widths","float","vertex",w);

			StrUtil::PrintPrompt("RIB - 解析%d个hair宽度",w.size());

			CString name;
		    name.Format(_T("%s[%d]"),fileName,index);
			StrUtil::PrintPrompt("RIB - 创建hair对象\"%s\"",name);
			api.geometry(name,"hair");
			api.instance(name+".instance",name);

			StrUtil::PrintPrompt("RIB -查找下一曲线组...");
			while(TRUE) 
			{
				CString t=p.getNextToken();
				if( t.IsEmpty() || t=="TransformEnd" ) 
				{
					done=TRUE;
					break;
				} 
				else if(t=="Curves")
					break;
			}
			index++;
		} while(!done);
		StrUtil::PrintPrompt("RIB -读取rib文件完成");
	} 
	catch(...) 
	{
		StrUtil::PrintPrompt("RIB解析异常: %s",fileName);
		
		return FALSE;
	} 

	return TRUE;
}

vector<int> ShaveRibParser::parseIntArray(Parser&p)
{
	IntArray a;
	BOOL done=FALSE;
	do 
	{
		CString s=p.getNextToken();
		if(s.Find("[")==0)
			s=StrUtil::subString(s,1);
		if(s.Find("]")==s.GetLength()-1) 
		{
			s=StrUtil::subString(s,0,s.GetLength()-1);
			done=TRUE;
		}
		a.add(atoi(s));
	} while(!done);

	return a.trim();
}

vector<float> ShaveRibParser::parseFloatArray(Parser&p)
{
	FloatArray a;
    BOOL done=FALSE;
	do 
	{
		CString s=p.getNextToken();
		if(s.Find("[")==0)
			s=StrUtil::subString(s,1);
		if(s.Find("]")==s.GetLength()-1) 
		{
			s=StrUtil::subString(s,0,s.GetLength()-1);
			done=TRUE;
		}
		a.add(atof(s));
	} while(!done);

	return a.trim();
}