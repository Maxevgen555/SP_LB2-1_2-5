// ClientTextProcessor_Explicit.cpp
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "../TextProcessor/TextProcessor.h"

// �������� ��� pragma ��� ������ �������� .lib �����
#pragma comment(lib, "TextProcessor.lib")

typedef void(__cdecl* ProcessFileType)(const char*);

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    const char* filename = "test.txt";

    printf("��������� ����� '%s'...\n", filename);

    // ����� �������� DLL
    HINSTANCE hDll = LoadLibraryA("TextProcessor.dll");
    if (!hDll) {
        printf("������ �������� DLL! ���: %lu\n", GetLastError());
        system("pause");
        return 1;
    }

    // �������� ����� �������
    ProcessFileType ProcessFile = (ProcessFileType)GetProcAddress(hDll, "ProcessFileAndAppendWordCount");
    if (!ProcessFile) {
        printf("������� �� �������! ���: %lu\n", GetLastError());
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    // �������� �������
    ProcessFile(filename);

    printf("��������� ���������. ��������� ������� � ����� �����.\n");

    // ��������� �������
    HINSTANCE result = ShellExecuteA(NULL, "open", "notepad.exe", filename, NULL, SW_SHOWNORMAL);

    FreeLibrary(hDll);
    system("pause");
    return 0;
}