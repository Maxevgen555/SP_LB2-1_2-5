#pragma once

// ����������� ����������� ������������ ������ Windows API
#include <windows.h>      // �������� ������� Windows API
#include <tchar.h>        // ��������� Unicode � ANSI ��������
#include <commctrl.h>     // �������� ���������� ������ �������
#include <tlhelp32.h>     // ������� ��� ������ � ���������� � ��������

// �������� ����� ����� ��� Windows ���������� (������ ����������� main)
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")

// ����������� ������������� ��� ����������� ��������� ����������
#ifndef IDC_STATIC
#define IDC_STATIC                      -1
#endif

// ================= ��������� ���� =================
// ������� ���� "��������"
#define IDM_PROCESS_NOTEPAD 1001        // ������ ��������
#define IDM_PROCESS_NOTEPAD_TEXT 1002   // ������ �������� � ��������� ������
#define IDM_PROCESS_CALC 1003           // ������ ������������
#define IDM_PROCESS_CLOSE_NOTEPAD 1004  // �������� ���� ���� ��������
#define IDM_PROCESS_TESTPROC 1005        // ����� ������� ��� TestProc
#define IDM_PROCESS_NOTEPAD_WAIT 1006    // ����� ������� ��� �������� � ���������

// ������� ���� "���������� � ���������"
#define IDM_INFO_PROCESS0 2001          // ���������� � ������� �������� (0)
#define IDM_INFO_PROCESS1 2002          // ���������� � �������� �������� (1)
#define IDM_INFO_PROCESS3 2003          // ���������� � �������� ������������ (3)
#define IDM_INFO_PROCESS2 2004          // ���������� � �������� �������� � ������� (2)

// ================= ���������� ���� =================
#define IDD_PROCESS_INFO 3001           // ID ����������� ���� � ����������� � ��������
#define IDD_SELECT_FILE 3002             // ����� ���������� ���� ������ �����

// ================= �������� ����������� ���� =================
// ���� ��� ����������� ���������� � ��������
#define IDC_EDIT_FILENAME 4001          // ����: ��� ������������ �����
#define IDC_EDIT_PARAMS 4002            // ����: ��������� ��������� ������
#define IDC_EDIT_PROC_HANDLE 4003       // ����: ���������� ��������
#define IDC_EDIT_PROC_ID 4004           // ����: ������������� �������� (PID)
#define IDC_EDIT_THREAD_HANDLE 4005     // ����: ���������� ��������� ������
#define IDC_EDIT_THREAD_ID 4006         // ����: ������������� ��������� ������ (TID)
#define IDC_EDIT_PROC_EXIT_CODE 4007    // ����: ��� ���������� ��������
#define IDC_EDIT_THREAD_EXIT_CODE 4008  // ����: ��� ���������� ������
#define IDC_EDIT_PRIORITY_CLASS 4009    // ����: ����� ���������� ��������
#define IDC_EDIT_LIFE_TIME 4010         // ����: ����� ����� �������� (��)
#define IDC_EDIT_USER_TIME 4011         // ����: ����� � ������ ������������ (��)
#define IDC_EDIT_KERNEL_TIME 4012       // ����: ����� � ������ ���� (��)
#define IDC_EDIT_IDLE_TIME 4013         // ����: ����� ������� (��)
// �������� ������� ������ �����
#define IDC_FILE_PATH 5001
#define IDC_BROWSE_BUTTON 5002

#define IDD_TESTPROC_PARAMS 5001
#define IDC_EDIT_X 5002
#define IDC_EDIT_Y 5003
#define IDC_EDIT_WIDTH 5004
#define IDC_EDIT_HEIGHT 5005
#define IDC_COMBO_STYLE 5006

// �������� ������� �������
INT_PTR CALLBACK TestProcParamsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// ================= ���������� ���������� =================
// ��� ���������� �������� �� ���� ������� ���������

extern HINSTANCE hInst;                 // ���������� ���������� ����������
extern TCHAR szClassName[];             // ��� ������ �������� ����
extern TCHAR szTitle[];                 // ��������� �������� ����

// ================= ������� ��� �������� ���������� � ��������� =================
// ������� ��������:
// 0 - ������� ������� (���� ����������)
// 1 - ������� ��������
// 2 - ������� �������� � ��������� ������  
// 3 - ������� ������������

extern HANDLE ProcHandle[5];            // ������ ������������ ���������
extern DWORD ProcId[5];                 // ������ ��������������� ��������� (PID)
extern HANDLE ThreadHandle[5];          // ������ ������������ �������� �������
extern DWORD ThreadId[5];               // ������ ��������������� ������� (TID)
extern LPTSTR ProcImage[5];             // ������ ����� � ����������� ������
extern TCHAR CmdParam[5][260];          // ������ ���������� ��������� ������

// ================= ��������� ������� =================
// ���������� ������� ��� �����������

// �������� ��������� ���� - ������������ ��� ��������� �������� ����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ������������� ���������� - ����������� ������ ����
BOOL InitApplication(HINSTANCE);

// �������� ���������� ���������� - �������� �������� ����
BOOL InitInstance(HINSTANCE, int);

// ������������� ������ � ��������� - ���������� �������� ���������� ����������
void InitializeProcessData();

// ������ �������� �� ���������� �������
BOOL StartProcess(int index);

// ����� � �������� ���� ���� ��������
BOOL FindAndCloseNotepad();

// ��������� ����������� ���� � ����������� � ��������
INT_PTR CALLBACK ProcessInfoDlgProc(HWND, UINT, WPARAM, LPARAM);

// ���������� ����������� ���� ����������� � ��������
void FillProcessInfo(HWND hDlg, int index);

BOOL RunNotepadAndWait(LPCTSTR filename);  // ����� �������
void DisplayFileContent(LPCTSTR filename); // ����� �������
BOOL StartProcessWithStartupInfo(int index, STARTUPINFO* si);
INT_PTR CALLBACK SelectFileDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);