#pragma once
// TextProcessor.h
// ������������ ���� ��� ���������� ��������� ������ (������� 3)

#ifdef TEXTPROC_EXPORTS
#define TP_API __declspec(dllexport)
#else
#define TP_API __declspec(dllimport)
#endif

// extern "C" ������� ������������� ����
extern "C" TP_API void ProcessFileAndAppendWordCount(const char* filename);