#pragma once
// Lb24_Goncharov.h
// «аголовочный файл динамической библиотеки
// —одержит объ€влени€ экспортируемых функций и переменных

// ћакрос дл€ управлени€ импортом/экспортом
#ifdef LB24_GONCHAROV_EXPORTS
#define LB24_API __declspec(dllexport) // Ёкспорт при компил€ции DLL
#else
#define LB24_API __declspec(dllimport) // »мпорт при компил€ции клиента
#endif

// ќбъ€влени€ дл€ экспортируемых глобальных переменных
// extern указывает, что переменные определены в другом файле
extern LB24_API int g_nDllCallsCount; // —четчик загрузок библиотеки (раздел€емый)
extern LB24_API int g_nFnCallsCount;   // —четчик вызовов функций (в рамках процесса)

// ќбъ€влени€ дл€ экспортируемых функций
// extern "C" предотвращает декорирование имен C++ компил€тором
extern "C" LB24_API int WINAPI  Fun31(double a, double b);
extern "C" LB24_API float Fun32(int a, int b, int c);
extern "C" LB24_API void Fun33(int in, int* out);