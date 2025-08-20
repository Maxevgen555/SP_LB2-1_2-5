#pragma once

#include <windows.h>

// Идентификаторы элементов управления
#define IDC_BUTTON_CLICK 1001  // Кнопка для нажатий
#define IDC_EDIT_INPUT   1002  // Поле ввода текста
#define IDC_STATIC_TEXT  1003  // Статический текст для отображения информации

// Прототипы функций оконной процедуры
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Функция регистрации класса окна
BOOL RegisterWindowClass(HINSTANCE hInstance);

// Функция создания главного окна с параметрами позиции и размера
HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, int x, int y, int width, int height);

// Обработчики сообщений
void OnCreate(HWND hwnd);                          // Обработка создания окна
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify); // Обработка команд
void OnPaint(HWND hwnd);                           // Обработка перерисовки окна
void OnDestroy(HWND hwnd);                         // Обработка закрытия окна
