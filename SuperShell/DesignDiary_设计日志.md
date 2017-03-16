>关于设计方向的讨论
>Discussion on design direction-Baidu or Google translate

##2017-03-16
1：只有\0\r\n需要特别处理,其他的需要放行.
老的代码导致无法在字符串中传递",因为它被转换了。

2. 增加一个启动界面，让人打开程序后起码知道这款软件是做什么的，怎么做。
3. 增加ico，来自http://www.easyicon.net/569839-matte_white_square_icon_alphanumeric_letter_ss_icon.html
4. 将上传一个Relase版exe。用于用户直接下载使用。

##xxxx
完成：
1. 释放变量
2. 错误信息打印

##2017-03-05
它几乎已经完成了。

我想了很多主意，试图去兼容_cdecl和_stdcl.通过保护ESP寄存器，我完成了这项工作。暂时没有发生问题，但是不一定稳定。

发现了不少BUG并修改能使用的状态。

它现在已经能用了。

下一步：
1. 释放变量们
2. 错误信息的打印
3. wchar?
4. 更好的结构
5. ctrl+c中断的处理

##2017-03-01
星期三 0:59
增加了以下功能：
- String
- char: like 'a'
- ,	:参数分隔符

由于可执行部分没有完成，以上增加均未能测试。

##2017-02-27
我们已经支持如下内容的处理：
- \0
- Space
- Number(+Neg,-小数)

需要增加以下内容的处理：
- String
- char: like 'a'
- ,	:参数分隔符


完成了:
0.分函数处理各类词法分析,
1.简易错误处理(来自SCC编译器),
2.anaInput()的拆分工作.

但是代码没有测试.

执行函数需要对参数进行支持。


##2017-02-25
现在的设计需要定一些方向：
1. 是否未来兼容Windows以外的其他操作系统？需要重构。语法分析，模块加载，函数执行。是否需要虚函数？如何设计？
2. 语法分析部分太乱，是否独立小函数？比如判断当前是否是数字，是否是字符串,如果将其混合，是否合理？我看到的几个项目都是混合的。我认为这样不合理，无助于模块化设计，因为模块化设计可以通过方法来复用。
例子是，我遭遇了‘-’负号的设计问题，当我识别出‘-’的时候，需要下一步必须是数字才可以。（因为SS不支持运算，所以不会被当做减号）这时我需要告诉数字识别程序，负号这个事情，但是用传统swtich 或者 while，你的负号只能暴露给while的所有子结构:
```
bool f = false;
while()
{
	if(-)
	{f=true}
	else if(number)
	{if(f)......}

	++p;
}
```
因为我们做的事情足够简单所以我希望它能更好一些：
```
AnalysisNumber(char* p,bool negative=false);

AnalysisNegative()
{

AnalysisNumber(p,true)
};

while()
{
	if(-)
	
	AnalysisNumber(p);

	++p
}
```
用call来代替 if？这样消耗似乎很大！

##2017-02-25
自我介绍：
你好，我的网名叫dalerkd,我来自中国，一直以来，我有一个需求，在Windows或者其他操作系统能够在Shell中愉快的使用类C的即时函数调用.是的，我是一个C语言使用者。有时你只是需要调用MessageBox()之类的简单事情，却需要庞大的编译器，我讨厌这样做，当然也许某些即时编译器支持SuperShell的这种做法，但他们似乎更乐衷于实现更多的功能。

我希望SuperShell的设计使对它的使用能够足够轻松，仅实现c语言语法的函数调用。我们的操作系统中已经提供了函数的功能，而我们要做的只是调用它而已。
但是在之前这是困难的。

我从未设计过编译器，但我认为这个过程是一个真正程序员必经之路。本项目从零开始。

如何使用SuperShell呢？我希望它未来可以用如下方式使用：
>ss MessageBoxA(0,"Hi","Title",0);
or
>ss ENTER
MessageBoxA(0,"Hi","Title",0);

看起来很酷！我将完成它一步一步:)



Introduce myself ：

Hello, my name is dalerkd, I come from China, has been, I have a demand, in Windows or other operating system can be used in the Shell happy class C instant function call

Yes, I am a C language user. Sometimes you just need to call MessageBox () simple things like, but needs a huge compiler, I hate to do this, of course, this approach may some time compiler SuperShell support, but they seem to be more willing to realize more functions.

I hope that the design of SuperShell makes it easy to use it, only to achieve the function call C language grammar. We already have the function of the operating system, and all we have to do is call it.

But it was difficult before.

I've never designed a compiler, but I think it's a real programmer. This project starts from zero.

How to use SuperShell? I hope it can be used in the following ways:

```
>ss MessageBoxA (0, "Hi", "Title", 0);

//Or

>ss ENTER

MessageBoxA (0, "Hi", "Title", 0);
```
Look cool! I will finish it step by step:)

