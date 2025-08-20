#pragma once

// Подключение необходимых заголовочных файлов Windows API
#include <windows.h>      // Основные функции Windows API
#include <tchar.h>        // Поддержка Unicode и ANSI символов
#include <commctrl.h>     // Элементы управления общего диалога
#include <tlhelp32.h>     // Функции для работы с процессами и потоками

// Указание точки входа для Windows приложения (вместо стандартной main)
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")

// Стандартный идентификатор для статических элементов управления
#ifndef IDC_STATIC
#define IDC_STATIC                      -1
#endif

// ================= КОНСТАНТЫ МЕНЮ =================
// Команды меню "Процессы"
#define IDM_PROCESS_NOTEPAD 1001        // Запуск Блокнота
#define IDM_PROCESS_NOTEPAD_TEXT 1002   // Запуск Блокнота с текстовым файлом
#define IDM_PROCESS_CALC 1003           // Запуск Калькулятора
#define IDM_PROCESS_CLOSE_NOTEPAD 1004  // Закрытие всех окон Блокнота

// Команды меню "Информация о процессах"
#define IDM_INFO_PROCESS0 2001          // Информация о текущем процессе (0)
#define IDM_INFO_PROCESS1 2002          // Информация о процессе Блокнота (1)
#define IDM_INFO_PROCESS3 2003          // Информация о процессе Калькулятора (3)
#define IDM_INFO_PROCESS2 2004          // Информация о процессе Блокнота с текстом (2)

// ================= ДИАЛОГОВЫЕ ОКНА =================
#define IDD_PROCESS_INFO 3001           // ID диалогового окна с информацией о процессе

// ================= ЭЛЕМЕНТЫ ДИАЛОГОВОГО ОКНА =================
// Поля для отображения информации о процессе
#define IDC_EDIT_FILENAME 4001          // Поле: Имя исполняемого файла
#define IDC_EDIT_PARAMS 4002            // Поле: Параметры командной строки
#define IDC_EDIT_PROC_HANDLE 4003       // Поле: Дескриптор процесса
#define IDC_EDIT_PROC_ID 4004           // Поле: Идентификатор процесса (PID)
#define IDC_EDIT_THREAD_HANDLE 4005     // Поле: Дескриптор основного потока
#define IDC_EDIT_THREAD_ID 4006         // Поле: Идентификатор основного потока (TID)
#define IDC_EDIT_PROC_EXIT_CODE 4007    // Поле: Код завершения процесса
#define IDC_EDIT_THREAD_EXIT_CODE 4008  // Поле: Код завершения потока
#define IDC_EDIT_PRIORITY_CLASS 4009    // Поле: Класс приоритета процесса
#define IDC_EDIT_LIFE_TIME 4010         // Поле: Время жизни процесса (мс)
#define IDC_EDIT_USER_TIME 4011         // Поле: Время в режиме пользователя (мс)
#define IDC_EDIT_KERNEL_TIME 4012       // Поле: Время в режиме ядра (мс)
#define IDC_EDIT_IDLE_TIME 4013         // Поле: Время простоя (мс)

// ================= ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =================
// Эти переменные доступны во всех модулях программы

extern HINSTANCE hInst;                 // Дескриптор экземпляра приложения
extern TCHAR szClassName[];             // Имя класса главного окна
extern TCHAR szTitle[];                 // Заголовок главного окна

// ================= МАССИВЫ ДЛЯ ХРАНЕНИЯ ИНФОРМАЦИИ О ПРОЦЕССАХ =================
// Индексы массивов:
// 0 - текущий процесс (само приложение)
// 1 - процесс Блокнота
// 2 - процесс Блокнота с текстовым файлом  
// 3 - процесс Калькулятора

extern HANDLE ProcHandle[4];            // Массив дескрипторов процессов
extern DWORD ProcId[4];                 // Массив идентификаторов процессов (PID)
extern HANDLE ThreadHandle[4];          // Массив дескрипторов основных потоков
extern DWORD ThreadId[4];               // Массив идентификаторов потоков (TID)
extern LPTSTR ProcImage[4];             // Массив путей к исполняемым файлам
extern TCHAR CmdParam[4][260];          // Массив параметров командной строки

// ================= ПРОТОТИПЫ ФУНКЦИЙ =================
// Объявления функций для компилятора

// Основная процедура окна - обрабатывает все сообщения главного окна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Инициализация приложения - регистрация класса окна
BOOL InitApplication(HINSTANCE);

// Создание экземпляра приложения - создание главного окна
BOOL InitInstance(HINSTANCE, int);

// Инициализация данных о процессах - заполнение массивов начальными значениями
void InitializeProcessData();

// Запуск процесса по указанному индексу
BOOL StartProcess(int index);

// Поиск и закрытие всех окон Блокнота
BOOL FindAndCloseNotepad();

// Процедура диалогового окна с информацией о процессе
INT_PTR CALLBACK ProcessInfoDlgProc(HWND, UINT, WPARAM, LPARAM);

// Заполнение диалогового окна информацией о процессе
void FillProcessInfo(HWND hDlg, int index);