#pragma once

#include "resource.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mutex>
#pragma comment (lib, "Ws2_32.lib")
//Static mutex for the dialog
HANDLE DiaMutex;
//dialog handle

//Single Client Process Thread
DWORD WINAPI singleClient(LPVOID pM);


