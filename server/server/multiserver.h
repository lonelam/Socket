#pragma once
#include "resource.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define MAX_LOADSTRING 100
#define DEFAULT_PORT "9527"

//全局实例变量
extern HINSTANCE hInst;                                // 当前实例

// 全局变量: 
extern WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
extern WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
extern WCHAR loadBuffer[MAX_LOADSTRING];
//Static mutex for the dialog
extern HANDLE logMutex;
//dialog handle
extern HWND hLogger;
//Receiver and Dispatcher Thread
DWORD WINAPI Receiver(LPVOID pM);
//Single Client Process Thread
//Parameter: Id
DWORD WINAPI singleClient(LPVOID pM);
//Async writing function
void inline writeLog(const WCHAR * s);
//write this to static
extern WCHAR logbuffer[30000];
//handle of listbox
extern HWND hList;
extern std::vector<SOCKET> clientSet;


