

#include <windows.h>
#include <stdio.h>
#include <map>
#include <string>
using std::map;
using std::string;
/*
LoadLibrary("")
*/

map<string,HMODULE> g_str_hm;

int call()
{
	try
	{
	}
	catch (...)
	{
		
	}
}



int main()
{
	

	/*
	1. 区分 LoadLibraryA和执行函数。
	2. LLA申请，判断是否有该DLL，成功添加进map;
	3. 执行函数申请：遍历map找到函数地址了吗?Y:调用，N:返回。

	*/
}