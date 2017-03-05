#include "SS.h"

#include <iostream>


SS::SS(void)
{
}


SS::~SS(void)
{
	//delete map HMOUDLE
	//词法vector中string ,每一条指令都要执行本删除？已经在clearArgAna()中处理
	
	map<string,HMODULE>::iterator iter;
	for (iter = m_str_hm.begin();iter!=m_str_hm.end();++iter)
	{
		CloseHandle(iter->second);
	}

}


bool SS::LoadLib(string libName)
{
	auto iter = m_str_hm.find(libName);
	 if(iter!=m_str_hm.end())
	 {Warning("Repetitive loading Library！");return true;}

	HMODULE hm = LoadLibraryA(libName.c_str());
	if (hm==0)return false;

	m_str_hm[libName]=hm;//insert
	return true;
}

void SS::getInput()
{
	while(true)
	{
		std::getline(std::cin,m_input);
		try
		{
			ayaInput();
		}
		catch(...)
		{
			;
		}
		
	}	

}

/*Input is a need execute function or load library? */
bool SS::tryBeLib(string str)
{
	string::size_type ret = str.find("LoadLibrary");
	if (ret==0)
	{
		/*
		LoadLibrary("LibName");
		We need LibName.
		*/
		std::string::size_type index = str.find("\"");
		if (index==string::npos)return false;
		str.erase(0,index+1);

		index = str.find("\"");
		if (index==string::npos)return false;
		str.erase(index);

		if(str.empty())return false;

		bool result =LoadLib(str);
		if (!result)
		{
			Error("Load Library Fault:%s",str.c_str());
		}
		return true;

	}
	else
	{
		return false;
	}
}

FARPROC SS::getFuncAddr(string functionName)
{
	std::string::size_type LKindex = functionName.find("(");
	if (LKindex==string::npos)return nullptr;


	functionName.erase(LKindex);

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
	return funT;
}

void SS::getArgcs(string argList)
{
	std::string::size_type LKindex = argList.find("(");
	if (LKindex==string::npos)
	{
		Error("Don't find (");
		return;
	}


	argList.erase(0,LKindex+1);// 5,'a',"b")

	char* pArg =(char*) argList.c_str();



	for(;;++pArg)
	{
		char* tmp = pArg;

		if (IsEnd(pArg))
		{
			break;
		}
		IsSpace(pArg);//Need Be First
		IsNumber(pArg);
		IsNeg(pArg);
		IsChar(pArg);
		IsString(pArg);

		if(*pArg==')')
		{
			break;
		}
		if (*pArg==',')
		{
			continue;
		}

		if (tmp==pArg)//No man decide it
		{
			Error("What's mean:0x%x\n",*pArg);
		}
	}
	return;
}



bool SS::tryBeFunc(string functionName)
{
	std::string str = functionName;
	std::string str_2 = functionName;

	FARPROC funT = getFuncAddr(str_2);
	if (funT==nullptr)
	{
		Error("Don't find function:%s\n",str_2.c_str());
	}
	string argList = str;

	getArgcs(argList);

	executeCode(funT);

	clearArgAna();

	return true;
}

void SS::executeCode(FARPROC funT)
{
	/*
	string			"abc"	Point
	char			'a'		Immediate number
	unsigned int	324		Immediate number
	int				-2222	Immediate number	hex,10, 8?
	float			
	double


	*/

	//Excute Code
	if(0==m_argVectorList.size())
	{
		//zero argv

		typedef  int (__cdecl *int_FUN_VOID)(void);

		int_FUN_VOID funA = (int_FUN_VOID)funT;
		if (funA==nullptr)return;
		
		DWORD CallResult=0;
		try
		{
			CallResult = funA();//temp to do,we need use SS::call()
		}
		catch (...)
		{
			Execute_error("Execute function Happen,Line:%d",__LINE__);
			return;
		}

		printf("result:0x%x",CallResult);		
	}
	else//Multi Argc
	{
		/*
		Construct excute shellcode
		
		push ebp	55
		mov ebp,esp	8B EC

		mov dword ptr ds:[12345678],esp		89 25 78 56 34 12
		
		//ShellCode Start
		push 0x12345678						68 78 56 34 12
		push b
		push c
		call x								E8 offset small Little
		
		mov esp,dword ptr ds:[12345678]

		can used to execute _cdecl & _stdcl

		how to do?"mov xxx,esp"
		we should protect esp
	
	核心是找到一个地方存放esp寄存器
	1. malloc() 一个堆上的地址p，直接用 'mov p,esp'，在调用结束使用'mov esp,p'。
	2. 由编译器来处理这件事，DWORD X;_asm mov X,esp;......


	The core is to find a place to store the ESP register

	1 malloc () a heap on the address P, directly with'mov P, esp', at the end of the call using'mov ESP, p'.

	2 by the compiler to deal with this matter, DWORD X; _asm mov X, ESP;

	So I chose the first one

		*/
		//start

		DWORD CallResult=0;
		DWORD _espTmp=0;
		
		_asm mov _espTmp,esp;//Protect Esp

		DWORD m_number = m_argVectorList.size();

		//Press the stack from the last argument to the left
		for (signed int i=m_number-1;i>=0;--i)
		{
			auto m_style = m_argVectorList[i].style;
			switch (m_style)
			{
			case INTEGERX:
				{
					DWORD t=m_argVectorList[i].value;
					_asm push t;
					break;
				}
			case CHARX:
				{
					DWORD t=m_argVectorList[i].value;
					_asm push t;
					break;
				}
			case STRINGX:
				{
					DWORD t=(DWORD)m_argVectorList[i].str;
					_asm push t;
					break;
				}
			default:
				Execute_error("Who know what's it");
				break;
			}


		}
		
		_asm call funT;
		_asm mov esp,_espTmp;//Repair Esp
		_asm mov CallResult,eax;
		printf("result:0x%x\n",CallResult);

		
	}

}



void SS::ayaInput()
{
	string str = m_input;
	str.erase(0, str.find_first_not_of(" \t")); // Remove header spaces
	str.erase(str.find_last_not_of(" \t") + 1); // Remove trailling spaces
	if(true == str.empty())return;

	string strTryBeLib=str;

	if (tryBeLib(strTryBeLib))
	{
		printf("LoadLibrary successful\n");
		return;
	}


	string functionName=str;

	tryBeFunc(functionName);


	return;

}

bool SS::IsEnd(char*& p)
{
	if (*p=='\0')
	{
		return true;
	}
	else
	{
		return false;
	}

}

void SS::IsSpace(char*& p)
{
	if (*p==0x20)
	{
		++p;
	}	
}

void SS::IsNumber(char*& pArg,bool neg)
{
	if (*pArg>=0x30&&*pArg<=0x39)//number			m_argVectorList
	{

		int m_number = 0;
		for (;;++pArg)	
		{
			IsSpace(pArg);

			if(*pArg>=0x30&&*pArg<=0x39)
			{
				m_number = m_number*10+*pArg-0x30;
			}
			else if (*pArg=='x'||*pArg=='X')//hex
			{
				++pArg;

				if (m_number!=0)
				{
					Error("What's mean:'x'%c left dont is 0\n",*pArg);//like    (9x9)
					return;
				}
				else//hex
				{
					for (;(*pArg<='9'&&*pArg>='0')||(*pArg<='F'&&*pArg>='A')||(*pArg<='f'&&*pArg>='a');++pArg)
					{
						m_number = m_number * 16 + (*pArg & 15) + (*pArg >= 'A' ? 9 : 0);
					}
					IsSpace(pArg);
					if (*pArg==')'||*pArg==',')
					{
						--pArg;
						break;
					}
					else
					{
						Error("What's mean:0x%x\n",*pArg);//like  0x1g
						break;
					}
				}
			}
			else if (*pArg==')'||*pArg==','||*pArg=='}')
			{
				--pArg;
				break;
			}
			else
			{
				Error("What's mean:0x%x\n",*pArg);
				return;//err
			}



		}

		if (neg)
		{
			m_number*=-1;
		}
		argFormat tmp;
		tmp.style = INTEGERX;
		tmp.value = m_number;

		m_argVectorList.push_back(tmp);


		++pArg;

	}
}

void SS::IsNeg(char*& p)
{
	if (*p=='-')
	{
		++p;
		IsNumber(p,true);
	}
}


void SS::IsString(char*& p)
{
	if (*p!='"')
	{
		return;
	}

	++p;
	/*
	if it is not "
	\" don't is "

	other:  \Number :this version don't permit.
	\r
	\n

	"" be \0
	*/
	char* strStart = p;

#define MAX_LENGTH_STRING 1024

	//char result[MAX_LENGTH_STRING]={0};
	char* result =(char*) malloc(MAX_LENGTH_STRING);
	if (nullptr==result)
	{
		Error("No enough memory in malloc,SIZE:0x%x",MAX_LENGTH_STRING);
	}
	memset(result,'\0',MAX_LENGTH_STRING);

	for (size_t i=0;;++i)
	{
		if (i>=MAX_LENGTH_STRING-10)
		{
			Error("So long string!");
		}
		char tmp;

		if (HandleBackslash(p,tmp))
		{
			result[i]=tmp;
		}
		else if (*p=='"')//string end
		{
			result[i]='\0';
			break;
		}
		else if (*p=='\0')
		{
			Error("Terminate in unexpected!");
		}
		else
		{
			result[i]=*p++;
		}

	}

	argFormat tmp;
	tmp.style = STRINGX;
	tmp.str = result;

	m_argVectorList.push_back(tmp);

	++p;
}

void SS::IsChar(char*& p)
{
	if (*p!='\'')
	{
		return;
	}
	++p;

	int value=0;

	char cTmp;
	if (HandleBackslash(p,cTmp))
	{
		value=cTmp;
	}
	else if (*p=='\'')//string end
	{
		Warning("two ' middle don't find anything.");
		value=0;
		goto CHAR_GO;
	}
	else if (*p=='\0')
	{
		Error("Terminate in unexpected!");
	}
	else
	{
		value=*p;
	}
	++p;

	if (*p!='\'')
	{
		Error("Don't find  ' be end");
	}
CHAR_GO:
	argFormat tmp;
	tmp.style = CHARX;
	tmp.value = value;

	m_argVectorList.push_back(tmp);

	++p;

}

bool SS::HandleBackslash(char*& p,char& result)
{
	if (*p=='\\')
	{
		//TODO: More
		++p;
		switch (*p)
		{
		case '\\':
			{
				result='\\';
				break;
			}
		case 'n':
			{
				result='\n';
				break;
			}
		case 'r':
			{
				result='\r';
				break;
			}
		case '"':
			{
				result='\'';
				break;
			}
		case '\0':
			{
				Error("Terminate in unexpected!");
				break;
			}
		default:
			{
				result=*p;
				break;
			}
		}
		++p;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void SS::Error(char* fmt,...)
{
	va_list ap;
	va_start(ap,fmt);

	Handle_Exception(STAGE_COMPILE,LEVEL_ERROR,fmt,ap);
	va_end(ap);

	throw("Error");
}

void SS::Warning(char* fmt,...)
{
	va_list ap;
	va_start(ap,fmt);

	Handle_Exception(STAGE_COMPILE,LEVEL_WARNING,fmt,ap);
	va_end(ap);

	throw("Error");
}

void SS::Execute_error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	Handle_Exception(STAGE_LINK,LEVEL_ERROR, fmt, ap);
	va_end(ap);
}

void SS::Handle_Exception(int stage,int level,char* fmt,va_list ap)
{
	char buf[1024];
	vsprintf_s(buf,sizeof(buf),fmt,ap);
	if (stage == STAGE_COMPILE)
	{
		if (level==LEVEL_WARNING)
		{
			printf("Compair Warning:%s\n",buf);
		}
		else
		{
			printf("Compair Error:%s\n",buf);
			throw("Error");
		}
	}
	else
	{
		printf("Execut Error:%s\n",buf);
		throw("Error");
	}
}

void SS::clearArgAna()
{
	//clear Argcs
	size_t m_NumberOfArray=m_argVectorList.size();

	for (size_t i=0;i<m_NumberOfArray;++i)
	{
		if (m_argVectorList[i].style==STRINGX)
		{
			delete(m_argVectorList[i].str);
		}

	}


	m_argVectorList.clear();
}
