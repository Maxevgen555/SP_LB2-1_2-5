// ClientLB24Exp.cpp
#include <windows.h>
#include <stdio.h>

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    printf("Клиент с явной компоновкой (Explicit Linking):\n");
    printf("ШАГ 1: Загрузка DLL...\n");

    HINSTANCE hDll = LoadLibraryA("Lb24_Goncharov.dll");
    if (!hDll) {
        printf("ОШИБКА: Не удалось загрузить DLL! Код: %lu\n", GetLastError());
        system("pause");
        return 1;
    }
    printf("✓ DLL успешно загружена\n");

    // Получаем только САМЫЕ ПРОСТЫЕ функции для теста
    FARPROC pGetDllCallsCount = GetProcAddress(hDll, "GetDllCallsCount");
    FARPROC pGetFnCallsCount = GetProcAddress(hDll, "GetFnCallsCount");

    if (!pGetDllCallsCount || !pGetFnCallsCount) {
        printf("ОШИБКА: Не найдены функции доступа!\n");
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    // Безопасное получение значений переменных
    typedef int* (__cdecl* GetCountType)();
    GetCountType GetDllCallsCount = (GetCountType)pGetDllCallsCount;
    GetCountType GetFnCallsCount = (GetCountType)pGetFnCallsCount;

    printf("ШАГ 2: Чтение глобальных переменных...\n");

    // ОЧЕНЬ осторожно получаем значения
    int* pDllCount = NULL;
    int* pFnCount = NULL;

    __try {
        pDllCount = GetDllCallsCount();
        pFnCount = GetFnCallsCount();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("ОШИБКА: Исключение при получении переменных!\n");
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    if (pDllCount && pFnCount) {
        printf("g_nDllCallsCount: %d\n", *pDllCount);
        printf("g_nFnCallsCount: %d\n", *pFnCount);
    }

    // ТЕПЕРЬ пробуем вызвать простые функции
    printf("ШАГ 3: Тестовый вызов простых функций...\n");

    // 1. Самая простая функция - GetDllCallsCount
    __try {
        int* result = GetDllCallsCount();
        printf("✓ GetDllCallsCount вызвана успешно\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("✗ ОШИБКА в GetDllCallsCount!\n");
    }

    // 2. Пробуем Fun33 (самая простая из основных)
    FARPROC pFun33 = GetProcAddress(hDll, "Fun33");
    if (pFun33) {
        typedef void(__cdecl* Fun33Type)(int, int*);
        Fun33Type Fun33 = (Fun33Type)pFun33;

        __try {
            int outputVal;
            Fun33(15, &outputVal);
            printf("✓ Fun33 результат: %d\n", outputVal);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            printf("✗ ОШИБКА в Fun33!\n");
        }
    }

    // 3. Пробуем Fun32
    FARPROC pFun32 = GetProcAddress(hDll, "Fun32");
    if (pFun32) {
        typedef float(__cdecl* Fun32Type)(int, int, int);
        Fun32Type Fun32 = (Fun32Type)pFun32;

        __try {
            float result2 = Fun32(10, 20, 30);
            printf("✓ Fun32 результат: %.2f\n", result2);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            printf("✗ ОШИБКА в Fun32!\n");
        }
    }

    // 4. И только потом Fun31 (самая проблемная)
    FARPROC pFun31 = GetProcAddress(hDll, "Fun31");
    if (pFun31) {
        typedef int(__cdecl* Fun31Type)(double, double);
        Fun31Type Fun31 = (Fun31Type)pFun31;

        __try {
            int result1 = Fun31(5.5, 3.2);
            printf("✓ Fun31 результат: %d\n", result1);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            printf("✗ ОШИБКА в Fun31!\n");
        }
    }

    // Финальные значения
    if (pDllCount && pFnCount) {
        printf("Финальные значения:\n");
        printf("g_nDllCallsCount: %d\n", *pDllCount);
        printf("g_nFnCallsCount: %d\n", *pFnCount);
    }

    FreeLibrary(hDll);
    printf("✓ Библиотека выгружена\n");
    system("pause");
    return 0;
}