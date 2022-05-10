// EasyGo.cpp : 定义应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS 1

#include "framework.h"
#include "EasyGo.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "resource.h"
#include "time.h"
#include "stdlib.h"

#pragma comment(lib,"winmm.lib")

//extern HHOOK hHook;//自定义按钮名称
//extern LRESULT __stdcall CBTHookProc(long ncode, WPARAM wParam, LPARAM lParam);

#define MAX_LOADSTRING 100
#define Default_amount 10

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct infor
{
    char name[10];
    char Iscoring[10];
    char Ifall[10];
    char Istime[10];
    char Igrade[10];
};

struct Rank
{
    struct infor* data;//成绩信息
    int num;//
    int capacity;
}rank;

//HHOOK   hHook;

//LRESULT   __stdcall   CBTHookProc(long   nCode, WPARAM   wParam, LPARAM   lParam)
//{
//    if (nCode == HCBT_ACTIVATE)
//    {
//        SetDlgItemText((HWND)wParam, IDYES, TEXT("得分榜(I)"));
//        SetDlgItemText((HWND)wParam, IDNO, TEXT("存活榜(E)"));
//        SetDlgItemText((HWND)wParam, IDCANCEL, TEXT("评级榜(G)"));
//        UnhookWindowsHookEx(hHook);
//    }
//    return   0;
//}

int left = 100, top = 20, right = left + 250, bottom = top + 400;
char c1, c2;
int x = -1, y = -1;
int iScoring = 0, iFall = 0;
int gameover = 0;
double stime;

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
    LoadStringW(hInstance, IDC_EASYGO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EASYGO));

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EASYGO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EASYGO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE, int)
{
    return 0;
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

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回

void DrawBk(HDC hdc, int x1, int y1, int x2, int y2)
{
    Rectangle(hdc, x1, y1, x2, y2);
    char s[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    TextOutA(hdc, x1 + 7, y2 - 24, s, strlen(s));
}

void ShowScoring(HDC hdc, int x, int y, int iScoring, int iFall)
{
    char szTemp[32];
    TextOutA(hdc, x, y, "当前得分:", strlen("当前得分:"));
    y = y + 20;
    sprintf(szTemp, "%d", iScoring);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
    y = y + 20;
    TextOutA(hdc, x, y, "当前失误:", strlen("当前失误:"));
    y = y + 20;
    sprintf(szTemp, "%d", iFall);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
    y = y + 20;
    TextOutA(hdc, x, y, "存活时间(s):", strlen("存活时间(s):"));
    y = y + 20;
    sprintf(szTemp, "%.2lf",stime);
    TextOutA(hdc, x, y, szTemp, strlen(szTemp));
}

void ShowFinaGrade(HDC hdc, int x, int y, int iScoring, int iFall,struct Rank* pa)
{
    char szTemp[10];
    COLORREF OldColor,NewColor = RGB(0, 0, 0);
    OldColor = SetTextColor(hdc, NewColor);
    TextOutA(hdc, x+35, y, "当前得分:", strlen("当前得分:"));
    TextOutA(hdc, x+35, y+40, "当前失误:", strlen("当前失误:"));
    TextOutA(hdc, x+35, y+80, "存活时间(s):", strlen("存活时间(s):"));
    TextOutA(hdc, x+35, y+120, "成绩评级:", strlen("成绩评级:"));
    if (iScoring >= 80 || (iScoring >= 50 && iFall <= 1))
    {
        NewColor = RGB(255, 255, 0);
        SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade ,"%s", "SSS");
    }
    if (iScoring >= 50 && iScoring < 80 && iFall>1 && iFall <= 5)
    {
        NewColor = RGB(255, 255, 128);
        SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade, "%s", "SS");
    }
    if (iScoring >= 50 && iScoring < 80)
    {
        NewColor = RGB(255, 255, 128);
        SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade, "%s", "S");
    }
    if (iScoring >= 30 && iScoring < 50)
    {
         NewColor = RGB(255, 2, 255);
         SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade, "%s", "A");
    }
    if (iScoring >= 20 && iScoring < 30)
    {
        NewColor = RGB(4, 130, 255);
        SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade, "%s", "B");
    }
    if (iScoring >= 10 && iScoring < 20)
    {
        NewColor = RGB(55, 255, 55);
        SetTextColor(hdc, NewColor);
        sprintf(pa->data[pa->num].Igrade, "%s", "C");
    }
    else
    {
        sprintf(pa->data[pa->num].Igrade, "%s", "D");
    }
    sprintf(pa->data[pa->num].Iscoring, "%d", iScoring);
    TextOutA(hdc, x+60, y+20, pa->data[pa->num].Iscoring, strlen(pa->data[pa->num].Iscoring));
    sprintf(pa->data[pa->num].Ifall, "%d", iFall);
    TextOutA(hdc, x+60, y+60, pa->data[pa->num].Ifall, strlen(pa->data[pa->num].Igrade));
    sprintf(pa->data[pa->num].Istime, "%.2lf", stime);
    TextOutA(hdc, x+55, y+100, pa->data[pa->num].Istime, strlen(pa->data[pa->num].Istime));
    TextOutA(hdc, x+60, y+140, pa->data[pa->num].Igrade, strlen(pa->data[pa->num].Igrade));
    SetTextColor(hdc, OldColor);
    RoundRect(hdc, x-3, y + 160, x + 57, y + 190, 20, 20);
    sprintf(szTemp, "%s", "保存[Y]");
    TextOutA(hdc, x + 2, y + 165, szTemp, strlen(szTemp));
    RoundRect(hdc, x+72, y + 160, x + 132, y + 190, 20, 20);
    sprintf(szTemp, "%s", "取消[N]");
    TextOutA(hdc, x +77, y + 165, szTemp, strlen(szTemp));
}

//void Load_infor(struct Rank* pa);
//void InitRank(struct Rank* pa)
//{
//    pa->data = (struct infor*)malloc(Default_amount * sizeof(struct infor));
//    if (pa->data == NULL)
//    {
//        return;
//    }
//    pa->num = 0;
//    pa->capacity = Default_amount;
//  // Load_infor(pa);
//}


//void Checkcap(struct Rank* pa);

//从文件中上传成绩信息到排行榜
//void Load_infor(struct Rank* pa)
//{
//    struct infor tmp = { 0 };
//    FILE* pfRead = fopen("infor.data", "rb");
//    if (pfRead == NULL)
//    {
//        printf("Load_infor::%s\n", strerror(errno));
//        return;
//    }
//    while (fread(&tmp, sizeof(struct infor), 1, pfRead))//;若在此加了分号，则while循环里不包含任何语句，只是在不断执行fread()读取，直到返回值为0(即假)
//    {                                                   //之后在执行其后的语句，则只会读取最后一个元素，因为前面的元素都一个个的被覆盖了
//        Checkcap(pa);
//        pa->data[pa->num] = tmp;
//        pa->num++;
//    }
//    fclose(pfRead);
//    pfRead = NULL;
//}

//检测排行榜容量并增容
//void Checkcap(struct Rank* pa)
//{
//    if (pa->num == pa->capacity)
//    {
//        pa->data = (struct infor*)realloc(pa->data, (pa->capacity + 2) * sizeof(struct infor));
//        if (pa->data != NULL)
//            pa->capacity += 5;
//    }
//}

////保存成绩信息
//void Preserve_infor(struct Rank* pa)
//{
//    InitRank(pa);
//    char input = getchar();
//    int i = 0;
//    if (input == 'y'|| input=='Y')
//    {
//        FILE* pfWrite = fopen("infor.data", "wb");
//        if (pfWrite == NULL)
//        {
//            printf("Preserve_infor::%s\n", strerror(errno));
//            return;
//        }
//        for (i = 0; i < pa->num; i++)
//        {
//            fwrite(&(pa->data[i]), sizeof(struct infor), 1, pfWrite);
//        }
//        fclose(pfWrite);
//        pfWrite = NULL;
//    }
//}
//
//void Destroy_infor(struct Rank* pa)
//{
//    free(pa);
//    pa = NULL;
//}
//
//void SortRanking(struct Rank* pa)
//{
//    char input=getchar();
//    struct infor tmp;
//    int i;
//    int flag = 0;
//    if (pa->num == 0)
//    {
//        ;
//    }
//    else
//    {
//		for (i = 0; i < pa->num; i++)
//		{
//			int j = 0;
//			for (j = 0; j < pa->num - i - 1; j++)
//			{
//				if (input == 'I')
//				{
//					if (pa->data[j].Iscoring > pa->data[j + 1].Iscoring)
//					{
//						tmp = pa->data[j];
//						pa->data[j] = pa->data[j + 1];
//						pa->data[j + 1] = tmp;
//						flag = 1;
//					}
//				}
//				if (input == 'E')
//				{
//					if (pa->data[j].Istime > pa->data[j + 1].Istime)
//					{
//						tmp = pa->data[j];
//						pa->data[j] = pa->data[j + 1];
//						pa->data[j + 1] = tmp;
//						flag = 1;
//					}
//				}
//				if (input == 'G')
//				{
//					if (strcmp(pa->data[j].Iscoring, "S") * strcmp(pa->data[j + 1].Iscoring, "SS") <= 0)
//					{
//						if (strcmp(pa->data[j].Iscoring, pa->data[j + 1].Iscoring) < 0)
//						{
//							tmp = pa->data[j];
//							pa->data[j] = pa->data[j + 1];
//							pa->data[j + 1] = tmp;
//							flag = 1;
//						}
//					}
//					else
//					{
//						if (strcmp(pa->data[j].Igrade, pa->data[j].Igrade) > 0)
//						{
//							tmp = pa->data[j];
//							pa->data[j] = pa->data[j + 1];
//							pa->data[j + 1] = tmp;
//							flag = 1;
//						}
//					}
//				}
//			}
//			if (flag == 0)
//				break;
//		}
//    }
//}
//
//void ShowRanking(const struct Rank* pa)
//{
//    int i = 0;
//    if (pa->num == 0)
//        printf("排行榜为空\n");
//    else
//    {      
//        printf("%-8s\t%-5s\t%-5s\t%-8s\t%-8s\n",
//            "姓名", "得分", "失误", "存活时间", "评级");
//        for (i = 0; i < pa->num; i++)
//        {
//            printf("%-8s\t%-5s\t%-5s\t%-8s\t%-8s\n",
//                pa->data[i].name,
//                pa->data[i].Iscoring,
//                pa->data[i].Ifall,
//                pa->data[i].Istime,
//                pa->data[i].Igrade);
//        }
//    }
//}

void GameOver(HDC hdc, int x, int y, HWND hWnd)
{
    COLORREF OldColor, NewColor = RGB(255, 0, 0);
    OldColor = SetTextColor(hdc, NewColor);
    RoundRect(hdc, x-20, y-45, x+105, y,25,25);
    TextOutA(hdc, x, y-30, "GAME OVER!", strlen("GAME OVER!"));
    Rectangle(hdc, x-35, y+20, x+130, y+230);
    ShowFinaGrade(hdc, x-15, y+22, iScoring, iFall,&rank);
   // Preserve_infor(&rank);
   // Destroy_infor(&rank);
}

void Fire(HDC hdc, int x, int y1, int y2,char c1,char c2)
{
    HPEN hOldPen, hNewPen = CreatePen(PS_DASHDOTDOT, 1, RGB(255, 0, 0));
    hOldPen = (HPEN)SelectObject(hdc, hNewPen);
    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, y2);
    ///*if (c1 == c2)
    //{
    //    if (y <= 20)
    //        PlaySound(LPWSTR(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
    //    if (y>20 && y <= 100 && iScoring >= 10)
    //        PlaySound(LPWSTR(IDR_WAVE2), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
    //    if (y >= bottom - 80 && y1 < bottom - 60)
    //        PlaySound(LPW*/STR(IDR_WAVE3), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
    //}
    Sleep(100);
    HPEN hNewPen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hNewPen2);
    MoveToEx(hdc, x, y1, NULL);
    LineTo(hdc, x, y2);
    SelectObject(hdc, hOldPen);
    DeleteObject(hNewPen);
    DeleteObject(hNewPen2);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    srand((unsigned int)time(NULL));
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case ID_START:
            if (gameover == 1)
                KillTimer(hWnd, 2);
            gameover = 0;
            iScoring = 0;
            iFall = 0;
            c1 = rand() % 26 + 'A';
            x = left + 5 + (c1 - 'A') * 9;
            y = top;
            SetTimer(hWnd, 1, 10, NULL);
            break;
            //case ID_RANKING:
            //    hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTHookProc, NULL, GetCurrentThreadId());
            //                                                                        //获取当前线程一个唯一的线程标识符
            //    MessageBoxA(hWnd, "选择排行榜类别", "排行榜", MB_YESNOCANCEL);
            //    SortRanking(&rank);
            //    ShowRanking(&rank);
            //    break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            DrawBk(hdc, left, top, right, bottom);
            ShowScoring(hdc, right + 20, top + 50, iScoring, iFall);
            if (gameover)
                GameOver(hdc, left + 80, top + 130,hWnd);
            else
            {
                char szTemp[8];
                sprintf(szTemp, "%c", c1);
                TextOutA(hdc, x, y, szTemp, strlen(szTemp));
            }
            EndPaint(hWnd, &ps);
        }
        break;  
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            y = y + iScoring / 10 + 1;
            stime += 0.01;
            if (y > bottom - 40)
            {
                gameover = 1;
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 2,300,NULL);
            }
            InvalidateRect(hWnd, 0, 0);
            break;
        case 2:
            InvalidateRect(hWnd, 0, 0);
            break;
        }
        break;
    case WM_CHAR:
    {
        c2 = (wParam >= 'a' && wParam <= 'z') ? (char)wParam + 'A' - 'a' : (char)wParam;
        HDC hdc = GetDC(hWnd);
        Fire(hdc, left + 5 + (c2 - 'A') * 9 + 4, top, bottom, c1, c2);
        ReleaseDC(hWnd, hdc);
        if (c2 == c1)
        {
            c1 = rand() % 26 + 'A';
            x = left + 5 + (c1 - 'A') * 9;
            y = top;
            iScoring = iScoring + 1;
        }
        else
            iFall = iFall + 1;
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
