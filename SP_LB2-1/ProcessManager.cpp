#include "ProcessManager.h"  // ����������� ������������� ����� � �������������
#include <stdio.h>           // ����������� ������� �����-������
#include <psapi.h>           // ������� ��� ������ � ���������� � ��������

// ��������� ������� ������������ ������� ����������, ����� ����� wWinMainCRTStartup
// ��� ��������� ������������ Unicode ������ WinMain (_tWinMain)

// ================= ������������� ���������� ���������� =================

// ���������� ���������� ���������� (����� ���������� � WinMain)
HINSTANCE hInst;

// ��� ������ �������� ���� ����������
TCHAR szClassName[] = _T("ProcessManagerApp");

// ��������� �������� ���� � ����������� � �������� � ������
TCHAR szTitle[100] = _T("SP-LB2-1 �������� �.�. ��. 40322");

// ������� ��� �������� ���������� � ��������� (4 ��������: 0-3)
HANDLE ProcHandle[4] = { NULL };        // ����������� ���������
DWORD ProcId[4] = { 0 };                // �������������� ��������� (PID)
HANDLE ThreadHandle[4] = { NULL };      // ����������� �������� �������
DWORD ThreadId[4] = { 0 };              // �������������� ������� (TID)
LPTSTR ProcImage[4] = { NULL };         // ���� � ����������� ������
TCHAR CmdParam[4][260] = { {0} };       // ��������� ��������� ������

// ================= ����� ����� ���������� =================

// ������� ������� Windows ����������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;  // ��������� ��� �������� ���������

    // ������������� ������ � ��������� - ���������� �������� ���������� ����������
    InitializeProcessData();

    // ����������� ������ ���� - �������� ������� ��� �������� ����
    if (!InitApplication(hInstance)) {
        return FALSE;  // ����� ��� ������ �����������
    }

    // �������� � ����������� �������� ���� ����������
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;  // ����� ��� ������ �������� ����
    }

    // �������� ���� ��������� ��������� ����������
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  // �������������� ��������� ������
        DispatchMessage(&msg);   // �������� ��������� ��������� ����
    }

    // ������� ���� ���������� ����������
    return (int)msg.wParam;
}

// ================= ������������� ������ � ��������� =================

void InitializeProcessData() {
    // ������� ������� (������ 0) - ���� ����������
    ProcId[0] = GetCurrentProcessId();  // ��������� PID �������� ��������
    // �������� ����������� �������� �������� � ������� �� ������ ���������� � ������ ������
    ProcHandle[0] = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcId[0]);
    ThreadId[0] = GetCurrentThreadId();  // ��������� TID �������� ������
    // �������� ����������� �������� ������ � ������� �� ������ ����������
    ThreadHandle[0] = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId[0]);

    // ��������� ���� � ������������ ����� �������� ��������
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    ProcImage[0] = _tcsdup(path);  // ����������� ������ � ������������ ������

    // ��������� � ���������� ��������� ������ �������� ��������
    LPTSTR cmdLine = GetCommandLine();
    _tcscpy_s(CmdParam[0], _countof(CmdParam[0]), cmdLine);

    // ������� ������� (������ 1) - ������� ������� ��� ����������
    ProcImage[1] = _tcsdup(_T("notepad.exe"));  // ���� � ������������ �����
    _tcscpy_s(CmdParam[1], _countof(CmdParam[1]), _T(""));  // ������ ���������

    // ������� ������� � ������� (������ 2) - ������� � ��������� �����
    ProcImage[2] = _tcsdup(_T("notepad.exe"));
    _tcscpy_s(CmdParam[2], _countof(CmdParam[2]), _T("Test.txt"));  // �������� - ���� ��� ��������

    // ������� ����������� (������ 3) - ����������� ����������� Windows
    ProcImage[3] = _tcsdup(_T("calc.exe"));
    _tcscpy_s(CmdParam[3], _countof(CmdParam[3]), _T(""));  // ��� ����������
}

// ================= ����������� ������ ���� =================

BOOL InitApplication(HINSTANCE hInstance) {
    WNDCLASSEX wcex;  // ��������� ��� �������� ������ ����

    // ���������� ��������� WNDCLASSEX
    wcex.cbSize = sizeof(WNDCLASSEX);          // ������ ���������
    wcex.style = CS_HREDRAW | CS_VREDRAW;      // ����� - ����������� ��� ��������� �������
    wcex.lpfnWndProc = WndProc;                // ��������� �� ��������� ����
    wcex.cbClsExtra = 0;                       // �������������� ������ ��� ������
    wcex.cbWndExtra = 0;                       // �������������� ������ ��� ����
    wcex.hInstance = hInstance;                // ���������� ����������
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // ����������� ������
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);    // ����������� ������
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // ���� ���� ����
    wcex.lpszMenuName = NULL;                  // ���� ����� ������� �����
    wcex.lpszClassName = szClassName;          // ��� ������ ����
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);  // ��������� ������

    // ����������� ������ ���� � �������
    return RegisterClassEx(&wcex);
}

// ================= �������� �������� ���� =================

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hWnd;  // ���������� ������������ ����

    hInst = hInstance;  // ���������� ����������� ����������

    // �������� �������� ���� ����������
    hWnd = CreateWindow(szClassName,           // ��� ������ ����
        szTitle,               // ��������� ����
        WS_OVERLAPPEDWINDOW,   // ����� ����
        CW_USEDEFAULT,         // ������� X �� ���������
        CW_USEDEFAULT,         // ������� Y �� ���������
        800,                   // ������ ����
        600,                   // ������ ����
        NULL,                  // ������������ ����
        NULL,                  // ����
        hInstance,             // ���������� ����������
        NULL);                 // �������������� ���������

    if (!hWnd) {
        return FALSE;  // ����� ��� ������ �������� ����
    }

    // ================= �������� ���� =================

    HMENU hMenu = CreateMenu();                // ������� ����
    HMENU hProcessMenu = CreatePopupMenu();    // ������� "��������"
    HMENU hInfoMenu = CreatePopupMenu();       // ������� "���������� � ���������"

    // ���������� ������� � ���� "��������"
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_NOTEPAD, _T("�������"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_NOTEPAD_TEXT, _T("������� � �������"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_CALC, _T("�����������"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_CLOSE_NOTEPAD, _T("������� �������"));

    // ���������� ������� � ���� "���������� � ���������"
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS0, _T("������� �������(0)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS1, _T("������� (1)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS2, _T("������� � ������� (2)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS3, _T("����������� (3)"));

    // ���������� ������� � ������� ����
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hProcessMenu, _T("��������"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hInfoMenu, _T("���������� � ���������"));

    // ��������� ���� ��� ����
    SetMenu(hWnd, hMenu);

    // ����������� � ���������� ����
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// ================= ������� ������ � �������� �������� =================

BOOL FindAndCloseNotepad() {
    // �������� ������ ���� ��������� � �������
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;  // ������ �������� ������
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);  // ����� ���������� ������ ���������

    // ��������� ���������� � ������ �������� � ������
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return FALSE;  // ������ ��������� ����������
    }

    BOOL found = FALSE;
    // ������� ���� ��������� � ������
    do {
        // �������� ����� �������� (��� ����� ��������)
        if (_tcsicmp(pe32.szExeFile, _T("notepad.exe")) == 0) {
            // �������� �������� � ������� �� ����������
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                // �������������� ���������� �������� � ����� 40322 (����� ������)
                TerminateProcess(hProcess, 40322);
                CloseHandle(hProcess);
                found = TRUE;  // ������� ������ � ������
            }
        }
    } while (Process32Next(hSnapshot, &pe32));  // ������� � ���������� ��������

    CloseHandle(hSnapshot);  // �������� handle ������
    return found;
}

// ================= ������ �������� =================

BOOL StartProcess(int index) {
    PROCESS_INFORMATION pi;  // ���������� � ��������� ��������
    STARTUPINFO si;          // ��������� ���������� ��� ��������

    // ������������� �������� ������
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);  // ������ ��������� ����������
    ZeroMemory(&pi, sizeof(pi));

    TCHAR commandLine[520];
    // ������������ ��������� ������
    if (_tcslen(CmdParam[index]) > 0) {
        // ���� ���� ��������� - ��������� �� � ��������
        _stprintf_s(commandLine, _countof(commandLine), _T("\"%s\" %s"), ProcImage[index], CmdParam[index]);
    }
    else {
        // ��� ���������� - ������ ���� � exe
        _tcscpy_s(commandLine, _countof(commandLine), ProcImage[index]);
    }

    // �������� ������ ��������
    if (!CreateProcess(NULL,           // ��� ������ (������������ commandLine)
        commandLine,     // ��������� ������
        NULL,            // �������� ������������ ��������
        NULL,            // �������� ������������ ������
        FALSE,           // ������������ handle
        0,               // ����� ��������
        NULL,            // ���������
        NULL,            // ������� �������
        &si,             // STARTUPINFO
        &pi)) {          // PROCESS_INFORMATION
        // ��������� ������ �������� ��������
        DWORD error = GetLastError();
        TCHAR errorMsg[256];
        _stprintf_s(errorMsg, _countof(errorMsg), _T("�� ������� ��������� �������. ��� ������: %lu"), error);
        MessageBox(NULL, errorMsg, _T("������"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // ���������� ���������� � ��������� ��������
    ProcHandle[index] = pi.hProcess;    // ���������� ��������
    ProcId[index] = pi.dwProcessId;     // ID ��������
    ThreadHandle[index] = pi.hThread;   // ���������� ��������� ������
    ThreadId[index] = pi.dwThreadId;    // ID ��������� ������

    // ��������� ���������� ������ - �� �� ����� ��� ���������� ������
    CloseHandle(pi.hThread);
    ThreadHandle[index] = NULL;

    return TRUE;
}

// ================= ��������� �������� ���� =================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:  // ��������� ������ ����
    {
        int wmId = LOWORD(wParam);  // ID ������� ����
        switch (wmId) {
        case IDM_PROCESS_NOTEPAD:        // ������ �������� ��������
            StartProcess(1);
            break;
        case IDM_PROCESS_NOTEPAD_TEXT:   // ������ �������� � ������
            StartProcess(2);
            break;
        case IDM_PROCESS_CALC:           // ������ ������������
            StartProcess(3);
            break;
        case IDM_PROCESS_CLOSE_NOTEPAD:  // �������� ���� ���������
            if (!FindAndCloseNotepad()) {
                MessageBox(hWnd, _T("�� ������� ����� ��� ������� �������"), _T("������"), MB_OK | MB_ICONERROR);
            }
            else {
                // ����� ���������� � �������� �������� (������ 1)
                if (ProcHandle[1]) {
                    CloseHandle(ProcHandle[1]);
                    ProcHandle[1] = NULL;
                }
                ProcId[1] = 0;
                ThreadId[1] = 0;
            }
            break;
        case IDM_INFO_PROCESS0:  // ���������� � ������� ��������
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 0);
            break;
        case IDM_INFO_PROCESS1:  // ���������� � �������� ��������
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 1);
            break;
        case IDM_INFO_PROCESS2:  // ���������� � �������� �������� � �������
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 2);
            break;
        case IDM_INFO_PROCESS3:  // ���������� � �������� ������������
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 3);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:  // �������� ����
        // ������� �������� - �������� ���� �������� ������������
        for (int i = 0; i < 4; i++) {
            if (ProcHandle[i] && i != 0) {  // �� ��������� ���������� �������� ��������
                DWORD exitCode;
                // �������� ���������� �������� ����� ��������� �����������
                if (GetExitCodeProcess(ProcHandle[i], &exitCode) && exitCode == STILL_ACTIVE) {
                    // ������� ��� ������� - ����� ���������� ������������ ������� ���
                }
                CloseHandle(ProcHandle[i]);
            }
            if (ThreadHandle[i] && i != 0) {  // �� ��������� ���������� �������� ������
                CloseHandle(ThreadHandle[i]);
            }
            if (ProcImage[i]) {
                free(ProcImage[i]);  // ������������ ������, ���������� ��� ����
            }
        }
        PostQuitMessage(0);  // ���������� ����� ���������
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ================= ��������� ����������� ���� =================

INT_PTR CALLBACK ProcessInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int index;  // ���������� ������� �������� ����� ��������

    switch (message) {
    case WM_INITDIALOG:  // ������������� ����������� ����
        index = (int)lParam;  // ��������� ������� �������� �� ���������
        FillProcessInfo(hDlg, index);  // ���������� ���� �����������
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {  // ������� ������ OK
            EndDialog(hDlg, 0);  // �������� ����������� ����
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ================= ���������� ���������� � �������� =================

void FillProcessInfo(HWND hDlg, int index) {
    HANDLE hProcess = ProcHandle[index];    // ���������� ��������
    HANDLE hThread = ThreadHandle[index];   // ���������� ������
    BOOL needCloseProcess = FALSE;          // ���� ��� �������� �������� ������������
    BOOL needCloseThread = FALSE;
    BOOL processActive = FALSE;             // ���� ���������� ��������

    // ����������� ��������� ��� �������� �������� (������ 1)
    if (index == 1 && hProcess == NULL && ProcId[1] != 0) {
        // ������� ������� ���������� �������� ��������, ���� �� ��� ������
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcId[1]);
        if (hProcess) {
            needCloseProcess = TRUE;
            // �������� ���������� ��������
            DWORD exitCode;
            if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                ProcHandle[1] = hProcess;      // ���������� �����������
                needCloseProcess = FALSE;      // �� ��������� - ��������� � �������
            }
        }
    }

    // ����������� ��������� ��� ����������� ������
    if (index == 1 && hThread == NULL && ThreadId[1] != 0) {
        hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId[1]);
        if (hThread) {
            needCloseThread = TRUE;
        }
    }

    TCHAR buffer[260];  // ����� ��� �������������� �����

    // ���������� ����� ����������� ���� ����������� � ��������

    // ��� ������������ �����
    SetDlgItemText(hDlg, IDC_EDIT_FILENAME, ProcImage[index]);

    // ��������� ��������� ������
    SetDlgItemText(hDlg, IDC_EDIT_PARAMS, CmdParam[index]);

    // ���������� �������� � ����������������� �������
    _stprintf_s(buffer, _countof(buffer), _T("0x%p"), hProcess);
    SetDlgItemText(hDlg, IDC_EDIT_PROC_HANDLE, buffer);

    // ID �������� (PID)
    _stprintf_s(buffer, _countof(buffer), _T("%lu"), ProcId[index]);
    SetDlgItemText(hDlg, IDC_EDIT_PROC_ID, buffer);

    // ���������� ������
    _stprintf_s(buffer, _countof(buffer), _T("0x%p"), hThread);
    SetDlgItemText(hDlg, IDC_EDIT_THREAD_HANDLE, buffer);

    // ID ������ (TID)
    _stprintf_s(buffer, _countof(buffer), _T("%lu"), ThreadId[index]);
    SetDlgItemText(hDlg, IDC_EDIT_THREAD_ID, buffer);

    // ��� ���������� ��������
    DWORD exitCode;
    if (hProcess && GetExitCodeProcess(hProcess, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, _T("��������� - �������"));
            processActive = TRUE;  // ������� �������
        }
        else {
            _stprintf_s(buffer, _countof(buffer), _T("%lu"), exitCode);
            SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, buffer);
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, _T("�� ��������"));
    }

    // ��� ���������� ������
    if (hThread && GetExitCodeThread(hThread, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, _T("��������� - �������"));
        }
        else {
            _stprintf_s(buffer, _countof(buffer), _T("%lu"), exitCode);
            SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, buffer);
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, _T("�� ��������"));
    }

    // ����� ���������� ��������
    if (hProcess) {
        DWORD priorityClass = GetPriorityClass(hProcess);
        if (priorityClass) {
            switch (priorityClass) {
            case IDLE_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("IDLE"));
                break;
            case NORMAL_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("NORMAL"));
                break;
            case HIGH_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("HIGH"));
                break;
            case REALTIME_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("REALTIME"));
                break;
            default:
                _stprintf_s(buffer, _countof(buffer), _T("%lu"), priorityClass);
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, buffer);
                break;
            }
        }
        else {
            SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("�� ��������"));
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("�� ��������"));
    }

    // ��������� �������������� �������� (������ ��� �������� ���������)
    if (hProcess && processActive) {
        FILETIME createTime, exitTime, kernelTime, userTime;
        if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
            // �������������� FILETIME � 64-������ �������� ��� ����������
            ULARGE_INTEGER ulCreateTime, ulKernelTime, ulUserTime;

            ulCreateTime.LowPart = createTime.dwLowDateTime;
            ulCreateTime.HighPart = createTime.dwHighDateTime;

            ulKernelTime.LowPart = kernelTime.dwLowDateTime;
            ulKernelTime.HighPart = kernelTime.dwHighDateTime;

            ulUserTime.LowPart = userTime.dwLowDateTime;
            ulUserTime.HighPart = userTime.dwHighDateTime;

            // ��������� �������� ���������� �������
            FILETIME now;
            GetSystemTimeAsFileTime(&now);
            ULARGE_INTEGER ulNow;
            ulNow.LowPart = now.dwLowDateTime;
            ulNow.HighPart = now.dwHighDateTime;

            // ���������� ������� ����� �������� (� �������������)
            ULONGLONG lifeTime = (ulNow.QuadPart - ulCreateTime.QuadPart) / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu ��"), lifeTime);
            SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, buffer);

            // ����� ���������� � ������ ������������
            ULONGLONG userTimeMs = ulUserTime.QuadPart / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu ��"), userTimeMs);
            SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, buffer);

            // ����� ���������� � ������ ����
            ULONGLONG kernelTimeMs = ulKernelTime.QuadPart / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu ��"), kernelTimeMs);
            SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, buffer);

            // ����� ������� (����� ����� ����� ����� CPU)
            ULONGLONG idleTime = lifeTime - (userTimeMs + kernelTimeMs);
            _stprintf_s(buffer, _countof(buffer), _T("%llu ��"), idleTime);
            SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, buffer);
        }
        else {
            // ������ ��������� ��������� �������������
            SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, _T("������� �� �������"));
            SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, _T("������� �� �������"));
            SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, _T("������� �� �������"));
            SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, _T("������� �� �������"));
        }
    }
    else {
        // ������� �� ������� ��� ����������
        SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, _T("������� �� �������"));
        SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, _T("������� �� �������"));
        SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, _T("������� �� �������"));
        SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, _T("������� �� �������"));
    }

    // �������� �������� �������� ������������
    if (needCloseProcess) {
        CloseHandle(hProcess);
    }
    if (needCloseThread) {
        CloseHandle(hThread);
    }
}