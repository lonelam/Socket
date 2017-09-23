
// server.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "multiserver.h"


// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ListBoxProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 850, 900, nullptr, nullptr, hInstance, nullptr);
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	hList;
	static DWORD TCPId;
	static DWORD UDPId;
    switch (message)
    {
	case WM_CREATE:
		{
		    
		    logMutex = CreateMutex(NULL, FALSE, NULL);
		    hList = CreateWindow(TEXT("ListBox"), TEXT(""),
			LBS_NOTIFY | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
			10, 10, 200, 800, hWnd, (HMENU)IDC_MYLISTBOX, hInst, nullptr);
			hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			HWND hAddButton = CreateWindow(TEXT("Button"), TEXT("添加一项"), 
				BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				220, 10, 90, 20, hWnd, (HMENU)IDM_ADDITEM, hInst, nullptr);
			HWND hDelButton = CreateWindow(TEXT("Button"), TEXT("删除选中项"),
				BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				220, 40, 90, 20, hWnd, (HMENU)IDM_DELITEM, hInst, nullptr);
			hLogger = CreateWindow(TEXT("Static"), TEXT("日志"),
				SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
				330, 10, 200, 800, hWnd, (HMENU)IDM_LOGGER, hInst, nullptr);
			hDisp = CreateWindow(TEXT("Static"), TEXT("状态显示"),
				SS_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
				560, 10, 200, 800, hWnd, (HMENU)IDM_DISP, hInst, nullptr);
			SendMessage(hLogger, WM_SETTEXT, NULL, (LPARAM)L"初始");
			SendMessage(hDelButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
			SendMessage(hDisp, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
			SendMessage(hAddButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
			SendMessage(hList, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(FALSE, 0));
			SendMessage(hLogger, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(FALSE, 0));
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Receiver, (LPVOID)"9527", NULL, &TCPId);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)udpReceiver, (LPVOID)"10086", NULL, &UDPId);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			LRESULT res;
			static WCHAR newLog[100];
            // 分析菜单选择: 
            switch (wmId)
            {
			case IDM_ADDITEM:
				{
					SendMessage(hList, LB_ADDSTRING, NULL, (LPARAM)L"expamle");
					time_t tmpTime = time(0);
					wsprintf(newLog, L"%s\n", _wctime(&tmpTime));
					writeLog(newLog);
				}
				break;
			case IDM_DELITEM:
				//关闭一个socket连接
				res = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
				SendMessage(hList, LB_DELETESTRING, res, NULL);
				SendMessage(hList, LB_SETCURSEL, res, NULL);
				break;
			case IDC_MYLISTBOX:
				if (HIWORD(wParam) == LBN_DBLCLK)
				{
					int nIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0);
					Display(nIndex);
				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
