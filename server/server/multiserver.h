#pragma once
#include "resource.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <map>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define MAX_LOADSTRING 100
#define TCP_PORT "9527"
#define UDP_PORT "10086"

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
//UDP Receiver Thread
DWORD WINAPI udpReceiver(LPVOID pM);

//Single Client Process Thread
//Parameter: Id
DWORD WINAPI singleClient(LPVOID pM);
//cast message to all exist connection
int broadcast(const char *s, int len);
//Async writing function
void inline writeLog(const WCHAR * s);
//Displayer
void Display(int nIndex);
//write status
void statAppend(int id, const WCHAR * s);
//write this to static
extern WCHAR logbuffer[30000];
//handle of listbox
extern HWND hList;
extern std::vector<SOCKET> clientSet;
extern std::map<int, std::wstring> status;
extern std::map<std::wstring, int> idTable;
extern std::map<int, std::wstring> addrTable;
extern HWND hDisp;
