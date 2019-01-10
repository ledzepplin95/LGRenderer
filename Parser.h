#pragma once

class Parser
{
	typedef std::vector<CString> CStrArray;

public:
	Parser(void);
	~Parser(void);
    Parser(const CString&fileName);
	void Set(const CString&fileName);

	Parser& operator=(Parser&src);
	void close();
	CString getNextToken();
	BOOL peekNextToken(const CString&tok);
	CString getNextCodeBlock();
	BOOL getNextBoolean();
	int getNextInt();
	float getNextFloat();
	void checkNextToken(const CString&token);	
	CString getFileName()const;
    
private:
	CString fetchNextToken();
	BOOL getNextLine();

private:
	int m_index;
	CStdioFile m_file;
	CStrArray m_lineTokens;
};
