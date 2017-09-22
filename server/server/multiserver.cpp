#include "stdafx.h"
#include "multiserver.h"

//全局实例变量
HINSTANCE hInst;                                // 当前实例

// 全局变量: 
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WCHAR loadBuffer[MAX_LOADSTRING];
WCHAR writeBuffer[MAX_LOADSTRING];
//Static mutex for the dialog
HANDLE logMutex;
//dialog handle
HWND hLogger;
HWND hList;
//buffer for static textbox log
WCHAR logbuffer[30000];

std::vector<SOCKET> clientSet;
//parameter: (int) IPPROTO_UDP / IPPROTO_TCP
DWORD WINAPI Receiver(LPVOID pM)
{
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		LoadString(hInst, IDS_FAILSTARTUP, loadBuffer, MAX_LOADSTRING);
		writeLog(loadBuffer);
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	//初始化用于接收socket的地址信息
	//其中ai_protocal参数用于定义TCP/UDP的协议，为了演示的时候让老师看到,写日志的时候写出来
	hints.ai_family = AF_INET;
	//不知道为什么SOCK_STREAM只支持TCP所以还是SOCK_RAW吧
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = (int)pM;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		LoadString(hInst, IDS_FAILGETADDR, loadBuffer, MAX_LOADSTRING);
		writeLog(loadBuffer);
		WSACleanup();
		return 1;
	}
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		LoadString(hInst, IDS_FAILGETADDR, loadBuffer, MAX_LOADSTRING);
		writeLog(loadBuffer);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		LoadString(hInst, IDS_FAILBIND, loadBuffer, MAX_LOADSTRING);
		freeaddrinfo(result);
		writeLog(loadBuffer);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	LoadString(hInst, IDS_SUCSOCKET, loadBuffer, MAX_LOADSTRING);
	lstrcat(loadBuffer, ((int)pM == IPPROTO_UDP) ? L"UDP" : ((int)pM == IPPROTO_TCP ? L"TCP" : L"Unknown"));
	writeLog(loadBuffer);
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		LoadString(hInst, IDS_FAILLIS, loadBuffer, MAX_LOADSTRING);
		writeLog(loadBuffer);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	LoadString(hInst, IDS_SUCLISTEN, loadBuffer, MAX_LOADSTRING);
	writeLog(loadBuffer);
	sockaddr_in cAddr;
	while (ClientSocket = accept(ListenSocket, (sockaddr *)&cAddr, NULL))
	{
		
		if (ClientSocket == INVALID_SOCKET)
		{
		    LoadString(hInst, IDS_FAILAC, loadBuffer, MAX_LOADSTRING);
			writeLog(loadBuffer);
		}
		else
		{
			clientSet.push_back(ClientSocket);
			static WCHAR tmp[100];
 			wsprintf(tmp, L"[%d]%hs:%d", clientSet.size(), inet_ntoa(cAddr.sin_addr), cAddr.sin_port);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)(tmp));
			singleClient((LPVOID) clientSet.size());
		}
	}
	return 0;
}

void inline writeLog(const WCHAR * s)
{
	WaitForSingleObject(logMutex, WSA_INFINITE);
	lstrcat(logbuffer, s);
	SendMessage(hLogger, WM_SETTEXT, NULL, (LPARAM)logbuffer);
	ReleaseMutex(logMutex);
}
DWORD WINAPI singleClient(LPVOID pM)
{
	int id = (int)pM;
	
	char recvbuf[DEFAULT_BUFLEN];
	WCHAR recvtrans[DEFAULT_BUFLEN * 2];
	const int recvbuflen = DEFAULT_BUFLEN;
	int iResult, iSendResult;
	do
	{
		iResult = recv(clientSet[id - 1], recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			LoadString(hInst, IDS_RECV, loadBuffer, MAX_LOADSTRING);
			wsprintf(writeBuffer, loadBuffer, iResult);
			writeLog(writeBuffer);
			time_t tmpTime = time(0);
			wsprintf(writeBuffer, L"%s [%d]:", _wctime(&tmpTime), id);
			mbstowcs(recvtrans, recvbuf, iResult);
			recvtrans[iResult] = 0;
			//MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, recvbuf, iResult, recvtrans, 0);
			lstrcat(writeBuffer, recvtrans);
			lstrcat(writeBuffer, L"\n");
			writeLog(writeBuffer);
			iSendResult = send(clientSet[id - 1], recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				LoadString(hInst, IDS_FAILSEND, loadBuffer, MAX_LOADSTRING);
				writeLog(loadBuffer);
				closesocket(clientSet[id-1]);
				//WSACleanup();
				return 1;
			}
			LoadString(hInst, IDS_SEND, loadBuffer, MAX_LOADSTRING);
			wsprintf(writeBuffer, loadBuffer, iSendResult);
			writeLog(writeBuffer);
		}
	} while (iResult > 0);
	iResult = shutdown(clientSet[id - 1], SD_SEND);
	closesocket(clientSet[id-1]);
	//WSACleanup();
	return 0;
}