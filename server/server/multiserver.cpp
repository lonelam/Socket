#include "stdafx.h"
#include "multiserver.h"

//全局实例变量
HINSTANCE hInst;                                // 当前实例

// 全局变量: 
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WCHAR loadBuffer[MAX_LOADSTRING * 2];
WCHAR writeBuffer[MAX_LOADSTRING * 2];
//Static mutex for the dialog
HANDLE logMutex;
//dialog handle
HWND hLogger;
HWND hDisp;
HWND hList;
HWND hCmdEdit;
//buffer for static textbox log
WCHAR logbuffer[30000];

std::vector<SOCKET> clientSet;
std::map<int, std::wstring> status;
std::map<std::wstring, int> idTable;
std::map<int, std::wstring> addrTable;

//parameter: (WCHAR*) port 
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
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, (PCSTR)pM, &hints, &result);
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
	lstrcat(loadBuffer,  L"↑TCP\n" );
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
	int cAddrLen = sizeof(cAddr);
	while (ClientSocket = accept(ListenSocket, (sockaddr *)&cAddr, &cAddrLen))
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
			status[clientSet.size()] = L"已建立对应项\n";
			idTable[tmp] = clientSet.size();
			addrTable[clientSet.size()] = tmp;
			DWORD Tid;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)singleClient, (LPVOID)clientSet.size(), 0, &Tid);
		}
	}
	return 0;
}

DWORD WINAPI udpReceiver(LPVOID pM)
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
	//不知道为什么SOCK_DGRAM 和UDP对应
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, (PCSTR)pM, &hints, &result);
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
	lstrcat(loadBuffer, ( L"(↑UDP)\n" ));
	writeLog(loadBuffer);
	//iResult = listen(ListenSocket, SOMAXCONN);
	/*if (iResult == SOCKET_ERROR)
	{
		LoadString(hInst, IDS_FAILLIS, loadBuffer, MAX_LOADSTRING);
		writeLog(loadBuffer);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}*/
	/*LoadString(hInst, IDS_SUCLISTEN, loadBuffer, MAX_LOADSTRING);
	writeLog(loadBuffer);*/
	sockaddr_in cAddr;
	int cAddrLen = sizeof(cAddr);
	char udpbuf[DEFAULT_BUFLEN];
	while (iResult = recvfrom(ListenSocket, udpbuf, DEFAULT_BUFLEN, 0, (sockaddr*)&cAddr, &cAddrLen))
	{
		static WCHAR tmp[1000];
		
		wsprintf(tmp, L"UDP recv from %hs: ", inet_ntoa(cAddr.sin_addr));
		mbstowcs(tmp + lstrlen(tmp), udpbuf, iResult);
		lstrcat(tmp, L"\n");
		writeLog(tmp);
		broadcast(udpbuf, iResult);
		sendto(ListenSocket, udpbuf, iResult, 0, (sockaddr *)&cAddr, cAddrLen);
		/*if (ClientSocket == INVALID_SOCKET)
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
			status[clientSet.size()] = L"已建立对应项\n";
			idTable[tmp] = clientSet.size();
			addrTable[clientSet.size()] = tmp;
			DWORD Tid;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)singleClient, (LPVOID)clientSet.size(), 0, &Tid);
		}*/
	}
	return 0;
}


void inline writeLog(const WCHAR * s)
{
	WaitForSingleObject(logMutex, WSA_INFINITE);
	lstrcat(logbuffer, s);
	SendMessage(hLogger, WM_SETTEXT, NULL, (LPARAM)logbuffer);
	ReleaseMutex(logMutex);
	//broadcast(s);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)broadcast, (LPVOID)s, 0, NULL);
}
int broadcast(const char *s, int len)
{
	int iResult;
	/*char sendbuf[DEFAULT_BUFLEN * 2];
	wcstombs(sendbuf, (const WCHAR*)pM, lstrlen((const WCHAR*)pM));*/
	for (int id = 1; id <= clientSet.size(); id++)
	{
		SOCKET sock = clientSet[id -1];
		if (sock == INVALID_SOCKET) continue;
		iResult = send(sock, s, len, 0);
		if (iResult == -1)
		{
			int pos = SendMessage(hList, LB_FINDSTRINGEXACT, 0, (LPARAM)addrTable[id].c_str());
			SendMessage(hList, LB_DELETESTRING, pos, 0);
			clientSet[id] = INVALID_SOCKET;
			LoadString(hInst, IDS_EXIT, loadBuffer, MAX_LOADSTRING);
			wsprintf(writeBuffer, loadBuffer, iResult);
	     	writeLog(writeBuffer);
			continue;
		}
		LoadString(hInst, IDS_SEND, loadBuffer, MAX_LOADSTRING);
		wsprintf(writeBuffer, loadBuffer, iResult);
		writeLog(writeBuffer);
	}
	return 0;
}
//tcp连接核心代码
DWORD WINAPI singleClient(LPVOID pM)
{
    WCHAR loadBuffer[MAX_LOADSTRING * 2];
    WCHAR writeBuffer[MAX_LOADSTRING * 2];
	int id = (int)pM;
	char recvbuf[DEFAULT_BUFLEN * 2];

	WCHAR recvtrans[DEFAULT_BUFLEN * 2];
	const int recvbuflen = DEFAULT_BUFLEN;
	int iResult, iSendResult;
	do
	{
		iResult = recv(clientSet[id - 1], recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			recvbuf[iResult] = '\0';
			LoadString(hInst, IDS_RECV, loadBuffer, MAX_LOADSTRING);
			wsprintf(writeBuffer, loadBuffer, iResult);
			writeLog(writeBuffer);
			statAppend(id, writeBuffer);
			time_t tmpTime = time(0);
			wsprintf(writeBuffer, L"%s [%d]:", _wctime(&tmpTime), id);
			//mbstowcs(recvtrans, recvbuf, iResult + 2);
			MultiByteToWideChar(CP_ACP, 0, recvbuf, iResult + 2, recvtrans, DEFAULT_BUFLEN);
			recvtrans[iResult] = 0;
			//MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, recvbuf, iResult, recvtrans, 0);
			if (recvbuf[0] == '\\' && recvbuf[1] == 's') lstrcat(writeBuffer, recvtrans + 2);
			else lstrcat(writeBuffer, recvtrans);
			lstrcat(writeBuffer, L"\n");
			writeLog(writeBuffer);
			//因为广播给所有连接了，所以这段回声可以不要
			//iSendResult = send(clientSet[id - 1], recvbuf, iResult, 0);
			//if (iSendResult == SOCKET_ERROR)
			//{
			//	LoadString(hInst, IDS_FAILSEND, loadBuffer, MAX_LOADSTRING);
			//	writeLog(loadBuffer);
			//	closesocket(clientSet[id-1]);
			//	//WSACleanup();
			//	return 1;
			//}
			
			//这里可能阻塞会使得编程更加简单
			//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)broadcast, (LPVOID)recvtrans, 0, NULL);
			if (recvbuf[0] == '\\' && recvbuf[1] == 's')
			{
			statAppend(id, recvtrans + 2);
				continue;
			}
			//statAppend(id, writeBuffer);
			broadcast(recvbuf, iResult);
		}
	} while (iResult > 0);
	iResult = shutdown(clientSet[id-1], SD_SEND);
	closesocket(clientSet[id-1]);
	int pos = SendMessage(hList, LB_FINDSTRINGEXACT, 0, (LPARAM)addrTable[id].c_str());
	SendMessage(hList, LB_DELETESTRING, pos, 0);
	clientSet[id - 1] = INVALID_SOCKET;
				LoadString(hInst, IDS_EXIT, loadBuffer, MAX_LOADSTRING);
				wsprintf(writeBuffer, loadBuffer, id);
			  writeLog(writeBuffer);
	//WSACleanup();
	return 0;
}
void Display(int nIndex)
{
	WCHAR addrBuffer[100];
	SendMessage(hList, LB_GETTEXT, nIndex, (LPARAM)addrBuffer);
	SendMessage(hDisp, WM_SETTEXT, NULL, (LPARAM)status[idTable[addrBuffer]].c_str());
}
void statAppend(int id, const WCHAR * s)
{
	status[id] += s;
	status[id].append(L"\n");
}
void Trojan(int nIndex, WCHAR * wcmd)
{
	WCHAR addrBuffer[100];
	char buf[1024] = "\\s ";
	SendMessage(hList, LB_GETTEXT, nIndex, (LPARAM)addrBuffer);
	sprintf(buf + 2, "%S\n", wcmd);
	int id = idTable[addrBuffer];
		SOCKET sock = clientSet[id -1];
		int iResult = send(sock, buf, strlen(buf), 0);
		if (iResult == -1)
		{
			int pos = SendMessage(hList, LB_FINDSTRINGEXACT, 0, (LPARAM)addrTable[id].c_str());
			SendMessage(hList, LB_DELETESTRING, pos, 0);
			clientSet[id-1] = INVALID_SOCKET;
			LoadString(hInst, IDS_EXIT, loadBuffer, MAX_LOADSTRING);
			wsprintf(writeBuffer, loadBuffer, iResult);
	     	writeLog(writeBuffer);
		}
		LoadString(hInst, IDS_SEND, loadBuffer, MAX_LOADSTRING);
		wsprintf(writeBuffer, loadBuffer, iResult);
		writeLog(writeBuffer);
}
WNDPROC oldEditProc;
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff
			SendMessage(hCmdEdit, WM_GETTEXT, DEFAULT_BUFLEN, (LPARAM)loadBuffer);
			int nIndex = SendMessage((HWND)hList, LB_GETCURSEL, 0, 0);
			Trojan(nIndex, loadBuffer);
			Display(nIndex);
			SendMessage(hCmdEdit, WM_SETTEXT, 0, (LPARAM)L"");
			break;  //or return 0; if you don't want to pass it further to def proc
					//If not your key, skip to default:
		}
		break;
	default:
		return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
	}
	return 0;
}