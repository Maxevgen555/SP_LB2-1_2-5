#define _CRT_SECURE_NO_WARNINGS  // ��������� �������������� � ������������ CRT �������
#define WIN32_LEAN_AND_MEAN      // ��������� ����� ������������ ���������� Windows
#include <windows.h>             // �������� ������������ ���� Windows API
#include <tchar.h>               // ��������� Unicode/ANSI ��������
#include <stdio.h>               // ����������� ����-�����
#include <time.h>                // ������� ������ �� ��������
#include <stdlib.h>  // ��� ������� rand() � srand()

#pragma comment(linker, "/subsystem:windows")  // ��������� ������� ������� Windows ���������� (�� ����������)

// ���������� ����������
HINSTANCE hInst;                            // ���������� ���������� ���������� (��������������� � WinMain)
TCHAR szClassName[] = _T("ThreadAppClass"); // ��� ������ �������� ����
TCHAR szTitle[100] = _T("��������_thread_lb"); // ��������� ���� (�������� �� ���� �������)

HANDLE hAnimationThread = NULL;    // ���������� ������ ��������
DWORD dwAnimationThreadId = 0;     // ID ������ ��������
int animationState = 0;            // ��������� ������ �������� (0-�� ������, 1-�������, 2-�������������)
int animationPriority = 0;         // ��������� ������ ��������

// ��������� �������� ������
int ballX = 100;                   // ������� X ������
int ballY = 100;                   // ������� Y ������
int ballSize = 30;                 // ������ ������
int ballSpeedX = 3;                // �������� �� X
int ballSpeedY = 2;                // �������� �� Y
COLORREF ballColor = RGB(255, 0, 0); // ���� ������ (�������)

// ���������� ��� ���������� ��������
HANDLE hSecThread[3] = { NULL };      // ������ ������������ �������: [0] - ���������, [1] - ����� 1, [2] - ����� 2
DWORD dwSecThreadId[3] = { 0 };       // ������ ��������������� ������� (Thread ID)
int g_uThCount = 0;                   // ������� ��������� ��������� �������
int threadStates[3] = { 0 };          // ��������� �������: 0 - �� ������, 1 - �������, 2 - �������������
int threadPriorities[3] = { 0 };      // ���������� ������� (�������� �� THREAD_PRIORITY_*)

// ������� ��� ������� �����
int thread1Pos = 0;  // ������� ������� X ��� ������ ������ 1
int thread2Pos = 0;  // ������� ������� X ��� ������ ������ 2

// ��������� ������� (���������� ������� ����� �� ��������������)
BOOL InitApplication(HINSTANCE hInstance);  // ������������� ���������� (����������� ������ ����)
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);  // �������� �������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // ���������� ��������� �������� ����
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);  // ���������� ������� "� ���������"
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);  // ���������� ������� ���������� � ������
void UpdateThreadInfo(HWND hDlg, int threadIndex);  // ���������� ���������� � ������ � �������

DWORD WINAPI AnimationThread(LPVOID lpParam);
void CreateAnimationThread(BOOL suspended);
void SuspendAnimationThread();
void ResumeAnimationThread();
void TerminateAnimationThread();
void IncreaseAnimationPriority();
void DecreaseAnimationPriority();
// ������� ������� (����� ����� ��� ��������� �������)
DWORD WINAPI ThreadFunc1(LPVOID lpParam);  // ������� ������ 1 (������� ������)
DWORD WINAPI ThreadFunc2(LPVOID lpParam);  // ������� ������ 2 (������� ������)

// ������� ���������� ��������
void CreateThread1(BOOL suspended);  // �������� ������ 1 (suspended = TRUE ��� ������� ������)
void CreateThread2(BOOL suspended);  // �������� ������ 2
void SuspendThread1();               // ������������ ������ 1
void ResumeThread1();                // ������������� ������ 1
void TerminateThread1();             // ���������� ������ 1
void IncreasePriority1();            // ���������� ���������� ������ 1
void DecreasePriority1();            // ���������� ���������� ������ 1
void SuspendThread2();               // ������������ ������ 2
void ResumeThread2();                // ������������� ������ 2
void TerminateThread2();             // ���������� ������ 2
void IncreasePriority2();            // ���������� ���������� ������ 2
void DecreasePriority2();            // ���������� ���������� ������ 2

// ����� ����� Windows ���������� (������ main() ��� ���������� ��������)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;  // ��������� ��� �������� ���������

    // ������������� ����������: ����������� ������ ����
    if (!InitApplication(hInstance))
        return FALSE;  // ����� ��� ������ �������������

    // �������� � ����������� �������� ����
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;  // ����� ��� ������ �������� ����

    // �������� ���� ��������� ���������
    while (GetMessage(&msg, NULL, 0, 0))  // ��������� ��������� �� �������
    {
        TranslateMessage(&msg);  // �������������� ��������� ������ (��������, WM_KEYDOWN -> WM_CHAR)
        DispatchMessage(&msg);   // �������� ��������� ��������������� ������� ���������
    }

    return (int)msg.wParam;  // ������� ���� ����������
}

// ������������� ���������� - ����������� ������ ����
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;  // ��������� ��� �������� ������ ����

    wcex.cbSize = sizeof(WNDCLASSEX);           // ������ ��������� (�����������)
    wcex.style = CS_HREDRAW | CS_VREDRAW;       // �����: ����������� ��� ��������� �������
    wcex.lpfnWndProc = WndProc;                 // ��������� �� ������� ��������� ���������
    wcex.cbClsExtra = 0;                        // �������������� ������ ��� ������
    wcex.cbWndExtra = 0;                        // �������������� ������ ��� ������� ����
    wcex.hInstance = hInstance;                 // ���������� ���������� ����������
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // ����������� ������
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   // ����������� ������
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ���� ���� (����������� ���� ����)
    wcex.lpszMenuName = MAKEINTRESOURCE(101);   // ������������� ���� (�� ��������)
    wcex.lpszClassName = szClassName;           // ��� ������ ����
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // ��������� ������

    return RegisterClassEx(&wcex);  // ����������� ������ ���� � �������
}

// �������� � ����������� �������� ����
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;  // ���������� ������������ ����

    hInst = hInstance;  // ���������� ����������� ���������� � ���������� ����������

    // �������� �������� ����
    hWnd = CreateWindow(szClassName,           // ��� ������ ����
        szTitle,               // ��������� ����
        WS_OVERLAPPEDWINDOW,   // ����� ���� (����������� ��������������� ����)
        CW_USEDEFAULT,         // ������� X �� ���������
        CW_USEDEFAULT,         // ������� Y �� ���������
        800,                   // ������ ����
        600,                   // ������ ����
        NULL,                  // ������������ ���� (���)
        NULL,                  // ���� (��� ������� � ������ ����)
        hInstance,             // ���������� ����������
        NULL);                 // �������������� ��������� ��������

    if (!hWnd)  // �������� ���������� �������� ����
        return FALSE;

    // ����������� � ���������� ����
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// ������� ������ 1 - ������� ������� ������ � ������� ����
DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;  // �������� ���������� �������� ���� �� ���������
    HDC hdc;                    // �������� ���������� ��� ���������
    RECT rect;                  // ������������� ��� �������� �������� ���������� �������
    TCHAR szMsg[] = _T("��������� ����� 1 ������ ��������"); // ����� ��� �����������

    // ����������� ���� ������ (����������� �������������)
    while (TRUE)
    {
        // ��������� ��������� ������ - ������ ������ ���� ����� �������
        if (threadStates[1] == 1) // ���� ����� �������
        {
            hdc = GetDC(hWnd);  // �������� �������� ���������� ��� ���������
            if (hdc)
            {
                GetClientRect(hWnd, &rect);  // �������� ������� ���������� ������� ����

                // ������� ���������� ������� ������ (�������� ������ �����)
                SetBkColor(hdc, GetSysColor(COLOR_WINDOW));     // ���� ���� = ���� ����
                SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // ���� ������ = �����������
                TextOut(hdc, thread1Pos - 1, 50, _T(""), 0);    // �������� ������ �����

                // ����� ������ ������
                SetBkColor(hdc, RGB(255, 255, 0));  // ������ ���
                SetTextColor(hdc, RGB(0, 0, 255));  // ����� �����
                TextOut(hdc, thread1Pos, 50, szMsg, _tcslen(szMsg));  // ������ �����

                ReleaseDC(hWnd, hdc);  // ����������� �������� ����������
            }

            thread1Pos += 5;  // ���������� ������� ������
            if (thread1Pos > rect.right)  // ���� ����� ���� �� ������ �������
                thread1Pos = -300;        // ���������� � ������ (� ������� ��� ���������)
        }

        Sleep(100);  // �������� ��� �������� �������� ��������
    }

    return 0;  // ��� ���������� (������� �� ����������� ��-�� ������������ �����)
}

// ������� ������ 2 - ���������� ������ 1, �� � ������� �����������
DWORD WINAPI ThreadFunc2(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    HDC hdc;
    RECT rect;
    TCHAR szMsg[] = _T("��������� ����� 2 ������ ��������");

    while (TRUE)
    {
        if (threadStates[2] == 1) // ���� ����� �������
        {
            hdc = GetDC(hWnd);
            if (hdc)
            {
                GetClientRect(hWnd, &rect);

                // ������� ���������� �������
                SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
                SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                TextOut(hdc, thread2Pos - 1, 100, _T(""), 0);

                // ����� ������ (������ ����� � �������)
                SetBkColor(hdc, RGB(255, 200, 200));  // ������-������� ���
                SetTextColor(hdc, RGB(255, 0, 0));    // ������� �����
                TextOut(hdc, thread2Pos, 100, szMsg, _tcslen(szMsg));

                ReleaseDC(hWnd, hdc);
            }

            thread2Pos += 5;  // ������ �������� ��������
            if (thread2Pos > rect.right)
                thread2Pos = -300;
        }

        Sleep(100);
    }

    return 0;
}

DWORD WINAPI AnimationThread(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    HDC hdc;
    RECT rect;

    while (TRUE)
    {
        if (animationState == 1) // ���� ����� �������� �������
        {
            hdc = GetDC(hWnd);
            if (hdc)
            {
                GetClientRect(hWnd, &rect);

                // ������� ���������� ��������� ������
                HBRUSH hWhiteBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hWhiteBrush);
                HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
                Ellipse(hdc, ballX - 1, ballY - 1, ballX + ballSize + 1, ballY + ballSize + 1);

                // ��������� ������� ������
                ballX += ballSpeedX;
                ballY += ballSpeedY;

                // ��������� ������������ � ���������
                if (ballX <= 0 || ballX + ballSize >= rect.right)
                {
                    ballSpeedX = -ballSpeedX; // ������ �� X
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // ������ ����
                }
                if (ballY <= 0 || ballY + ballSize >= rect.bottom)
                {
                    ballSpeedY = -ballSpeedY; // ������ �� Y
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // ������ ����
                }

                // ������ �����
                HBRUSH hBallBrush = CreateSolidBrush(ballColor);
                SelectObject(hdc, hBallBrush);
                Ellipse(hdc, ballX, ballY, ballX + ballSize, ballY + ballSize);

                // ��������������� � ������� �������
                SelectObject(hdc, hOldBrush);
                SelectObject(hdc, hOldPen);
                DeleteObject(hWhiteBrush);
                DeleteObject(hBallBrush);

                ReleaseDC(hWnd, hdc);
            }
        }
        Sleep(30); // �������� ��� ��������� ��������
    }
    return 0;
}

// �������� ������ 1
void CreateThread1(BOOL suspended)
{
    // ��������: ���� ����� ��� ������, ���������� ��������� �� ������
    if (hSecThread[1] != NULL)
    {
        MessageBox(NULL, _T("����� 1 ��� ������!"), _T("������"), MB_OK | MB_ICONWARNING);
        return;
    }

    // �������� ���������� �������� ����
    HWND hWnd = FindWindow(szClassName, NULL);
    // ������� �����: NULL - ������������ �� ���������, 0 - ������ ����� �� ���������,
    // ThreadFunc1 - ������� ������, hWnd - ��������, ������������ � �������,
    // CREATE_SUSPENDED ��� 0 - ����� ��������, &dwSecThreadId[1] - �������� ID ������
    hSecThread[1] = CreateThread(NULL, 0, ThreadFunc1, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwSecThreadId[1]);

    if (hSecThread[1])  // ���� ����� ������� ������
    {
        // ������������� ��������� � ���������
        threadStates[1] = suspended ? 2 : 1;  // 2 - �������������, 1 - �������
        threadPriorities[1] = THREAD_PRIORITY_NORMAL;  // ���������� ���������
        SetThreadPriority(hSecThread[1], threadPriorities[1]);  // ������������� ���������
        g_uThCount++;  // ����������� ������� �������
    }
}

// �������� ������ 2 (���������� ������ 1)
void CreateThread2(BOOL suspended)
{
    if (hSecThread[2] != NULL)
    {
        MessageBox(NULL, _T("����� 2 ��� ������!"), _T("������"), MB_OK | MB_ICONWARNING);
        return;
    }

    HWND hWnd = FindWindow(szClassName, NULL);
    hSecThread[2] = CreateThread(NULL, 0, ThreadFunc2, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwSecThreadId[2]);

    if (hSecThread[2])
    {
        threadStates[2] = suspended ? 2 : 1;
        threadPriorities[2] = THREAD_PRIORITY_NORMAL;
        SetThreadPriority(hSecThread[2], threadPriorities[2]);
        g_uThCount++;
    }
}

// ������������ ������ 1
void SuspendThread1()
{
    // ���������: ���� ����� ���������� � �������
    if (hSecThread[1] && threadStates[1] == 1)
    {
        SuspendThread(hSecThread[1]);  // ���������������� �����
        threadStates[1] = 2;           // ������������� ��������� "�������������"
    }
}

// ������������� ������ 1
void ResumeThread1()
{
    // ���������: ���� ����� ���������� � �������������
    if (hSecThread[1] && threadStates[1] == 2)
    {
        ResumeThread(hSecThread[1]);  // ������������ �����
        threadStates[1] = 1;          // ������������� ��������� "�������"
    }
}

// ���������� ������ 1
void TerminateThread1()
{
    if (hSecThread[1])
    {
        TerminateThread(hSecThread[1], 0);  // �������������� ���������� ������ � ����� 0
        CloseHandle(hSecThread[1]);         // ��������� ���������� ������
        hSecThread[1] = NULL;               // �������� ����������
        dwSecThreadId[1] = 0;               // �������� ID ������
        threadStates[1] = 0;                // ������������� ��������� "�� ������"
        g_uThCount--;                       // ��������� ������� �������

        // ������� ������� ������ (�������������� ����)
        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);  // ���������� ��������� WM_PAINT
    }
}

// ���������� ���������� ������ 1
void IncreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);  // ������� ���������
        // ���������, ����� �� ��������� ��������� (�� ���� HIGHEST)
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority + 1);  // ����������� ���������
            threadPriorities[1] = currentPriority + 1;              // ��������� ����� ��������
        }
    }
}

// ���������� ���������� ������ 1
void DecreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);
        // ���������, ����� �� ��������� ��������� (�� ���� LOWEST)
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority - 1);
            threadPriorities[1] = currentPriority - 1;
        }
    }
}

// ������� ��� ������ 2 (���������� �������� ������ 1)
void SuspendThread2()
{
    if (hSecThread[2] && threadStates[2] == 1)
    {
        SuspendThread(hSecThread[2]);
        threadStates[2] = 2;
    }
}

void ResumeThread2()
{
    if (hSecThread[2] && threadStates[2] == 2)
    {
        ResumeThread(hSecThread[2]);
        threadStates[2] = 1;
    }
}

void TerminateThread2()
{
    if (hSecThread[2])
    {
        TerminateThread(hSecThread[2], 0);
        CloseHandle(hSecThread[2]);
        hSecThread[2] = NULL;
        dwSecThreadId[2] = 0;
        threadStates[2] = 0;
        g_uThCount--;

        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void IncreasePriority2()
{
    if (hSecThread[2])
    {
        int currentPriority = GetThreadPriority(hSecThread[2]);
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hSecThread[2], currentPriority + 1);
            threadPriorities[2] = currentPriority + 1;
        }
    }
}

void DecreasePriority2()
{
    if (hSecThread[2])
    {
        int currentPriority = GetThreadPriority(hSecThread[2]);
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hSecThread[2], currentPriority - 1);
            threadPriorities[2] = currentPriority - 1;
        }
    }
}

void CreateAnimationThread(BOOL suspended)
{
    if (hAnimationThread != NULL)
    {
        MessageBox(NULL, _T("����� �������� ��� ������!"), _T("������"), MB_OK | MB_ICONWARNING);
        return;
    }

    HWND hWnd = FindWindow(szClassName, NULL);
    hAnimationThread = CreateThread(NULL, 0, AnimationThread, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwAnimationThreadId);

    if (hAnimationThread)
    {
        animationState = suspended ? 2 : 1;
        animationPriority = THREAD_PRIORITY_NORMAL;
        SetThreadPriority(hAnimationThread, animationPriority);
        g_uThCount++;
    }
}

void SuspendAnimationThread()
{
    if (hAnimationThread && animationState == 1)
    {
        SuspendThread(hAnimationThread);
        animationState = 2;
    }
}

void ResumeAnimationThread()
{
    if (hAnimationThread && animationState == 2)
    {
        ResumeThread(hAnimationThread);
        animationState = 1;
    }
}

void TerminateAnimationThread()
{
    if (hAnimationThread)
    {
        TerminateThread(hAnimationThread, 0);
        CloseHandle(hAnimationThread);
        hAnimationThread = NULL;
        dwAnimationThreadId = 0;
        animationState = 0;
        g_uThCount--;

        // ������� ������� ��������
        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void IncreaseAnimationPriority()
{
    if (hAnimationThread)
    {
        int currentPriority = GetThreadPriority(hAnimationThread);
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hAnimationThread, currentPriority + 1);
            animationPriority = currentPriority + 1;
        }
    }
}

void DecreaseAnimationPriority()
{
    if (hAnimationThread)
    {
        int currentPriority = GetThreadPriority(hAnimationThread);
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hAnimationThread, currentPriority - 1);
            animationPriority = currentPriority - 1;
        }
    }
}

// ���������� ��������� �������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:  // ��������� ������ ���� � ������ ��������� ����������
    {
        int wmId = LOWORD(wParam);  // ��������� ID �������
        switch (wmId)
        {
        case 1001: // IDM_ABOUT - "� ���������"
            DialogBox(hInst, MAKEINTRESOURCE(102), hWnd, AboutDlgProc);
            break;

        case 1002: // IDM_EXIT - "�����"
            DestroyWindow(hWnd);  // ��������� ����
            break;

            // ������� ��� ������ 1
        case 1101: // IDM_THREAD1_CREATE - "������� �����"
            CreateThread1(FALSE);  // ������� �������� �����
            break;
        case 1102: // IDM_THREAD1_CREATE_SUSPENDED - "������� ������ �����"
            CreateThread1(TRUE);   // ������� ���������������� �����
            break;
        case 1103: // IDM_THREAD1_SUSPEND - "������������� �����"
            SuspendThread1();
            break;
        case 1104: // IDM_THREAD1_RESUME - "���������� ������ ������"
            ResumeThread1();
            break;
        case 1105: // IDM_THREAD1_TERMINATE - "���������� �����"
            TerminateThread1();
            break;
        case 1106: // IDM_THREAD1_INCREASE_PRIORITY - "��������� ���������"
            IncreasePriority1();
            break;
        case 1107: // IDM_THREAD1_DECREASE_PRIORITY - "��������� ���������"
            DecreasePriority1();
            break;

            // ������� ��� ������ 2 (���������� ������ 1)
        case 1201: // IDM_THREAD2_CREATE
            CreateThread2(FALSE);
            break;
        case 1202: // IDM_THREAD2_CREATE_SUSPENDED
            CreateThread2(TRUE);
            break;
        case 1203: // IDM_THREAD2_SUSPEND
            SuspendThread2();
            break;
        case 1204: // IDM_THREAD2_RESUME
            ResumeThread2();
            break;
        case 1205: // IDM_THREAD2_TERMINATE
            TerminateThread2();
            break;
        case 1206: // IDM_THREAD2_INCREASE_PRIORITY
            IncreasePriority2();
            break;
        case 1207: // IDM_THREAD2_DECREASE_PRIORITY
            DecreasePriority2();
            break;

            // ������� ���������� � �������
        case 1301: // IDM_INFO_PRIMARY - "��������� �����"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 0);
            break;
        case 1302: // IDM_INFO_THREAD1 - "����� 1"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 1);
            break;
        case 1303: // IDM_INFO_THREAD2 - "����� 2"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 2);
            break;
        case 1401: // IDM_ANIMATION_CREATE
            CreateAnimationThread(FALSE);
            break;
        case 1402: // IDM_ANIMATION_CREATE_SUSPENDED
            CreateAnimationThread(TRUE);
            break;
        case 1403: // IDM_ANIMATION_SUSPEND
            SuspendAnimationThread();
            break;
        case 1404: // IDM_ANIMATION_RESUME
            ResumeAnimationThread();
            break;
        case 1405: // IDM_ANIMATION_TERMINATE
            TerminateAnimationThread();
            break;
        case 1406: // IDM_ANIMATION_INCREASE_PRIORITY
            IncreaseAnimationPriority();
            break;
        case 1407: // IDM_ANIMATION_DECREASE_PRIORITY
            DecreaseAnimationPriority();
            break;
        case 1408: // IDM_INFO_ANIMATION
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 3);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);  // ����������� ���������
        }
    }
    break;

    case WM_PAINT:  // ��������� ����������� ����
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // ������ �����������
        // ����� ����� �������� �������������� ���������
        EndPaint(hWnd, &ps);  // ���������� �����������
    }
    break;

    case WM_DESTROY:  // ��������� �������� ����
        // ���������� ���� ������� ����� �������
        if (hSecThread[1]) TerminateThread1();
        if (hSecThread[2]) TerminateThread2();
        if (hAnimationThread) TerminateAnimationThread();
        PostQuitMessage(0);  // ���������� ��������� WM_QUIT ��� ������ �� �����
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);  // ����������� ��������� ��������� ���������
    }
    return 0;
}

// ���������� ������� "� ���������"
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:  // ������������� �������
        return TRUE;

    case WM_COMMAND:  // ��������� ������ � �������
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // ��������� ������
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ���������� ������� ���������� � ������
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int threadIndex;  // ��������� ������ ������ ����� ��������

    switch (message)
    {
    case WM_INITDIALOG:  // ������������� �������
        threadIndex = (int)lParam;  // �������� ������ ������ �� ���������
        UpdateThreadInfo(hDlg, threadIndex);  // ��������� ���������� � ������
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ���������� ���������� � ������ � ���������� ����
void UpdateThreadInfo(HWND hDlg, int threadIndex)
{
    TCHAR buffer[256];
    DWORD exitCode;
    FILETIME creationTime, exitTime, kernelTime, userTime;
    SYSTEMTIME sysTime;
    ULONGLONG lifeTime;

    // ��������� - ��� ������
    if (threadIndex == 0)
        SetDlgItemText(hDlg, 1401, _T("��������� �����"));
    else if (threadIndex == 1)
        SetDlgItemText(hDlg, 1401, _T("����� 1"));
    else if (threadIndex == 2)
        SetDlgItemText(hDlg, 1401, _T("����� 2"));
    else if (threadIndex == 3)
        SetDlgItemText(hDlg, 1401, _T("����� ��������"));
    else
    {
        _stprintf(buffer, _T("����� %d"), threadIndex);
        SetDlgItemText(hDlg, 1401, buffer);
    }

    // ID ������
    if (threadIndex == 0)
        dwSecThreadId[0] = GetCurrentThreadId();

    if (threadIndex == 3) // ����� ��������
    {
        _stprintf(buffer, _T("0x%08X"), dwAnimationThreadId);
    }
    else
    {
        _stprintf(buffer, _T("0x%08X"), dwSecThreadId[threadIndex]);
    }
    SetDlgItemText(hDlg, 1402, buffer);

    // ���������� ������
    if (threadIndex == 0)
        hSecThread[0] = GetCurrentThread();

    if (threadIndex == 3) // ����� ��������
    {
        _stprintf(buffer, _T("0x%p"), hAnimationThread);
    }
    else
    {
        _stprintf(buffer, _T("0x%p"), hSecThread[threadIndex]);
    }
    SetDlgItemText(hDlg, 1403, buffer);

    // ��������� ������
    if (threadIndex == 0)
    {
        SetDlgItemText(hDlg, 1404, _T("��������"));
    }
    else if (threadIndex == 3) // ����� ��������
    {
        if (hAnimationThread)
        {
            if (GetExitCodeThread(hAnimationThread, &exitCode))
            {
                if (exitCode == STILL_ACTIVE)
                {
                    if (animationState == 1)
                        SetDlgItemText(hDlg, 1404, _T("��������"));
                    else
                        SetDlgItemText(hDlg, 1404, _T("����������������"));
                }
                else
                {
                    _stprintf(buffer, _T("�������� (%lu)"), exitCode);
                    SetDlgItemText(hDlg, 1404, buffer);
                }
            }
        }
        else
        {
            SetDlgItemText(hDlg, 1404, _T("�� ������"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        if (GetExitCodeThread(hSecThread[threadIndex], &exitCode))
        {
            if (exitCode == STILL_ACTIVE)
            {
                if (threadStates[threadIndex] == 1)
                    SetDlgItemText(hDlg, 1404, _T("��������"));
                else
                    SetDlgItemText(hDlg, 1404, _T("����������������"));
            }
            else
            {
                _stprintf(buffer, _T("�������� (%lu)"), exitCode);
                SetDlgItemText(hDlg, 1404, buffer);
            }
        }
    }
    else
    {
        SetDlgItemText(hDlg, 1404, _T("�� ������"));
    }

    // ��������� ������
    if (threadIndex == 0)
    {
        int priority = GetThreadPriority(GetCurrentThread());
        // ... ��������� ����������
    }
    else if (threadIndex == 3) // ����� ��������
    {
        if (hAnimationThread)
        {
            int priority = GetThreadPriority(hAnimationThread);
            switch (priority)
            {
            case THREAD_PRIORITY_IDLE: _tcscpy(buffer, _T("IDLE")); break;
            case THREAD_PRIORITY_LOWEST: _tcscpy(buffer, _T("LOWEST")); break;
            case THREAD_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("BELOW_NORMAL")); break;
            case THREAD_PRIORITY_NORMAL: _tcscpy(buffer, _T("NORMAL")); break;
            case THREAD_PRIORITY_ABOVE_NORMAL: _tcscpy(buffer, _T("ABOVE_NORMAL")); break;
            case THREAD_PRIORITY_HIGHEST: _tcscpy(buffer, _T("HIGHEST")); break;
            case THREAD_PRIORITY_TIME_CRITICAL: _tcscpy(buffer, _T("TIME_CRITICAL")); break;
            default: _stprintf(buffer, _T("%d"), priority);
            }
            SetDlgItemText(hDlg, 1405, buffer);
        }
        else
        {
            SetDlgItemText(hDlg, 1405, _T("�� ��������"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        // ... ������������ ��������� ��� ������� 1 � 2
    }
    else
    {
        SetDlgItemText(hDlg, 1405, _T("�� ��������"));
    }

    // ����� ������ ������
    if (threadIndex == 0)
    {
        HANDLE hThread = GetCurrentThread();
        // ... ��������� ������� ������
    }
    else if (threadIndex == 3) // ����� ��������
    {
        if (hAnimationThread)
        {
            if (GetThreadTimes(hAnimationThread, &creationTime, &exitTime, &kernelTime, &userTime))
            {
                FileTimeToSystemTime(&creationTime, &sysTime);
                ULARGE_INTEGER ulCreation;
                ulCreation.LowPart = creationTime.dwLowDateTime;
                ulCreation.HighPart = creationTime.dwHighDateTime;

                FILETIME now;
                GetSystemTimeAsFileTime(&now);
                ULARGE_INTEGER ulNow;
                ulNow.LowPart = now.dwLowDateTime;
                ulNow.HighPart = now.dwHighDateTime;

                lifeTime = (ulNow.QuadPart - ulCreation.QuadPart) / 10000;
                _stprintf(buffer, _T("%llu ��"), lifeTime);
                SetDlgItemText(hDlg, 1406, buffer);
            }
            else
            {
                SetDlgItemText(hDlg, 1406, _T("�� ��������"));
            }
        }
        else
        {
            SetDlgItemText(hDlg, 1406, _T("�� ��������"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        // ... ������������ ��������� ��� ������� 1 � 2
    }
    else
    {
        SetDlgItemText(hDlg, 1406, _T("�� ��������"));
    }
}