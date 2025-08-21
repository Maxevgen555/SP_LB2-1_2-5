// thread.cpp : Defines the entry point for the console application.
//    
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <Windows.h>
#include <process.h>
#include <stdio.h>
#include <psapi.h>
#include <commdlg.h> 

void for_delay() { for (int j = 0; j <= 1000000; j++) { int k = j; } }
void for_delay1() { for (int j = 0; j <= 10000000; j++) { int k = j; } }

DWORD WINAPI threadFunc(LPVOID lpvParam)
{
    wchar_t lpszMsg[100]; // Изменено на wchar_t
    wsprintf(lpszMsg, L"Thread parametr = %d ", *(DWORD*)lpvParam); // Добавлен L перед строкой
    wprintf(L" -- ThreadFunc is started!\n  %s\n", lpszMsg); // Использован wprintf

    int i = 30;
    while (i--) {
        for_delay();
        wsprintf(lpszMsg, L"Thread repetition %d", 30 - i);
        wprintf(L" -- ThreadFunc!  %s\n", lpszMsg);
        for_delay();
        for_delay();
    }

    wprintf(L" -- Ending of ThreadFunc! \n");
    return 0;
}

int main(int argc, char* argv[])
{
    DWORD dwThreadId, dwThreadParam = 12;
    HANDLE hThread;
    wchar_t lpszMsg[100]; // Изменено на wchar_t

    hThread = CreateThread(NULL, 0L, threadFunc, &dwThreadParam, 0L, &dwThreadId);
    wsprintf(lpszMsg, L"Thread is created , handle = %p", hThread); // %p для указателя
    wprintf(L"main! %s\n", lpszMsg); // Использован wprintf

    int i = 20;
    while (i--) {
        swprintf(lpszMsg, L"Repetition %d", 20 - i); // Использована swprintf
        for_delay1();
        wprintf(L"main!   %s\n", lpszMsg); // Использован wprintf
    }

    MessageBox(NULL, L"Завершение первичного потока.", L"MainFunc", MB_OK); // Добавлены L
    wsprintf(lpszMsg, L"Ending of Primery Thread.");
    wprintf(L"main!   %s\n", lpszMsg);
    return 0;
}