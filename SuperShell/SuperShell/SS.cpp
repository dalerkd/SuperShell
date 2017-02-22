#include "SS.h"

#include <iostream>


SS::SS(void)
{
}


SS::~SS(void)
{
	//delete map HMOUDLE
}

void SS::call()
{
	try
	{
	}
	catch (...)
	{

	}
}

void SS::LoadLib(string libName)
{
	HMODULE hm = LoadLibraryA(libName.c_str());
	if (hm==INVALID_HANDLE_VALUE)return;

	m_str_hm[libName]=hm;//insert
}

void SS::getInput()
{
	while(true)
	{
		std::getline(std::cin,m_input);
		ayaInput();
	}	
	
}

/*Input is a need execute function or load library? */

void SS::ayaInput()
{
	string str = m_input;

	str.erase(0, str.find_first_not_of(" \t")); // Remove header spaces
	str.erase(str.find_last_not_of(" \t") + 1); // Remove trailling spaces

	if(true == str.empty())return;


	string::size_type ret = str.find("LoadLibrary");
	if (ret==0)
	{
		/*
			LoadLibrary("LibName");
			We need LibName.
		*/
		std::string::size_type index = str.find("\"");
		if (index==string::npos)return;
		str.erase(0,index+1);
		
		index = str.find("\"");
		if (index==string::npos)return;
		str.erase(index);
		
		if(str.empty())return;
		
		LoadLib(str);
	
		return;
	}
	
	string functionName=str;

	std::string::size_type index = functionName.find("(");
	if (index==string::npos)return;

	//zero argv
	functionName.erase(index);

	typedef  int (__cdecl *int_FUN_VOID)(void);

	map<string,HMODULE>::iterator it;

	it = m_str_hm.begin();

	FARPROC funT=nullptr;

	while(it != m_str_hm.end())
	{
		funT = GetProcAddress(it->second,functionName.c_str());
		if (funT!=nullptr)
		{
			break;
		}
		it++;         
	}

	int_FUN_VOID funA = (int_FUN_VOID)funT;
	if (funA==nullptr)return;
	
	try
	{
		funA();//temp to do,we need use SS::call()
	}
	catch (...)
	{
		return;
	}
	
	return;

}
