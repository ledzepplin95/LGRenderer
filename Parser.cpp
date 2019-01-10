#include "StdAfx.h"

#include "Parser.h"

Parser::Parser(void)
{	
	m_index=0;
}

Parser::~Parser(void)
{
	if( !m_file.m_hFile==(int)CFile::hFileNull )
		m_file.Close();
}

Parser::Parser(const CString &fileName)
{
	if( !m_file.Open(fileName,
		CFile::modeRead | CFile::typeText) ) return;
    m_index=0;
}

CString Parser::getFileName()const
{
	return m_file.GetFilePath();
}

void Parser::Set(const CString&fileName)
{
	if( !m_file.Open(fileName,
		CFile::modeRead | CFile::typeText) ) return;
	m_index=0;
	m_lineTokens.clear();
}

Parser& Parser::operator=(Parser&src)
{
	m_index=src.m_index;
	CString fileName=src.m_file.GetFilePath();
	if(fileName.IsEmpty()) return *this;
	if( !src.m_file.m_hFile==(int)CFile::hFileNull )
	{
		src.m_file.Close();
		return *this;
	}
	if( !m_file.Open(fileName,
		CFile::modeRead | CFile::typeText) ) return *this;

	return *this;
}

void Parser::close()
{
    m_file.Close();
}

BOOL Parser::getNextLine()
{
	CString line;
LOOP:;
	if( !m_file.ReadString(line) ) return FALSE;
	if(line.IsEmpty()) goto LOOP;
	
	CStrArray tokenList;
	CString current;
	BOOL inQuotes=FALSE;

	for(int i=0; i<line.GetLength(); i++)
	{
		UINT uChar=(line[i]&0xff);
		if( (uChar&0x80) && (i+1<line.GetLength()) )
		{		    
			current.AppendChar(line[i]);
			i++;
			current.AppendChar(line[i]);
			continue;
		}
		
		char c=line.GetAt(i);
		if(current.GetLength()==0 && ( c=='%' || c=='#' ) )
			break;
		BOOL quote=c=='\"';
		inQuotes=inQuotes^quote;
		if( !quote && ( inQuotes || !isspace(c) ) )
			current+=c;
		else if(current.GetLength()>0)
		{
			tokenList.push_back(current);
			current.Empty();
		}
	}
	if(current.GetLength()>0)
		tokenList.push_back(current);
	m_lineTokens=tokenList;
	m_index=0;

	return TRUE;
}

CString Parser::fetchNextToken()
{
	CString empty;
	while(TRUE)
	{
		if( m_index<m_lineTokens.size() )
			return m_lineTokens[m_index++];
		else if( !getNextLine() )
			return empty;
	}
}

CString Parser::getNextToken()
{
	while(TRUE) 
	{
		CString tok=fetchNextToken();
		if(tok.IsEmpty())
			return tok;
		CString str1="/*";
		CString str2="*/";
		if(tok==str1) 
		{
			do
			{
				tok=fetchNextToken();
				if(tok.IsEmpty())
					return tok;
			} 
			while(!tok==str2);
		} 
		else
			return tok;
	}
}

BOOL Parser::peekNextToken(const CString&tok)
{
	while(TRUE) 
	{
		CString t=fetchNextToken();
		if(t.IsEmpty())
			return FALSE;
		CString str1="/*";
		CString str2="*/";
		if(t==str1) 
		{
			do 
			{
				t=fetchNextToken();
				if(t.IsEmpty())
					return FALSE; 
			} 
			while(!t==str2);
		}
		else if(t==tok)
		{		
			return TRUE;
		} 
		else 
		{		
			m_index--;
			return FALSE;
		}
	}
}

void Parser::checkNextToken(const CString&token)
{
	CString found=getNextToken();
	if(!token==found)
		close();	
}

CString Parser::getNextCodeBlock()
{	
	CString code;
	CString codeStr="<code>";
	checkNextToken(codeStr);
	while(TRUE) 
	{
		CString line;
		try 
		{
			m_file.ReadString(line);			
		} 
		catch(...)
		{			
			return line;
		}		
		if(line.Trim()==codeStr)
			return code;
		code+=line;
		code+="\n";
	}
}

BOOL Parser::getNextBoolean()
{
	CString tok=getNextToken();

	return (BOOL)atoi(tok);
}

int Parser::getNextInt()
{
	CString tok=getNextToken();

	return atoi(tok);
}

float Parser::getNextFloat()
{
	CString tok=getNextToken();

	return (float)atof(tok);
}
