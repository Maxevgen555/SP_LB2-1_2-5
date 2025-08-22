#pragma once

#ifdef LB24_GONCHAROV_EXPORTS
#define LB24_API __declspec(dllexport)
#else
#define LB24_API __declspec(dllimport)
#endif

// Объявления для экспортируемых глобальных переменных
extern LB24_API int g_nDllCallsCount;
extern LB24_API int g_nFnCallsCount;

// Объявления для экспортируемых функций (ВСЕ в __cdecl)
#ifdef __cplusplus
extern "C" {
#endif

	LB24_API int Fun31(double a, double b);
	LB24_API float Fun32(int a, int b, int c);
	LB24_API void Fun33(int in, int* out);
	LB24_API int* GetDllCallsCount();
	LB24_API int* GetFnCallsCount();

#ifdef __cplusplus
}
#endif