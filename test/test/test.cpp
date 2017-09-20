#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"

//最简单的创建多线程实例  
#include <stdio.h>  
#include <windows.h>  
#include <WinSock2.h>
//子线程函数  
DWORD WINAPI SocketProcessor(LPVOID pM)
{
	SOCKET socketInfo = (SOCKET)pM;

	return 0;
}
//主函数，所谓主函数其实就是主线程执行的函数。  
int main()
{
	printf("     最简单的创建多线程实例\n");
	printf(" -- by MoreWindows( http://blog.csdn.net/MoreWindows ) --\n\n");

	HANDLE handle = CreateThread(NULL, 0, SocketProcessor,(LPVOID) 9527 , 0, NULL);
	WaitForSingleObject(handle, INFINITE);
	system("pause");
	return 0;
}