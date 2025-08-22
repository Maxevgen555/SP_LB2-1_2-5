// Lb24_Goncharov.cpp
// Главный файл динамической библиотеки
// Содержит точку входа DllMain и реализацию экспортируемых функций

// Определяем макрос для экспорта функций и переменных
#define LB24_GONCHAROV_EXPORTS

// Подключаем необходимые заголовки
#include "Lb24_Goncharov.h" // Наш заголовочный файл
#include <windows.h>        // Для WinAPI функций и типов
#include <iostream>         // Для вывода сообщений

// Создаем разделяемую секцию данных для g_nDllCallsCount
// #pragma directive указывает компилятору поместить следующую переменную в специальную секцию
#pragma data_seg(".SHARED")          // Начинаем секцию с именем ".SHARED"
LB24_API int g_nDllCallsCount = 0;    // Инициализированная переменная в разделяемой секции
#pragma data_seg()                   // Возвращаемся к стандартной секции данных

// Указываем компоновщику сделать секцию ".SHARED" доступной для чтения, записи и разделения
// R - Read, W - Write, S - Shared
#pragma comment(linker, "/SECTION:.SHARED,RWS")

// Инициализация НЕразделяемой переменной (обычная глобальная переменная)
LB24_API int g_nFnCallsCount = 0;

// Точка входа DLL. Вызывается системой при событиях загрузки/выгрузки и создания/завершения потоков.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    // switch по причине вызова
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Библиотека подгружена в процесс
        g_nDllCallsCount++; // Увеличиваем СОВМЕСТНЫЙ счетчик загрузок
        std::cout << "Загружается библиотека Lb24_Goncharov.dll. Проект Goncharov" << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        // Библиотека выгружается из процесса
        std::cout << "Выгружается библиотека Lb24_Goncharov.dll." << std::endl;
        break;
    case DLL_THREAD_ATTACH:
        // В процессе, владеющем библиотекой, создан новый поток
        std::cout << "Создан новый поток." << std::endl;
        break;
    case DLL_THREAD_DETACH:
        // В процессе, владеющем библиотекой, завершается поток
        std::cout << "Завершается поток." << std::endl;
        break;
    }
    return TRUE; // Возвращаем TRUE при успешной инициализации
}

// Реализация функции Fun31
// WINAPI указывает на соглашение о вызове __stdcall
LB24_API int WINAPI Fun31(double a, double b) {
    g_nFnCallsCount++; // Увеличиваем счетчик вызовов функций В ЭТОМ ПРОЦЕССЕ
    // Простая демонстрационная операция - сумма двух чисел с приведением к int
    return (int)(a + b);
}

// Реализация функции Fun32
LB24_API float Fun32(int a, int b, int c) {
    g_nFnCallsCount++;
    // Вычисляем среднее арифметическое трех целых чисел
    return (a + b + c) / 3.0f; // .0f для гарантии вычисления с плавающей точкой
}

// Реализация функции Fun33
// Принимает входное значение in и возвращает результат через указатель out
LB24_API void Fun33(int in, int* out) {
    g_nFnCallsCount++;
    // Записываем по переданному адресу удвоенное значение входного параметра
    *out = in * 2;
}