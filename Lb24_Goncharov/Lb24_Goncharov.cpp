#define LB24_GONCHAROV_EXPORTS
#include "Lb24_Goncharov.h"
#include <windows.h>
#include <iostream>

// Разделяемая секция данных
#pragma data_seg(".SHARED")
LB24_API int g_nDllCallsCount = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.SHARED,RWS")

// Обычная глобальная переменная
LB24_API int g_nFnCallsCount = 0;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_nDllCallsCount++;
        std::cout << "Загружается библиотека Lb24_Goncharov.dll. Проект Goncharov" << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        std::cout << "Выгружается библиотека Lb24_Goncharov.dll." << std::endl;
        break;
    case DLL_THREAD_ATTACH:
        std::cout << "Создан новый поток." << std::endl;
        break;
    case DLL_THREAD_DETACH:
        std::cout << "Завершается поток." << std::endl;
        break;
    }
    return TRUE;
}

// Реализации функций (ВСЕ в __cdecl)
LB24_API int Fun31(double a, double b) {
    g_nFnCallsCount++;
    return (int)(a + b);
}

LB24_API float Fun32(int a, int b, int c) {
    g_nFnCallsCount++;
    return (a + b + c) / 3.0f;
}

LB24_API void Fun33(int in, int* out) {
    g_nFnCallsCount++;
    *out = in * 2;
}

LB24_API int* GetDllCallsCount() {
    return &g_nDllCallsCount;
}

LB24_API int* GetFnCallsCount() {
    return &g_nFnCallsCount;
}