#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"

//��򵥵Ĵ������߳�ʵ��  
#include <stdio.h>  
#include <windows.h>  
#include <WinSock2.h>
//���̺߳���  
DWORD WINAPI SocketProcessor(LPVOID pM)
{
	SOCKET socketInfo = (SOCKET)pM;

	return 0;
}
//����������ν��������ʵ�������߳�ִ�еĺ�����  
int main()
{
	printf("     ��򵥵Ĵ������߳�ʵ��\n");
	printf(" -- by MoreWindows( http://blog.csdn.net/MoreWindows ) --\n\n");

	HANDLE handle = CreateThread(NULL, 0, SocketProcessor,(LPVOID) 9527 , 0, NULL);
	WaitForSingleObject(handle, INFINITE);
	system("pause");
	return 0;
}