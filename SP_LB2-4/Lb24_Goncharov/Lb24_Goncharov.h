#pragma once
// Lb24_Goncharov.h
// ������������ ���� ������������ ����������
// �������� ���������� �������������� ������� � ����������

// ������ ��� ���������� ��������/���������
#ifdef LB24_GONCHAROV_EXPORTS
#define LB24_API __declspec(dllexport) // ������� ��� ���������� DLL
#else
#define LB24_API __declspec(dllimport) // ������ ��� ���������� �������
#endif

// ���������� ��� �������������� ���������� ����������
// extern ���������, ��� ���������� ���������� � ������ �����
extern LB24_API int g_nDllCallsCount; // ������� �������� ���������� (�����������)
extern LB24_API int g_nFnCallsCount;   // ������� ������� ������� (� ������ ��������)

// ���������� ��� �������������� �������
// extern "C" ������������� ������������� ���� C++ ������������
extern "C" LB24_API int WINAPI  Fun31(double a, double b);
extern "C" LB24_API float Fun32(int a, int b, int c);
extern "C" LB24_API void Fun33(int in, int* out);