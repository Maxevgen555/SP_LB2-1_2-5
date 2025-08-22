// ClientTextProcessor_Explicit.cpp
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "../TextProcessor/TextProcessor.h"

// Добавьте эту pragma для явного указания .lib файла
#pragma comment(lib, "TextProcessor.lib")

typedef void(__cdecl* ProcessFileType)(const char*);

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    const char* filename = "test.txt";

    printf("Обработка файла '%s'...\n", filename);

    // Явная загрузка DLL
    HINSTANCE hDll = LoadLibraryA("TextProcessor.dll");
    if (!hDll) {
        printf("Ошибка загрузки DLL! Код: %lu\n", GetLastError());
        system("pause");
        return 1;
    }

    // Получаем адрес функции
    ProcessFileType ProcessFile = (ProcessFileType)GetProcAddress(hDll, "ProcessFileAndAppendWordCount");
    if (!ProcessFile) {
        printf("Функция не найдена! Код: %lu\n", GetLastError());
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    // Вызываем функцию
    ProcessFile(filename);

    printf("Обработка завершена. Результат записан в конец файла.\n");

    // Запускаем Блокнот
    HINSTANCE result = ShellExecuteA(NULL, "open", "notepad.exe", filename, NULL, SW_SHOWNORMAL);

    FreeLibrary(hDll);
    system("pause");
    return 0;
}