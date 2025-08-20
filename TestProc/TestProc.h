#pragma once

#include <windows.h>

// �������������� ��������� ����������
#define IDC_BUTTON_CLICK 1001  // ������ ��� �������
#define IDC_EDIT_INPUT   1002  // ���� ����� ������
#define IDC_STATIC_TEXT  1003  // ����������� ����� ��� ����������� ����������

// ��������� ������� ������� ���������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ������� ����������� ������ ����
BOOL RegisterWindowClass(HINSTANCE hInstance);

// ������� �������� �������� ���� � ����������� ������� � �������
HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, int x, int y, int width, int height);

// ����������� ���������
void OnCreate(HWND hwnd);                          // ��������� �������� ����
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify); // ��������� ������
void OnPaint(HWND hwnd);                           // ��������� ����������� ����
void OnDestroy(HWND hwnd);                         // ��������� �������� ����
