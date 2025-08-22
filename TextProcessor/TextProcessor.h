#pragma once
// TextProcessor.h
// Заголовочный файл для библиотеки обработки текста (Вариант 3)

#ifdef TEXTPROC_EXPORTS
#define TP_API __declspec(dllexport)
#else
#define TP_API __declspec(dllimport)
#endif

// extern "C" убирает декорирование имен
extern "C" TP_API void ProcessFileAndAppendWordCount(const char* filename);