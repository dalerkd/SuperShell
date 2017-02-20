#pragma once

#include <windows.h>
#include <stdio.h>
#include <map>
#include <string>
using std::map;
using std::string;

class SS
{
public:
	SS(void);
	~SS(void);

public:
	void call();//执行调用
	void LoadLib(string libName);//加载库
	void ayaInput();//词法分析，语法分析
	

private:
	map<string,HMODULE> m_str_hm;

};

