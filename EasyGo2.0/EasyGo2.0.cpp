// EasyGo2.0.cpp : 定义应用程序的入口点。
#define _CRT_SECURE_NO_WARNINGS 1

#include "framework.h"
#include "EasyGo2.0.h"
#include "stdio.h"
#include <windows.h>
#include <mmsystem.h>
#include "resource.h"
#include "time.h"
#include "stdlib.h"
#include "resource.h"
#include<string.h>

#pragma comment(lib,"winmm.lib")

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
    int Iscoring;
    float Istime;
};

struct Rank
{
    struct infor* data;//保存成绩信息
    int num;
    int capacity;
}Data;


int left = 100, top = 20, right = left + 250, bottom = top + 400;//定义变量以及字母的下落区
char c1, c2;//定义变量c1保存当前下落字母，c2保存用户键入的字母
int x = -1, y = -1; //定义变量保存下落字母当前下落位置坐标，-1表示还没下落
int iScoring = 0, iFail = 0;//iScoring记录得分，iFail记录射击失败次数
int gameover = 0; //标识当前游戏是否已经失败
float stime = 0;//记录存活时间
float time_tmp=0;
int speed=1;//根据不同难度设置不同的起始速度，默认为1
int multiple=1;//得分倍数默认为1(即默认为简单难度)

void DrawBk(HDC hdc, int x1, int y1, int x2, int y2)
{
    Rectangle(hdc, x1, y1, x2, y2);//绘制一个矩形标识字母下落范围
    char s[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    TextOutA(hdc, x1 + 5, y2 - 25, s, strlen(s));                 
}

//初始化结构体变量Data以及开辟动态内存空间

void InitData(HDC hdc,struct Rank* pa)
{
    pa->data = (struct infor*)malloc(Default_amount * sizeof(struct infor));
    if (pa->data == NULL)
    {
        return;
    }
    pa->num = 0;
    pa->capacity = Default_amount;
}

void Checkcap(struct Rank* pa);

//从文件中上传成绩信息到data
void Load_infor(struct Rank* pa)
{
    struct infor tmp={0};
    FILE* pfRead = fopen("成绩信息.data", "rb");
    if (pfRead == NULL)
    {
        return;
    }
    while (fread(&tmp, sizeof(struct infor), 1, pfRead))
    {
        Checkcap(pa);
        pa->data[pa->num]=tmp;
        pa->num++;
    }
    fclose(pfRead);
    pfRead = NULL;
}


//检查当前pa指向的内存空间是否足够存储成绩
void Checkcap(struct Rank* pa)
{
    if (pa->num == pa->capacity)
    {
        struct infor* tmp = pa->data;
         pa->data = (struct infor*)realloc(tmp, (pa->capacity + 5) * sizeof(struct infor));//开辟一块动态内存空间来存放成绩信息
        if (pa->data != NULL)
        {
            pa->capacity += 5;
        }
        else
            return;
    }
}


//保存成绩信息
void Preserve_infor(HWND hWnd, struct Rank* pa)
{
    FILE* pfWrite = fopen("成绩信息.data", "ab");
    if (pfWrite == NULL)
    {
        return;
    }
    else
    {
        pa->data[pa->num].Iscoring = iScoring;
        pa->data[pa->num].Istime = stime;
        fwrite(&(pa->data[pa->num]), sizeof(struct infor), 1, pfWrite);
        MessageBoxA(hWnd, "保存成功", "成绩保存", 0);
        fclose(pfWrite);
    }
    pfWrite = NULL;
}

//销毁成绩信息(释放动态开辟内存空间)
void Destroy_infor(struct Rank* pa)
{
    free(pa->data);
    pa->data = NULL;
}

void ShowScoring(HDC hdc, int x, int y, int iScoring, int iFail)
{
    char szTmp[20];
    TextOutA(hdc, x, y, "当前得分：", strlen("当前得分："));
    y = y + 20;
    sprintf(szTmp, "%d", iScoring);
    TextOutA(hdc, x, y, szTmp, strlen(szTmp));
    y = y + 20;
    TextOutA(hdc, x, y, "当前失误：", strlen("当前失误："));
    y = y + 20;
    sprintf(szTmp, "%d", iFail);
    TextOutA(hdc, x, y, szTmp, strlen(szTmp));
    y = y + 20;
    TextOutA(hdc, x, y, "存活时间(s):", strlen("存活时间(s):"));
    y = y + 20;
    sprintf(szTmp, "%.2f", stime);
    TextOutA(hdc, x, y, szTmp, strlen(szTmp));
}

//游戏结束显示整个成绩
void ShowFinaGrade(HDC hdc, int x, int y, int iScoring, int iFall, struct Rank* pa)
{
    char tmp[20];
    COLORREF OldColor, NewColor = RGB(0, 0, 0);
    OldColor = SetTextColor(hdc, NewColor);
    TextOutA(hdc, x + 35, y, "当前得分:", strlen("当前得分:"));
    TextOutA(hdc, x + 35, y + 40, "当前失误:", strlen("当前失误:"));
    TextOutA(hdc, x + 35, y + 80, "存活时间(s):", strlen("存活时间(s):"));
    TextOutA(hdc, x + 35, y + 120, "成绩评级:", strlen("成绩评级:"));
    if (iScoring >= 80 || (iScoring >= 50 && iFall <= 1))//根据成绩进行成绩评级判定并设置不同评级的颜色
    {
        NewColor = RGB(185, 10, 0);//评级SSS为黑红色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "SSS");
    }
    if (iScoring >= 50 && iScoring < 80 && iFall>1 && iFall <= 5)
    {
        NewColor = RGB(245, 245, 1);//评级SS为金黄色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "SS");
    }
    if (iScoring >= 50 && iScoring < 80)
    {
        NewColor = RGB(255, 255, 110);//评级S为黄色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "S");
    }
    if (iScoring >= 30 && iScoring < 50)
    {
        NewColor = RGB(255, 2, 255);//评级A为紫色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "A");
    }
    if (iScoring >= 20 && iScoring < 30)
    {
        NewColor = RGB(4, 130, 255);//评级B为蓝色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "B");
    }
    if (iScoring >= 10 && iScoring < 20)
    {
        NewColor = RGB(40, 255, 0);//评级C为绿色
        SetTextColor(hdc, NewColor);
        strcpy(tmp, "C");
    }
    if(iScoring < 10)
    {
        strcpy(tmp, "D");
    }
    TextOutA(hdc, x + 60, y + 140, tmp, strlen(tmp));
    sprintf_s(tmp, "%d", iScoring);
    TextOutA(hdc, x + 60, y + 20, tmp, strlen(tmp));
    sprintf_s(tmp, "%d", iFall);
    TextOutA(hdc, x + 60, y + 60, tmp, strlen(tmp));
    sprintf_s(tmp, "%.2f", stime);
    TextOutA(hdc, x + 50, y + 100, tmp, strlen(tmp));
}

void GameOver(HDC hdc, int x, int y,HWND hWnd)
{
    COLORREF OldColor, NewColer = RGB(rand()%255, rand() % 255, rand() % 255);
    OldColor = SetTextColor(hdc, NewColer);//设置打印Game Over!时字体颜色为彩色
    RoundRect(hdc, x - 25, y - 60, x + 120, y, 25, 25);
    TextOutA(hdc, x, y-40, "GAME OVER!", strlen("GAME OVER!"));
    Rectangle(hdc, x - 35, y + 20, x + 130, y + 220);
    ShowFinaGrade(hdc, x - 15, y + 30, iScoring, iFail, &Data);
}

void Fire(HDC hdc, int x, int y1, int y2)
{
    HPEN hOldPen, hNewPen = CreatePen(PS_DASHDOTDOT, 1, RGB(rand()%255, rand()%255, rand()%255));
    hOldPen = (HPEN)SelectObject(hdc, hNewPen);
    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, y2);
    if (c1 == c2)
    {
        if (y <= 60)//当在字母下落到纵坐标为60之前消除字母播放amazing音效
        {
                mciSendString(L"open amazing.wav alias WAVE1", NULL, 0, NULL);
                mciSendString(L"play WAVE1 wait", NULL, 0, NULL);
                mciSendString(L"close WAVE1", NULL, 0, NULL);
        }
        else if (y > 60 && y <= 150 && iScoring >= 10)//当在字母下落到纵坐标为150之前且得分大于10消除字母播放great音效
        { 
                mciSendString(L"open great.wav alias WAVE2", NULL, 0, NULL);
                mciSendString(L"play WAVE2 wait", NULL, 0, NULL);
                mciSendString(L"close WAVE2", NULL, 0, NULL);
        }
        else if (y >= bottom - 80 && y < bottom - 30)//当在字母即将下落到底部时消除字母播放unbelievable音效
        {
                mciSendString(L"open unbelievable.wav alias WAVE3", NULL, 0, NULL);
                mciSendString(L"play WAVE3 wait", NULL, 0, NULL);
                mciSendString(L"close WAVE3", NULL, 0, NULL);
        }
    }
    Sleep(50);
    HPEN hNewPen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hNewPen2);
    MoveToEx(hdc, x, y1, NULL);
    LineTo(hdc, x, y2);
    SelectObject(hdc, hOldPen);
    DeleteObject(hNewPen);
    DeleteObject(hNewPen2);
}

//对成绩进行排序形成对应的排行榜
void SortRanking(struct Rank* pa, char ch)
{
    int flag = 0;
    struct infor tmp={0};
    for (int i = 0; i < pa->num; i++)
    {
        int j = 0;
        for (j = 0; j < pa->num - i - 1; j++)
        {
            if (ch == 'I')
            {
                if (pa->data[j].Iscoring < pa->data[j + 1].Iscoring)
                {
                    tmp = pa->data[j];
                    pa->data[j] = pa->data[j + 1];
                    pa->data[j + 1] = tmp;
                    flag = 1;
                }
            }
            else
            {
                if (pa->data[j].Istime < pa->data[j + 1].Istime)
                {
                    tmp = pa->data[j];
                    pa->data[j] = pa->data[j + 1];
                    pa->data[j + 1] = tmp;
                    flag = 1;
                }
            }
        }
        if (flag == 0)
            break;
    }
}

//显示排行榜成绩
void ShowRanking(HWND hWnd,HDC hdc,struct Rank* pa,char ch)
{
    if (pa->num == 0)
        MessageBoxA(hWnd, "排行榜为空", "排行榜", MB_ICONEXCLAMATION);
    else
    {
        int i;
        char sztmp[20];
        COLORREF OldColor, NewColer = RGB(34, 255, 255);
        OldColor = SetTextColor(hdc, NewColer);
        Rectangle(hdc, left + 500, top - 15, left + 650, bottom);
        if (ch == 'I')
        {
            TextOutA(hdc, left + 555, top - 10, "得分榜", strlen("得分榜"));
            TextOutA(hdc, left + 530, top + 6, "排名", strlen("排名"));
            TextOutA(hdc, left + 590, top + 6, "得分", strlen("得分"));
            for (i = 0; i <= pa->num && i < 20; i++)  //由于TextOutA()函数的原因，所以写成i<=pa->num
            {
                if (i < pa->num)
                {
                    sprintf_s(sztmp, "%d", i + 1);
                    TextOutA(hdc, left + 540, top + 20 * (i + 1), sztmp, strlen(sztmp));
                }
                sprintf_s(sztmp, "%d", pa->data[i].Iscoring);
                TextOutA(hdc, left + 600, top + 20 * (i + 1), sztmp, strlen(sztmp));
            }
        }
        else
        {
            TextOutA(hdc, left + 545, top - 10, "存活榜", strlen("存活榜"));
            TextOutA(hdc, left + 520, top + 6, "排名", strlen("排名"));
            TextOutA(hdc, left + 580, top + 6, "存活时间", strlen("存活时间"));
            for (i = 0; i <= pa->num && i < 20; i++)
            {
                if (i < pa->num)
                {
                    sprintf_s(sztmp, "%d", i + 1);
                    TextOutA(hdc, left + 540, top + 20 * (i + 1), sztmp, strlen(sztmp));
                }
                sprintf_s(sztmp, "%.2f", pa->data[i].Istime);
                TextOutA(hdc, left + 590, top + 20 * (i + 1), sztmp, strlen(sztmp));
            }
        }
        SetTextColor(hdc, OldColor);
        Sleep(6000);
    }
}

//排行榜函数，实现排行榜所需的各种功能
void RankList(HWND hWnd, struct Rank* pa, char ch)
{
    HDC hdc = GetDC(hWnd);
    Load_infor(pa);
    SortRanking(pa, ch);
    ShowRanking(hWnd, hdc, pa, ch);
    ReleaseDC(hWnd, hdc);
}


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
    LoadStringW(hInstance, IDC_EASYGO20, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EASYGO20));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EASYGO20));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EASYGO20);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  srand((unsigned int)time(NULL));//利用时间戳给srand()传一个变化的参数
  HDC hdc=GetDC(hWnd);
  InitData(hdc,&Data);
  ReleaseDC(hWnd, hdc);
  int wmId;
  switch (message)
  {
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case ID_START:
            InvalidateRect(hWnd, 0, 1);                             //刷新画面
            gameover = 0;
            iScoring = 0;
            iFail = 0;
            stime = 0;//一次结束后再次开始存活时间清零
            time_tmp = float(clock());//捕捉从程序开始运行到此clock()函数被调用时所耗费的时间
            if (gameover == 1)
                KillTimer(hWnd, 2);
            else
            {
                SetTimer(hWnd, 1, 10, NULL);
                c1 = rand() % 26 + 'A';
                x = left + 5 + (c1 - 'A') * 9;
                y = top;
            }
            break;
		case ID_ISCORING://处理得分榜信息
            char ch;
            ch = 'I';
            RankList(hWnd, &Data, ch);
            InvalidateRect(hWnd, 0, 1);
			break;
		case ID_EXIST://处理存活榜信息
            ch = 'E';
            RankList(hWnd, &Data, ch);
            InvalidateRect(hWnd, 0, 1);
			break;
        case ID_EASY:
            MessageBoxA(hWnd, "选择成功", "难度", 0);
            speed = 1;
            multiple = 1;
            break;;
        case ID_MIDDLE:
            MessageBoxA(hWnd, "选择成功", "难度", 0);
            speed = 3;
            multiple = 3;
            break;
        case ID_HARD:
            MessageBoxA(hWnd, "选择成功", "难度", 0);
            speed = 5;
            multiple = 5;
            break;
        case ID_BGM1://播放选择的背景音乐
            PlaySound(LPWSTR(IDR_WAVE4), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
            break;
        case ID_BGM2:
            PlaySound(LPWSTR(IDR_WAVE5), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
            break;
        case ID_BGM3:
            PlaySound(LPWSTR(IDR_WAVE6), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
            break;
        case  ID_PAUSE:
            Sleep(4000);
            break;
		case ID_PRESERVE:
			Preserve_infor(hWnd,&Data);
			break;
		case ID_HELP:
            MessageBoxA(hWnd, "EasyGo为一款简易打字游戏\n  1.点击菜单-开始即可进入游戏\n  2.点击菜单-退出即可退出游戏\n  3.点击菜单-暂停即可暂停4s的时间\n  4.点击菜单-难度即可选择不同难度\n  5.点击菜单-音乐即可选择背景音乐\n  6.点击菜单-保存即可保存成绩\n  7.点击菜单-排行榜即可查看各类排行榜信息\n", "帮助", MB_ICONINFORMATION);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
		case IDM_EXIT:
            Destroy_infor(&Data);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	    break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		DrawBk(hdc, left, top, right, bottom); 
		ShowScoring(hdc, right + 20, top + 50, iScoring, iFail);
		if (gameover)                                           //判断游戏是否结束
			GameOver(hdc, left + 80, top + 130,hWnd);           //显示游戏结束
        else                                                    //显示游戏画面
        {
            char szTemp[8];
            sprintf_s(szTemp, "%c", c1);                        //将下落字母转为字符串
            TextOutA(hdc, x, y, szTemp, strlen(szTemp));        //显示下落字母
        }
			EndPaint(hWnd, &ps);
	    break;
    case WM_CHAR:                                             //添加键盘输入消息处理
        c2 = (wParam >= 'a' && wParam <= 'z') ? (char)wParam + 'A' - 'a' : (char)wParam;  
        hdc = GetDC(hWnd); //在WM_PAINT消息之外绘图
        Fire(hdc, left + 5 + (c2 - 'A') * 9 + 4, top, bottom);      //显示射击效果
        ReleaseDC(hWnd, hdc);
        if (c2 == c1)                                               //判断是否击中
        {
            c1 = rand() % 26 + 'A';                                 //产生新下落字母
            x = left + 5 + (c1 - 'A') * 9;                          //计算下落初始x坐标
            y = top;                                                //计算下落初始y坐标
            iScoring = iScoring + 1 * multiple;                     //不同难度每次分数加和不同
        }
        else
            iFail = iFail + 1;                                  //未击中，记录                                                      //处理键盘输入结束
        break;
	case WM_TIMER:                                                  //添加定时器消息处理                                             
    {
        switch (wParam)
        {
        case 1:                                                     //接到定时器1消息，字母下落
            stime = (clock() - time_tmp) / CLK_TCK * multiple;      //CLK_TCK为机器时钟每秒所走的时钟打点数，拿clock的返回值除以它即可得到时间
            y = y + iScoring / 10 + speed;                          //根据当前分数计算下落速度
            if (y > bottom - 30)                                    //判断是否落地
            {
                gameover = 1;                                       //游戏结束
                KillTimer(hWnd, 1);                                 //关掉字母下落计时器1
                SetTimer(hWnd, 2, 300, NULL);                       //建立显示GameOver效果的计时器2
            }
            InvalidateRect(hWnd, 0, 0);                             //刷新画面
            break;
        case 2:                                                     //接到定时器2消息，显示GameOver动画效果
            InvalidateRect(hWnd, 0, 0);                             //刷新画面
            break;
        }
        break;
    }
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
