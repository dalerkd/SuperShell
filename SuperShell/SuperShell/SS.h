#pragma once

#include <windows.h>
#include <stdio.h>
#include <map>
#include <string>
#include <vector>

#include <stdarg.h>

using std::map;
using std::string;
using std::vector;

enum ArgStyle
{
	INTEGERX=6,CHARX,STRINGX
};


struct argFormat
{
	ArgStyle	style;//arg Style	
	int			value;//only imm
	char*		str;  //string point
};

class SS
{
public:
	SS(void);
	~SS(void);

public:
	bool LoadLib(string libName);//加载库
	void ayaInput();//词法分析，语法分析
	void getInput();

	bool tryBeLib(string x);//尝试作为一个库去加载,成功true
	bool tryBeFunc(string x);//尝试作为一个函数去执行

	FARPROC getFuncAddr(string x);//遍历查询某函数获取其地址
	void getArgcs(string s);//处理参数们
	void executeCode(FARPROC X);//执行程序
private:
	map<string,HMODULE> m_str_hm;		//lib句柄表
	vector<argFormat>   m_argVectorList;//词法分析结果
	
	string m_input;

private:
	/*词法分析，判断+处理
	
	Execption:
	意外语法
	语句结束
	
	
	责任：如果你消耗了当前字符，请在最后处理指针并+1,Amazing和End除外;
	*/
	bool IsEnd(char*&);		//\0
	void IsSpace(char*&);	//空格

	void IsNumber(char*&,bool IsNeg=false);	//Number
	void IsNeg(char*&);		//负号

	void IsString(char*&);
	void IsChar(char*&);

	bool HandleBackslash(char*&,char& result);//处理\们反斜杠,bool,true处理成功,
	

	//////////////////////////////////////////

	void Error(char* fmt,...);
	void Warning(char* fmt,...);
	void Execute_error(char *fmt, ...);
	void Handle_Exception(int stage,int level,char* fmt,va_list ap);
	void clearArgAna();

	/* 错误级别 */
	enum e_ErrorLevel
	{
		LEVEL_WARNING,
		LEVEL_ERROR,
	};

	/* 工作阶段 */
	enum e_WorkStage
	{
		STAGE_COMPILE,
		STAGE_LINK,
	};
};

