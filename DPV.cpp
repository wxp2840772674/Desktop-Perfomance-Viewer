﻿// DPV.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "DPV.h"


#define MAX_LOADSTRING 100
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HFONT hFont;                                    // 全局字体
HWND OSStatic,CPUStatic, MemoryStatic;          // 全局控件(OS版本、CPU信息、内存信息)
HWND DiskStatic, VideoDriverStatic, ConnectedNetworkStatic,performanceStatic;
                                                // 全局控件（硬盘、显卡、已连接的网卡信息）
HWND hWndMain;                                  //主窗口
HANDLE refrushThread,perfThread;                // 更新线程
DWORD ThreadStatus,perfStatus;
HANDLE globalHeap;                              //堆
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DPV, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DPV));

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
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DPV));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, NULL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION);
   SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW |WS_EX_NOACTIVATE);
   SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 0, 1);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   UpdateWindow(hWnd);
   return TRUE;
}






void updateInfo(int param)
{
    HWND hWnd = hWndMain;
    RECT hWndSize;
    
    GetWindowRect(hWnd, &hWndSize);
    //UPDATEINFOPARA a = (UPDATEINFOPARA)param;
    INT16 findDevice = param;
    if (findDevice == -1) {
        wchar_t* deleteStr;
        std::string osVersion;
        osVersion = "操作系统版本：" + getOSFullName();
        SetWindowText(OSStatic, deleteStr = multi_Byte_To_Wide_Char(osVersion));
        //MoveWindow(OSStatic, 0, 0, hWndSize.right - hWndSize.left, 22, TRUE);
        delete[]deleteStr;
        std::string CPUInfo;
        CPUInfo = "CPU：" + getCPUInformation();
        SetWindowText(CPUStatic, deleteStr = multi_Byte_To_Wide_Char(CPUInfo));
        //MoveWindow(CPUStatic, 0, 22, hWndSize.right - hWndSize.left, 44 * getCPUCount(), TRUE);
        delete[] deleteStr;
        std::string MemoryInfo;
        MemoryInfo = "内存：" + getMemoryInfo();
        SetWindowText(MemoryStatic, deleteStr = multi_Byte_To_Wide_Char(MemoryInfo));
        //MoveWindow(MemoryStatic, 0, 22 + 44 * getCPUCount(), hWndSize.right - hWndSize.left, 22 * getMemoryCount(), TRUE);
        delete[] deleteStr;
        std::string VideoDriverInfo;
        VideoDriverInfo = "显卡信息：" + getVideoDriverInfo();
        SetWindowText(VideoDriverStatic, deleteStr = multi_Byte_To_Wide_Char(VideoDriverInfo));
        delete[] deleteStr;
        //MoveWindow(MemoryStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount(), hWndSize.right - hWndSize.left, 22 * getVideoDriverCount(), TRUE);
        std::string DiskInfo;
        DiskInfo = "硬盘：" + getDiskInfo();
        SetWindowText(DiskStatic, deleteStr = multi_Byte_To_Wide_Char(DiskInfo));
        delete[] deleteStr;
        // MoveWindow(MemoryStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount(), hWndSize.right - hWndSize.left, 22 * getDiskCount(), TRUE);
        std::string ConnectedNetworkInfo;
        ConnectedNetworkInfo = "已连接的网卡信息：\n" + getConnectedNetworkDriverInfo();
        SetWindowText(ConnectedNetworkStatic, deleteStr = multi_Byte_To_Wide_Char(ConnectedNetworkInfo));
        delete[] deleteStr;

        //MoveWindow(ConnectedNetworkStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount(), hWndSize.right - hWndSize.left, 22 + 88 * getConnectedNetworkDriverCount(), TRUE);
        MoveWindow(OSStatic, 0, 0, hWndSize.right - hWndSize.left, 22, TRUE);
        MoveWindow(CPUStatic, 0, 22, hWndSize.right - hWndSize.left, 44 * getCPUCount(), TRUE);
        MoveWindow(MemoryStatic, 0, 22 + 44 * getCPUCount(), hWndSize.right - hWndSize.left, 22 * getMemoryCount(), TRUE);
        MoveWindow(DiskStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount(), hWndSize.right - hWndSize.left, 22 * getDiskCount(), TRUE);
        MoveWindow(VideoDriverStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount(), hWndSize.right - hWndSize.left, 22 * getVideoDriverCount(), TRUE);
        MoveWindow(ConnectedNetworkStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount(), hWndSize.right - hWndSize.left, 22 + 88 * getConnectedNetworkDriverCount(), TRUE);
        MoveWindow(performanceStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount() + 22 + 88 * getConnectedNetworkDriverCount(), hWndSize.right - hWndSize.left, 66, TRUE);
        //UpdateWindow(hWnd);
        //UpdateWindow(hWnd);
    }
    else if (findDevice == 1)
    {
        std::string ConnectedNetworkInfo;
        std::string original;
        wchar_t* deleteStr;
        ConnectedNetworkInfo = "已连接的网卡信息：\n" + getConnectedNetworkDriverInfo();
        int length = GetWindowTextLength(ConnectedNetworkStatic);
        wchar_t *wcctxt = new wchar_t[length+1];
        GetWindowText(ConnectedNetworkStatic, wcctxt, length+1);
        original = wide_Char_To_Multi_Byte(wcctxt);
        if (original.compare(ConnectedNetworkInfo))
        {
            MoveWindow(ConnectedNetworkStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount(), hWndSize.right - hWndSize.left, 22 + 88 * getConnectedNetworkDriverCount(), TRUE);
            SetWindowText(ConnectedNetworkStatic, deleteStr = multi_Byte_To_Wide_Char(ConnectedNetworkInfo));
            delete[] deleteStr;
        }
        delete[] wcctxt;
        MoveWindow(ConnectedNetworkStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount(), hWndSize.right - hWndSize.left, 22 + 88 * getConnectedNetworkDriverCount(), TRUE);
        MoveWindow(performanceStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount()+ 22 + 88 * getConnectedNetworkDriverCount(), hWndSize.right - hWndSize.left, 66, TRUE);
    }
    else if (findDevice == 2)
    {
        std::string VideoDriverInfo;
        VideoDriverInfo = "显卡信息：" + getVideoDriverInfo();
        
        std::string DiskInfo;
        DiskInfo = "硬盘：" + getDiskInfo();
        
        // MoveWindow(MemoryStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount(), hWndSize.right - hWndSize.left, 22 * getDiskCount(), TRUE);
        std::string ConnectedNetworkInfo;
        ConnectedNetworkInfo = "已连接的网卡信息：\n" + getConnectedNetworkDriverInfo();
        wchar_t* deleteStr;
        MoveWindow(ConnectedNetworkStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount(), hWndSize.right - hWndSize.left, 22 + 88 * getConnectedNetworkDriverCount(), TRUE);
        MoveWindow(VideoDriverStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount(), hWndSize.right - hWndSize.left, 22 * getVideoDriverCount(), TRUE);
        MoveWindow(DiskStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount(), hWndSize.right - hWndSize.left, 22 * getDiskCount(), TRUE);
        MoveWindow(performanceStatic, 0, 22 + 44 * getCPUCount() + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount() + 22 + 88 * getConnectedNetworkDriverCount(), hWndSize.right - hWndSize.left, 88, TRUE);
        SetWindowText(VideoDriverStatic, deleteStr=multi_Byte_To_Wide_Char(VideoDriverInfo));
        delete[] deleteStr;
        SetWindowText(DiskStatic, deleteStr = multi_Byte_To_Wide_Char(DiskInfo));
        delete[] deleteStr;
        SetWindowText(ConnectedNetworkStatic, deleteStr = multi_Byte_To_Wide_Char(ConnectedNetworkInfo));
        delete[] deleteStr;
        
    }
    UpdateWindow(hWnd);
    
}

int needUpdate = 1;




DWORD WINAPI updatePerf(LPVOID param)
{
    WCHAR CPUF[] = L"\\Processor Information(_Total)\\Processor Frequency";
    DPHViewer CPUFreq(CPUF);

    WCHAR CPUITime[] = L"\\Processor Information(_Total)\\% Processor Time";
    DPHViewer CPUITimes(CPUITime);

    WCHAR MemoryAvailStr[] = L"\\Memory\\Available MBytes";
    DPHViewer MemoryAvail(MemoryAvailStr);

    WCHAR NetworkStr[] = L"\\Network Interface(*)\\Bytes Total/sec";
    DPHViewer Network(NetworkStr);
    while (1)
    {
        Sleep(1000);
        double cpuNums = CPUFreq.getData();
        double cpuITimesD = CPUITimes.getData();
        int memoryAvail = MemoryAvail.getData();
        double networkTran = Network.getData();
        WCHAR out[128];
        swprintf_s(out, L"%lf\n", networkTran);
        OutputDebugString(out);
        WCHAR str[512];
        
        swprintf_s(str, L"资源使用率：\tCPU：%.2lf%%（%.2lf GHz）\n\t\t内存：%d MB 空闲\n\t\t网络活动：%.1f %cB/s", 
            cpuITimesD, cpuNums / 1000, memoryAvail,
            int(networkTran / 1024 / 1024) > 0 ? networkTran / 1024/1024: networkTran / 1024,
            int(networkTran / 1024 / 1024) > 0?'M':'K');

        SetWindowText(performanceStatic, str);
    }

}


DWORD WINAPI checkUpdate(LPVOID param)
{
    int count = 0;
    while (1)
    {
        if (needUpdate == 2)
        {
            updateInfo(1);
            needUpdate = 0;
        }
        else if (needUpdate == 1)
        {
            updateInfo(-1);
            needUpdate = 0;
        }
        else if (needUpdate == 3)
        {
            updateInfo(2);
            needUpdate = 0;
        }
        else
            Sleep(2000);
        _CrtDumpMemoryLeaks();
        count++;
        if (count % 2 == 0)
        {
            if (needUpdate == 0)
                needUpdate = 2;

        }
        else if (count == 15)
        {
            count = 0;
        }
    }
}












//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        globalHeap = GetProcessHeap(); 
        //SetProcessWorkingSetSize()
        SetProcessDPIAware();
        RECT workSpace;
        SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&workSpace, 0);
        hFont = CreateFont(-16/*高*/, -8/*宽*/, 0, 0, 400 /*400表示正常字体*/,
            FALSE/*斜体?*/, FALSE/*下划线?*/, FALSE/*删除线?*/, DEFAULT_CHARSET,
            OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
            FF_DONTCARE, TEXT("微软雅黑")
        );
        MoveWindow(hWnd, workSpace.right / 2 - workSpace.left / 2, 50, workSpace.right / 2 - workSpace.left / 2, workSpace.bottom - workSpace.top-50,TRUE);
        OSStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"操作系统版本",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 0 /*Y坐标*/, workSpace.right/2-workSpace.left/2 /*宽度*/, 20 /*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        /*std::string osVersion;
        osVersion = "操作系统版本：" + getOSFullName();
        SetWindowText(OSStatic, multi_Byte_To_Wide_Char(osVersion));*/
        SendMessage(OSStatic, WM_SETFONT, (WPARAM)hFont, NULL);
        
       /* std::string CPUInfo;
        CPUInfo = "CPU：" + getCPUInformation();*/
        CPUStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"CPU：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20 /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/, 44*getCPUCount() /*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(CPUStatic, multi_Byte_To_Wide_Char(CPUInfo));
        SendMessage(CPUStatic, WM_SETFONT, (WPARAM)hFont, NULL);
        
        //std::string MemoryInfo;
        //MemoryInfo = "内存：" + getMemoryInfo();
        MemoryStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"内存：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20+(44*getCPUCount()) /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/,  22*getMemoryCount()/*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(MemoryStatic, multi_Byte_To_Wide_Char(MemoryInfo));
        SendMessage(MemoryStatic, WM_SETFONT, (WPARAM)hFont, NULL);


        //std::string DiskInfo;
        //DiskInfo = "硬盘：" + getDiskInfo();
        DiskStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"硬盘：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20 + (44 * getCPUCount())+ 22*getMemoryCount() /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/, 22 * getDiskCount()/*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(DiskStatic, multi_Byte_To_Wide_Char(DiskInfo));
        SendMessage(DiskStatic, WM_SETFONT, (WPARAM)hFont, NULL);


        //std::string VideoDriverInfo;
        //VideoDriverInfo = "显卡信息：" + getVideoDriverInfo();
        VideoDriverStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"显卡信息：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20 + (44 * getCPUCount()) + 22 * getMemoryCount()+22*getDiskCount() /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/, 22 * getVideoDriverCount()/*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(VideoDriverStatic, multi_Byte_To_Wide_Char(VideoDriverInfo));
        SendMessage(VideoDriverStatic, WM_SETFONT, (WPARAM)hFont, NULL);


        //std::string ConnectedNetworkInfo;
        //ConnectedNetworkInfo = "已连接的网卡信息：\n" + getConnectedNetworkDriverInfo();
        ConnectedNetworkStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"显卡信息：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20 + (44 * getCPUCount()) + 22 * getMemoryCount() + 22 * getDiskCount()+ 22 * getVideoDriverCount()
            /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/, 22 + 88 * getConnectedNetworkDriverCount()/*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(ConnectedNetworkStatic, multi_Byte_To_Wide_Char(ConnectedNetworkInfo));
        SendMessage(ConnectedNetworkStatic, WM_SETFONT, (WPARAM)hFont, NULL);


        performanceStatic = CreateWindow(
            L"static",			//静态文本框的类名
            L"性能信息：",		//控件的文本
            WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | SS_LEFT /*水平居中*/  /*垂直居中*/,
            0 /*X坐标*/, 20 + (44 * getCPUCount()) + 22 * getMemoryCount() + 22 * getDiskCount() + 22 * getVideoDriverCount() + 22 + 88 * getConnectedNetworkDriverCount()
            /*Y坐标*/, workSpace.right / 2 - workSpace.left / 2 /*宽度*/, 88/*高度*/,
            hWnd,		 //父窗口句柄
            (HMENU)1,	 //为控件指定一个唯一标识符
            hInst,		 //当前程序实例句柄
            NULL
        );
        //SetWindowText(ConnectedNetworkStatic, multi_Byte_To_Wide_Char(ConnectedNetworkInfo));
        SendMessage(performanceStatic, WM_SETFONT, (WPARAM)hFont, NULL);

        hWndMain = hWnd;
        int control = -1;
           //updateInfo(&a);
        refrushThread = CreateThread(NULL, 0, checkUpdate, NULL, 0, &ThreadStatus);
        perfThread = CreateThread(NULL, 0, updatePerf, NULL, 0, &perfStatus);
        SystemParametersInfo(SPI_SETFONTSMOOTHING,
            TRUE,
            0,
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE,
            0,
            (PVOID)FE_FONTSMOOTHINGCLEARTYPE,
            SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
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
            
            //SetLayeredWindowAttributes(hWnd, RGB(251, 255, 242), 0, LWA_COLORKEY);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
            
        }
        break;
    case WM_DEVICECHANGE:
    {
    //updateInfo(&a);
        needUpdate = 3;
        
    }   break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(255,255,254));

        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        closeWMI();
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
