#pragma once
class Exception
{
public:
	Exception(void);
	~Exception(void);

public:
	void Error(int LineNumber,char* FunName,char*,...);
	void Warning(int LineNumber,char* FunName,char*,...);
	void Tips(int LineNumber,char* FunName,char*,...);
	
	

};

