// Отключение предупреждений безопасности CRT функций
#define _CRT_SECURE_NO_WARNINGS

// Исключение редко используемых компонентов Windows для уменьшения размера кода
#define WIN32_LEAN_AND_MEAN

// Подключение необходимых заголовочных файлов
#include <windows.h>        // Основной заголовочный файл Windows API
#include <tchar.h>          // Поддержка Unicode и ANSI символов
#include <stdio.h>          // Стандартные функции ввода-вывода
#include <time.h>           // Функции работы со временем
#include <stdlib.h>         // Стандартные функции (rand, srand)
#include "resource.h"       // Заголовочный файл с ресурсами приложения

// Указание линкеру создать Windows приложение (не консольное)
#pragma comment(linker, "/subsystem:windows")

// ==================== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ====================

HINSTANCE hInst;                            // Дескриптор экземпляра приложения
TCHAR szClassName[] = _T("ThreadAppClass"); // Имя класса главного окна
TCHAR szTitle[100] = _T("Гончаров_thread_lb"); // Заголовок окна приложения

// Переменные для ждущего таймера
HANDLE hWaitableTimer = NULL;              // Дескриптор ждущего таймера
int g_timerCounter = 0;                    // Счетчик срабатываний таймера
BOOL g_timerEnabled = FALSE;               // Флаг активности таймера

// Прототипы функций для таймера
void CreateWaitableTimer();                // Создание ждущего таймера
void StartWaitableTimer();                 // Запуск таймера
void StopWaitableTimer();                  // Остановка таймера
DWORD WINAPI TimerThreadProc(LPVOID lpParam); // Функция потока таймера
void ShowTimerMessageBox();                // Показать MessageBox таймера

// Переменные для потока анимации (движущийся шарик)
HANDLE hAnimationThread = NULL;             // Дескриптор потока анимации
DWORD dwAnimationThreadId = 0;              // Идентификатор потока анимации
int animationState = 0;                     // Состояние потока: 0-не создан, 1-активен, 2-приостановлен
int animationPriority = 0;                  // Приоритет потока анимации

// Параметры анимации шарика
int ballX = 100;                            // Начальная позиция X шарика
int ballY = 100;                            // Начальная позиция Y шарика
int ballSize = 30;                          // Размер шарика в пикселях
int ballSpeedX = 3;                         // Скорость движения по оси X
int ballSpeedY = 2;                         // Скорость движения по оси Y
COLORREF ballColor = RGB(255, 0, 0);        // Начальный цвет шарика (красный)

// Переменные для управления потоками с бегущими строками
HANDLE hSecThread[3] = { NULL };            // Массив дескрипторов потоков: [0]-первичный, [1]-поток1, [2]-поток2
DWORD dwSecThreadId[3] = { 0 };             // Массив идентификаторов потоков
int g_uThCount = 0;                         // Счетчик созданных вторичных потоков
int threadStates[3] = { 0 };                // Состояния потоков: 0-не создан, 1-активен, 2-приостановлен
int threadPriorities[3] = { 0 };            // Приоритеты потоков

// Позиции для отображения бегущих строк
int thread1Pos = 0;                         // Текущая позиция X для строки потока 1
int thread2Pos = 0;                         // Текущая позиция X для строки потока 2

// ==================== ПЕРЕМЕННЫЕ ДЛЯ СИНХРОНИЗАЦИИ ====================
CRITICAL_SECTION g_cs;                      // Критическая секция для синхронизации потоков
BOOL g_synchronizationEnabled = FALSE;      // Флаг включения/выключения синхронизации
int g_stepCounter[3] = { 0 };               // Счетчики шагов для каждого потока (для статистики)
TCHAR g_thread1Text[] = _T("Вторичный поток 1 создал Гончаров"); // Текст для потока 1
TCHAR g_thread2Text[] = _T("Вторичный поток 2 создал Гончаров"); // Текст для потока 2

// Таймер для автоматического обновления информации на экране
UINT_PTR g_updateTimer = 0;                 // Идентификатор таймера обновления

// ==================== ПРОТОТИПЫ ФУНКЦИЙ ====================

// Функции инициализации приложения
BOOL InitApplication(HINSTANCE hInstance);   // Регистрация класса окна
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow); // Создание главного окна

// Обработчик сообщений главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Обработчики диалоговых окон
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); // Диалог "О программе"
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); // Диалог информации о потоке
void UpdateThreadInfo(HWND hDlg, int threadIndex); // Обновление информации в диалоге

// Функции для потока анимации (шарик)
DWORD WINAPI AnimationThread(LPVOID lpParam); // Функция потока анимации
void CreateAnimationThread(BOOL suspended);   // Создание потока анимации
void SuspendAnimationThread();                // Приостановка потока анимации
void ResumeAnimationThread();                 // Возобновление потока анимации
void TerminateAnimationThread();              // Завершение потока анимации
void IncreaseAnimationPriority();             // Увеличение приоритета анимации
void DecreaseAnimationPriority();             // Уменьшение приоритета анимации

// Функции потоков (бегущие строки)
DWORD WINAPI ThreadFunc1(LPVOID lpParam);    // Функция потока 1
DWORD WINAPI ThreadFunc2(LPVOID lpParam);    // Функция потока 2

// Функции управления потоками
void CreateThread1(BOOL suspended);          // Создание потока 1
void CreateThread2(BOOL suspended);          // Создание потока 2
void SuspendThread1();                       // Приостановка потока 1
void ResumeThread1();                        // Возобновление потока 1
void TerminateThread1();                     // Завершение потока 1
void IncreasePriority1();                    // Увеличение приоритета потока 1
void DecreasePriority1();                    // Уменьшение приоритета потока 1
void SuspendThread2();                       // Приостановка потока 2
void ResumeThread2();                        // Возобновление потока 2
void TerminateThread2();                     // Завершение потока 2
void IncreasePriority2();                    // Увеличение приоритета потока 2
void DecreasePriority2();                    // Уменьшение приоритета потока 2

// Функции синхронизации
void EnableSynchronization();                // Включение синхронизации
void DisableSynchronization();               // Выключение синхронизации
void DrawRunningString(HDC hdc, HWND hWnd, int threadIndex, TCHAR* text, int* position, int yPos); // Отрисовка строки с синхронизацией

// ==================== ГЛАВНАЯ ФУНКЦИЯ ПРИЛОЖЕНИЯ ====================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;  // Структура для хранения сообщений Windows

    // Инициализация критической секции для синхронизации потоков
    InitializeCriticalSection(&g_cs);

    // Регистрация класса окна приложения
    if (!InitApplication(hInstance))
        return FALSE;  // Выход при ошибке регистрации

    // Создание и отображение главного окна
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;  // Выход при ошибке создания окна

    // Основной цикл обработки сообщений приложения
    while (GetMessage(&msg, NULL, 0, 0))  // Получение сообщения из очереди
    {
        TranslateMessage(&msg);  // Преобразование сообщений клавиш (например, WM_KEYDOWN -> WM_CHAR)
        DispatchMessage(&msg);   // Передача сообщения соответствующей оконной процедуре
    }

    // Удаление критической секции при выходе из приложения
    DeleteCriticalSection(&g_cs);

    return (int)msg.wParam;  // Возврат кода завершения приложения
}

// ==================== РЕГИСТРАЦИЯ КЛАССА ОКНА ====================
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;  // Структура для описания класса окна

    // Заполнение структуры параметрами класса окна
    wcex.cbSize = sizeof(WNDCLASSEX);           // Размер структуры (обязательно)
    wcex.style = CS_HREDRAW | CS_VREDRAW;       // Стиль: перерисовка при изменении размера
    wcex.lpfnWndProc = WndProc;                 // Указатель на функцию обработки сообщений
    wcex.cbClsExtra = 0;                        // Дополнительная память для класса
    wcex.cbWndExtra = 0;                        // Дополнительная память для каждого окна
    wcex.hInstance = hInstance;                 // Дескриптор экземпляра приложения
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Стандартная иконка приложения
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   // Стандартный курсор (стрелка)
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Цвет фона (стандартный цвет окна)
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU); // Идентификатор меню из ресурсов
    wcex.lpszClassName = szClassName;           // Имя класса окна
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Маленькая иконка

    // Регистрация класса окна в системе Windows
    return RegisterClassEx(&wcex);
}

// ==================== СОЗДАНИЕ И ОТОБРАЖЕНИЕ ГЛАВНОГО ОКНА ====================
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;  // Дескриптор создаваемого окна

    hInst = hInstance;  // Сохранение дескриптора экземпляра в глобальной переменной

    // Создание главного окна приложения
    hWnd = CreateWindow(
        szClassName,           // Имя класса окна
        szTitle,               // Заголовок окна
        WS_OVERLAPPEDWINDOW,   // Стиль окна (стандартное перекрывающееся окно)
        CW_USEDEFAULT,         // Позиция X по умолчанию
        CW_USEDEFAULT,         // Позиция Y по умолчанию
        800,                   // Ширина окна в пикселях
        600,                   // Высота окна в пикселях
        NULL,                  // Родительское окно (нет)
        NULL,                  // Меню (уже указано в классе окна)
        hInstance,             // Дескриптор экземпляра приложения
        NULL                   // Дополнительные параметры создания
    );

    // Проверка успешности создания окна
    if (!hWnd)
        return FALSE;

    // СОЗДАНИЕ ТАЙМЕРА ДЛЯ АВТОМАТИЧЕСКОГО ОБНОВЛЕНИЯ ИНФОРМАЦИИ НА ЭКРАНЕ
    // Таймер с ID=1 будет срабатывать каждые 100 мс (10 раз в секунду)
    g_updateTimer = SetTimer(hWnd, 1, 100, NULL);

    // Отображение и обновление окна
    ShowWindow(hWnd, nCmdShow);  // Показ окна
    UpdateWindow(hWnd);          // Принудительная перерисовка окна

    return TRUE;  // Успешное создание окна
}

// ==================== ФУНКЦИЯ ОТРИСОВКИ БЕГУЩЕЙ СТРОКИ С СИНХРОНИЗАЦИЕЙ ====================
void DrawRunningString(HDC hdc, HWND hWnd, int threadIndex, TCHAR* text, int* position, int yPos)
{
    RECT rect;
    GetClientRect(hWnd, &rect);  // Получение размеров клиентской области окна

    // Проверка включена ли синхронизация
    if (g_synchronizationEnabled)
    {
        // ВХОД В КРИТИЧЕСКУЮ СЕКЦИЮ - другие потоки будут ждать
        EnterCriticalSection(&g_cs);

        // Визуальный индикатор блокировки (красный квадратик слева от текста)
        HBRUSH hLockBrush = CreateSolidBrush(RGB(255, 200, 200)); // Светло-красная кисть
        RECT lockRect = { 10, yPos - 5, 15, yPos + 15 };           // Прямоугольник для индикатора
        FillRect(hdc, &lockRect, hLockBrush);                    // Заливка прямоугольника
        DeleteObject(hLockBrush);                               // Удаление кисти

        // ВАРИАНТ К16: выполнение N = 4 * длина строки шагов атомарно (без прерывания)
        int nSteps = 4 * _tcslen(text);  // Расчет количества шагов: N = 4 * длина строки
        for (int i = 0; i < nSteps; i++) // Цикл выполнения N шагов
        {
            // Очистка предыдущей позиции текста (затирание старого текста)
            SetBkColor(hdc, GetSysColor(COLOR_WINDOW));     // Установка цвета фона = цвет окна
            SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // Установка цвета текста = стандартный
            TextOut(hdc, *position - 1, yPos, _T(""), 0);    // Вывод пустой строки для затирания

            // Вывод текста с различными цветами для разных потоков
            if (threadIndex == 1)  // Поток 1
            {
                SetBkColor(hdc, RGB(220, 255, 220));  // Светло-зеленый фон
                SetTextColor(hdc, RGB(0, 100, 0));    // Темно-зеленый текст
            }
            else  // Поток 2
            {
                SetBkColor(hdc, RGB(255, 220, 220));  // Светло-красный фон
                SetTextColor(hdc, RGB(100, 0, 0));    // Темно-красный текст
            }

            // Вывод текста на текущей позиции
            TextOut(hdc, *position, yPos, text, _tcslen(text));

            // Обновление позиции и проверка границ окна
            (*position) += 2;  // Медленное перемещение для наглядности синхронизации
            if (*position > rect.right)  // Если текст ушел за правую границу
                *position = -300;        // Возврат в начало (с запасом для плавности)

            g_stepCounter[threadIndex]++;  // Увеличение счетчика шагов потока

            Sleep(10);  // Небольшая задержка для визуализации процесса
        }

        // ВЫХОД ИЗ КРИТИЧЕСКОЙ СЕКЦИИ - разрешение другим потокам работать
        LeaveCriticalSection(&g_cs);
    }
    else
    {
        // РЕЖИМ БЕЗ СИНХРОНИЗАЦИИ - потоки работают параллельно
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));     // Цвет фона = цвет окна
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // Цвет текста = стандартный
        TextOut(hdc, *position - 1, yPos, _T(""), 0);    // Затирание старого текста

        // Разные цветовые схемы для разных потоков
        if (threadIndex == 1)  // Поток 1
        {
            SetBkColor(hdc, RGB(255, 255, 200));  // Светло-желтый фон
            SetTextColor(hdc, RGB(0, 0, 150));    // Синий текст
        }
        else  // Поток 2
        {
            SetBkColor(hdc, RGB(255, 200, 255));  // Светло-фиолетовый фон
            SetTextColor(hdc, RGB(150, 0, 0));    // Красный текст
        }

        // Вывод текста и обновление позиции
        TextOut(hdc, *position, yPos, text, _tcslen(text));
        (*position) += 5;  // Быстрое перемещение без синхронизации
        if (*position > rect.right)  // Проверка правой границы
            *position = -300;        // Возврат в начало

        g_stepCounter[threadIndex]++;  // Увеличение счетчика шагов
    }
}

// ==================== ФУНКЦИЯ ПОТОКА 1 (БЕГУЩАЯ СТРОКА) ====================
DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;  // Получение дескриптора главного окна из параметра
    HDC hdc;                    // Контекст устройства для рисования

    // Бесконечный цикл потока (завершается только принудительно)
    while (TRUE)
    {
        // Проверка состояния потока - работаем только если поток активен
        if (threadStates[1] == 1)
        {
            hdc = GetDC(hWnd);  // Получение контекста устройства для рисования
            if (hdc)
            {
                // Вызов функции отрисовки с поддержкой синхронизации
                DrawRunningString(hdc, hWnd, 1, g_thread1Text, &thread1Pos, 50);
                ReleaseDC(hWnd, hdc);  // Освобождение контекста устройства
            }
        }
        Sleep(100);  // Задержка для контроля скорости движения текста
    }
    return 0;  // Код завершения (никогда не выполняется из-за бесконечного цикла)
}

// ==================== ФУНКЦИЯ ПОТОКА 2 (БЕГУЩАЯ СТРОКА) ====================
DWORD WINAPI ThreadFunc2(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;  // Дескриптор главного окна
    HDC hdc;                    // Контекст устройства

    while (TRUE)
    {
        if (threadStates[2] == 1)  // Проверка активности потока
        {
            hdc = GetDC(hWnd);
            if (hdc)
            {
                // Отрисовка строки для потока 2
                DrawRunningString(hdc, hWnd, 2, g_thread2Text, &thread2Pos, 100);
                ReleaseDC(hWnd, hdc);
            }
        }
        Sleep(100);  // Задержка
    }
    return 0;
}

// ==================== ФУНКЦИЯ ПОТОКА АНИМАЦИИ (ДВИЖУЩИЙСЯ ШАРИК) ====================
DWORD WINAPI AnimationThread(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;  // Дескриптор главного окна
    HDC hdc;                    // Контекст устройства
    RECT rect;                  // Прямоугольник для хранения размеров окна

    while (TRUE)
    {
        if (animationState == 1)  // Проверка активности потока анимации
        {
            hdc = GetDC(hWnd);  // Получение контекста устройства
            if (hdc)
            {
                GetClientRect(hWnd, &rect);  // Получение размеров клиентской области

                // Стирание предыдущего положения шарика
                HBRUSH hWhiteBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // Кисть цвета окна
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hWhiteBrush);        // Сохранение старой кисти
                HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN)); // Установка прозрачного пера
                // Рисование белого круга для затирания старого шарика
                Ellipse(hdc, ballX - 1, ballY - 1, ballX + ballSize + 1, ballY + ballSize + 1);

                // Обновление позиции шарика
                ballX += ballSpeedX;
                ballY += ballSpeedY;

                // Проверка столкновения с границами окна
                if (ballX <= 0 || ballX + ballSize >= rect.right)  // Столкновение с левой/правой границей
                {
                    ballSpeedX = -ballSpeedX; // Изменение направления по X
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // Случайный цвет
                }
                if (ballY <= 0 || ballY + ballSize >= rect.bottom) // Столкновение с верхней/нижней границей
                {
                    ballSpeedY = -ballSpeedY; // Изменение направления по Y
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // Случайный цвет
                }

                // Рисование шарика новым цветом
                HBRUSH hBallBrush = CreateSolidBrush(ballColor); // Кисть цвета шарика
                SelectObject(hdc, hBallBrush);                   // Выбор кисти для рисования
                Ellipse(hdc, ballX, ballY, ballX + ballSize, ballY + ballSize); // Рисование шарика

                // Восстановление исходных объектов и удаление созданных
                SelectObject(hdc, hOldBrush);    // Восстановление старой кисти
                SelectObject(hdc, hOldPen);      // Восстановление старого пера
                DeleteObject(hWhiteBrush);       // Удаление кисти затирания
                DeleteObject(hBallBrush);        // Удаление кисти шарика

                ReleaseDC(hWnd, hdc);  // Освобождение контекста устройства
            }
        }
        Sleep(30); // Задержка для плавности анимации (~33 FPS)
    }
    return 0;
}

// ==================== ФУНКЦИИ УПРАВЛЕНИЯ ПОТОКОМ 1 ====================

// Создание потока 1
void CreateThread1(BOOL suspended)
{
    // Проверка: если поток уже создан, показываем сообщение об ошибке
    if (hSecThread[1] != NULL)
    {
        MessageBox(NULL, _T("Поток 1 уже создан!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    // Получение дескриптора главного окна
    HWND hWnd = FindWindow(szClassName, NULL);
    // Создание потока с возможностью приостановки
    hSecThread[1] = CreateThread(
        NULL,                   // Атрибуты безопасности по умолчанию
        0,                      // Размер стека по умолчанию
        ThreadFunc1,            // Функция потока
        hWnd,                   // Параметр, передаваемый в функцию потока
        suspended ? CREATE_SUSPENDED : 0, // Флаги создания (с приостановкой или без)
        &dwSecThreadId[1]       // Получение ID потока
    );

    // Проверка успешности создания потока
    if (hSecThread[1])
    {
        threadStates[1] = suspended ? 2 : 1;  // Установка состояния: 2-приостановлен, 1-активен
        threadPriorities[1] = THREAD_PRIORITY_NORMAL;  // Установка нормального приоритета
        SetThreadPriority(hSecThread[1], threadPriorities[1]);  // Применение приоритета
        g_uThCount++;  // Увеличение счетчика потоков
    }
}

// Приостановка потока 1
void SuspendThread1()
{
    // Проверка: если поток существует и активен
    if (hSecThread[1] && threadStates[1] == 1)
    {
        SuspendThread(hSecThread[1]);  // Приостановка потока
        threadStates[1] = 2;           // Установка состояния "приостановлен"
    }
}

// Возобновление потока 1
void ResumeThread1()
{
    // Проверка: если поток существует и приостановлен
    if (hSecThread[1] && threadStates[1] == 2)
    {
        ResumeThread(hSecThread[1]);  // Возобновление потока
        threadStates[1] = 1;          // Установка состояния "активен"
    }
}

// Завершение потока 1
void TerminateThread1()
{
    if (hSecThread[1])
    {
        TerminateThread(hSecThread[1], 0);  // Принудительное завершение потока
        CloseHandle(hSecThread[1]);         // Закрытие дескриптора потока
        hSecThread[1] = NULL;               // Обнуление дескриптора
        dwSecThreadId[1] = 0;               // Обнуление ID потока
        threadStates[1] = 0;                // Установка состояния "не создан"
        g_uThCount--;                       // Уменьшение счетчика потоков

        // Очистка области вывода (перерисовка окна)
        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);  // Отправка сообщения WM_PAINT
    }
}

// Увеличение приоритета потока 1
void IncreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);  // Текущий приоритет
        // Проверка возможности увеличения (не выше HIGHEST)
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority + 1);  // Увеличение приоритета
            threadPriorities[1] = currentPriority + 1;              // Сохранение нового значения
        }
    }
}

// Уменьшение приоритета потока 1
void DecreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);  // Текущий приоритет
        // Проверка возможности уменьшения (не ниже LOWEST)
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority - 1);  // Уменьшение приоритета
            threadPriorities[1] = currentPriority - 1;              // Сохранение нового значения
        }
    }
}

// ==================== ФУНКЦИИ УПРАВЛЕНИЯ ПОТОКОМ 2 (аналогичны потоку 1) ====================

void CreateThread2(BOOL suspended)
{
    if (hSecThread[2] != NULL)
    {
        MessageBox(NULL, _T("Поток 2 уже создан!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    HWND hWnd = FindWindow(szClassName, NULL);
    hSecThread[2] = CreateThread(NULL, 0, ThreadFunc2, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwSecThreadId[2]);

    if (hSecThread[2])
    {
        threadStates[2] = suspended ? 2 : 1;
        threadPriorities[2] = THREAD_PRIORITY_NORMAL;
        SetThreadPriority(hSecThread[2], threadPriorities[2]);
        g_uThCount++;
    }
}

void SuspendThread2()
{
    if (hSecThread[2] && threadStates[2] == 1)
    {
        SuspendThread(hSecThread[2]);
        threadStates[2] = 2;
    }
}

void ResumeThread2()
{
    if (hSecThread[2] && threadStates[2] == 2)
    {
        ResumeThread(hSecThread[2]);
        threadStates[2] = 1;
    }
}

void TerminateThread2()
{
    if (hSecThread[2])
    {
        TerminateThread(hSecThread[2], 0);
        CloseHandle(hSecThread[2]);
        hSecThread[2] = NULL;
        dwSecThreadId[2] = 0;
        threadStates[2] = 0;
        g_uThCount--;

        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void IncreasePriority2()
{
    if (hSecThread[2])
    {
        int currentPriority = GetThreadPriority(hSecThread[2]);
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hSecThread[2], currentPriority + 1);
            threadPriorities[2] = currentPriority + 1;
        }
    }
}

void DecreasePriority2()
{
    if (hSecThread[2])
    {
        int currentPriority = GetThreadPriority(hSecThread[2]);
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hSecThread[2], currentPriority - 1);
            threadPriorities[2] = currentPriority - 1;
        }
    }
}

// ==================== ФУНКЦИИ УПРАВЛЕНИЯ ПОТОКОМ АНИМАЦИИ ====================

void CreateAnimationThread(BOOL suspended)
{
    if (hAnimationThread != NULL)
    {
        MessageBox(NULL, _T("Поток анимации уже создан!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    HWND hWnd = FindWindow(szClassName, NULL);
    hAnimationThread = CreateThread(NULL, 0, AnimationThread, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwAnimationThreadId);

    if (hAnimationThread)
    {
        animationState = suspended ? 2 : 1;
        animationPriority = THREAD_PRIORITY_NORMAL;
        SetThreadPriority(hAnimationThread, animationPriority);
        g_uThCount++;
    }
}

void SuspendAnimationThread()
{
    if (hAnimationThread && animationState == 1)
    {
        SuspendThread(hAnimationThread);
        animationState = 2;
    }
}

void ResumeAnimationThread()
{
    if (hAnimationThread && animationState == 2)
    {
        ResumeThread(hAnimationThread);
        animationState = 1;
    }
}

void TerminateAnimationThread()
{
    if (hAnimationThread)
    {
        TerminateThread(hAnimationThread, 0);
        CloseHandle(hAnimationThread);
        hAnimationThread = NULL;
        dwAnimationThreadId = 0;
        animationState = 0;
        g_uThCount--;

        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

void IncreaseAnimationPriority()
{
    if (hAnimationThread)
    {
        int currentPriority = GetThreadPriority(hAnimationThread);
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hAnimationThread, currentPriority + 1);
            animationPriority = currentPriority + 1;
        }
    }
}

void DecreaseAnimationPriority()
{
    if (hAnimationThread)
    {
        int currentPriority = GetThreadPriority(hAnimationThread);
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hAnimationThread, currentPriority - 1);
            animationPriority = currentPriority - 1;
        }
    }
}

// ==================== ФУНКЦИИ ВКЛЮЧЕНИЯ/ВЫКЛЮЧЕНИЯ СИНХРОНИЗАЦИИ ====================

// Включение синхронизации потоков
void EnableSynchronization()
{
    g_synchronizationEnabled = TRUE;  // Установка флага синхронизации

    // Принудительное обновление окна для отображения изменений
    HWND hWnd = FindWindow(szClassName, NULL);
    if (hWnd) InvalidateRect(hWnd, NULL, TRUE);

    // Информационное сообщение пользователю
    MessageBox(NULL, _T("Синхронизация включена\nКаждый поток будет выполнять N шагов атомарно"),
        _T("Синхронизация"), MB_OK | MB_ICONINFORMATION);
}

// Выключение синхронизации потоков
void DisableSynchronization()
{
    g_synchronizationEnabled = FALSE;  // Сброс флага синхронизации

    // Принудительное обновление окна
    HWND hWnd = FindWindow(szClassName, NULL);
    if (hWnd) InvalidateRect(hWnd, NULL, TRUE);

    // Информационное сообщение пользователю
    MessageBox(NULL, _T("Синхронизация выключена\nПотоки работают параллельно"),
        _T("Синхронизация"), MB_OK | MB_ICONINFORMATION);
}

// ==================== ОБРАБОТЧИК СООБЩЕНИЙ ГЛАВНОГО ОКНА ====================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:  // ОБРАБОТКА СООБЩЕНИЯ ТАЙМЕРА
        // При получении сообщения таймера принудительно перерисовываем окно
        // Это обеспечивает обновление информации о синхронизации в реальном времени
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_COMMAND:  // ОБРАБОТКА КОМАНД МЕНЮ И КНОПОК
    {
        int wmId = LOWORD(wParam);  // Извлекаем ID команды из младшего слова wParam
        switch (wmId)
        {
        case IDM_ABOUT:  // Команда "О программе"
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
            break;

        case IDM_EXIT:  // Команда "Выход"
            DestroyWindow(hWnd);  // Закрытие окна
            break;

            // Команды для потока 1
        case IDM_THREAD1_CREATE:  // Создать поток 1
            CreateThread1(FALSE);
            break;
        case IDM_THREAD1_CREATE_SUSPENDED:  // Создать ждущий поток 1
            CreateThread1(TRUE);
            break;
        case IDM_THREAD1_SUSPEND:  // Приостановить поток 1
            SuspendThread1();
            break;
        case IDM_THREAD1_RESUME:  // Возобновить поток 1
            ResumeThread1();
            break;
        case IDM_THREAD1_TERMINATE:  // Завершить поток 1
            TerminateThread1();
            break;
        case IDM_THREAD1_INCREASE_PRIORITY:  // Увеличить приоритет потока 1
            IncreasePriority1();
            break;
        case IDM_THREAD1_DECREASE_PRIORITY:  // Уменьшить приоритет потока 1
            DecreasePriority1();
            break;

            // Команды для потока 2
        case IDM_THREAD2_CREATE:
            CreateThread2(FALSE);
            break;
        case IDM_THREAD2_CREATE_SUSPENDED:
            CreateThread2(TRUE);
            break;
        case IDM_THREAD2_SUSPEND:
            SuspendThread2();
            break;
        case IDM_THREAD2_RESUME:
            ResumeThread2();
            break;
        case IDM_THREAD2_TERMINATE:
            TerminateThread2();
            break;
        case IDM_THREAD2_INCREASE_PRIORITY:
            IncreasePriority2();
            break;
        case IDM_THREAD2_DECREASE_PRIORITY:
            DecreasePriority2();
            break;

            // Команды информации о потоках
        case IDM_INFO_PRIMARY:  // Информация о первичном потоке
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_THREADINFO), hWnd, ThreadInfoDlgProc, 0);
            break;
        case IDM_INFO_THREAD1:  // Информация о потоке 1
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_THREADINFO), hWnd, ThreadInfoDlgProc, 1);
            break;
        case IDM_INFO_THREAD2:  // Информация о потоке 2
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_THREADINFO), hWnd, ThreadInfoDlgProc, 2);
            break;

            // Команды для потока анимации
        case IDM_ANIMATION_CREATE:
            CreateAnimationThread(FALSE);
            break;
        case IDM_ANIMATION_CREATE_SUSPENDED:
            CreateAnimationThread(TRUE);
            break;
        case IDM_ANIMATION_SUSPEND:
            SuspendAnimationThread();
            break;
        case IDM_ANIMATION_RESUME:
            ResumeAnimationThread();
            break;
        case IDM_ANIMATION_TERMINATE:
            TerminateAnimationThread();
            break;
        case IDM_ANIMATION_INCREASE_PRIORITY:
            IncreaseAnimationPriority();
            break;
        case IDM_ANIMATION_DECREASE_PRIORITY:
            DecreaseAnimationPriority();
            break;
        case IDM_INFO_ANIMATION:  // Информация о потоке анимации
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_THREADINFO), hWnd, ThreadInfoDlgProc, 3);
            break;

            // Команды синхронизации
        case IDM_SYNC_ENABLE:  // Включить синхронизацию
            EnableSynchronization();
            break;
        case IDM_SYNC_DISABLE:  // Выключить синхронизацию
            DisableSynchronization();
            break;
            // ==================== КОМАНДЫ ТАЙМЕРА ====================
        case IDM_TIMER_CREATE:  // Создать таймер
            CreateWaitableTimer();
            break;

        case IDM_TIMER_START:   // Запустить таймер
            StartWaitableTimer();
            break;

        case IDM_TIMER_STOP:    // Остановить таймер
            StopWaitableTimer();
            break;

        case IDM_TIMER_INFO:    // Информация о таймере
        {
            TCHAR info[256];
            _stprintf(info, _T("Состояние таймера: %s\nСрабатываний: %d\nТаймер: %s"),
                g_timerEnabled ? _T("Активен") : _T("Не активен"),
                g_timerCounter,
                hWaitableTimer ? _T("Создан") : _T("Не создан"));
            MessageBox(NULL, info, _T("Информация о таймере"), MB_OK | MB_ICONINFORMATION);
        }
        break;

        default:  // Обработка остальных команд по умолчанию
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:  // ПЕРЕРИСОВКА ОКНА
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // Начало перерисовки

        // СОЗДАНИЕ ШРИФТА ДЛЯ ОТОБРАЖЕНИЯ ИНФОРМАЦИИ
        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // ОТОБРАЖЕНИЕ ИНФОРМАЦИИ О СИНХРОНИЗАЦИИ
        SetBkMode(hdc, TRANSPARENT);  // Прозрачный фон для текста

        // ОТОБРАЖЕНИЕ ИНФОРМАЦИИ О ТАЙМЕРЕ
        SetTextColor(hdc, RGB(100, 50, 150));  // Фиолетовый текст
        TCHAR timerInfo[256];
        _stprintf(timerInfo, _T("Таймер: %s | Срабатываний: %d"),
            g_timerEnabled ? _T("Активен") : _T("Выключен"), g_timerCounter);
        TextOut(hdc, 20, 215, timerInfo, _tcslen(timerInfo));

        TCHAR syncInfo[256];
        if (g_synchronizationEnabled)  // Если синхронизация включена
        {
            SetTextColor(hdc, RGB(0, 128, 0));  // Зеленый текст
            _stprintf(syncInfo, _T("✓ СИНХРОНИЗАЦИЯ ВКЛЮЧЕНА (Режим К16: N = %d шагов)"),
                4 * _tcslen(g_thread1Text));
        }
        else  // Если синхронизация выключена
        {
            SetTextColor(hdc, RGB(200, 0, 0));  // Красный текст
            _tcscpy(syncInfo, _T("✗ СИНХРОНИЗАЦИЯ ВЫКЛЮЧЕНА (Параллельный режим)"));
        }
        TextOut(hdc, 20, 155, syncInfo, _tcslen(syncInfo));

        // ОТОБРАЖЕНИЕ СЧЕТЧИКОВ ШАГОВ ПОТОКОВ
        SetTextColor(hdc, RGB(0, 0, 128));  // Темно-синий текст
        TCHAR counterInfo[256];
        _stprintf(counterInfo, _T("Поток 1: %d шагов  |  Поток 2: %d шагов  |  Всего: %d шагов"),
            g_stepCounter[1], g_stepCounter[2], g_stepCounter[1] + g_stepCounter[2]);
        TextOut(hdc, 20, 175, counterInfo, _tcslen(counterInfo));

        // ОТОБРАЖЕНИЕ СТАТУСОВ ПОТОКОВ
        SetTextColor(hdc, RGB(80, 80, 80));  // Серый текст
        TCHAR statusInfo[256];
        _stprintf(statusInfo, _T("Статус: Поток1-%s | Поток2-%s | Анимация-%s"),
            hSecThread[1] ? (threadStates[1] == 1 ? _T("Активен") : _T("Приостановлен")) : _T("Не создан"),
            hSecThread[2] ? (threadStates[2] == 1 ? _T("Активен") : _T("Приостановлен")) : _T("Не создан"),
            hAnimationThread ? (animationState == 1 ? _T("Активна") : _T("Приостановлена")) : _T("Не создана"));
        TextOut(hdc, 20, 195, statusInfo, _tcslen(statusInfo));

        // ВОССТАНОВЛЕНИЕ ИСХОДНОГО ШРИФТА И УДАЛЕНИЕ СОЗДАННОГО
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);  // Завершение перерисовки
    }
    break;

    case WM_DESTROY:  // ЗАКРЫТИЕ ОКНА
        // Уничтожение таймера обновления
        if (g_updateTimer != 0)
        {
            KillTimer(hWnd, g_updateTimer);
            g_updateTimer = 0;
        }

        // Завершение всех потоков перед выходом из приложения
        if (hSecThread[1]) TerminateThread1();
        if (hSecThread[2]) TerminateThread2();
        if (hAnimationThread) TerminateAnimationThread();

        PostQuitMessage(0);  // Отправка сообщения WM_QUIT для выхода из цикла
        break;
        // Остановка таймера при выходе
        if (g_timerEnabled)
        {
            StopWaitableTimer();
        }

        // Закрытие дескриптора таймера
        if (hWaitableTimer)
        {
            CloseHandle(hWaitableTimer);
            hWaitableTimer = NULL;
        }

    default:  // Обработка остальных сообщений по умолчанию
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ==================== ОБРАБОТЧИК ДИАЛОГА "О ПРОГРАММЕ" ====================
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:  // Инициализация диалога
        return TRUE;     // Возврат TRUE означает, что фокус установлен правильно

    case WM_COMMAND:     // Обработка команд в диалоге
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // Закрытие диалога
            return TRUE;
        }
        break;
    }
    return FALSE;  // Возврат FALSE означает, что сообщение не обработано
}

// ==================== ОБРАБОТЧИК ДИАЛОГА ИНФОРМАЦИИ О ПОТОКЕ ====================
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int threadIndex;  // Сохранение индекса потока между вызовами

    switch (message)
    {
    case WM_INITDIALOG:  // Инициализация диалога
        threadIndex = (int)lParam;  // Получение индекса потока из параметра
        UpdateThreadInfo(hDlg, threadIndex);  // Обновление информации о потоке
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // Закрытие диалога
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ==================== ОБНОВЛЕНИЕ ИНФОРМАЦИИ О ПОТОКЕ В ДИАЛОГЕ ====================
void UpdateThreadInfo(HWND hDlg, int threadIndex)
{
    TCHAR buffer[256];
    DWORD exitCode;
    FILETIME creationTime, exitTime, kernelTime, userTime;
    SYSTEMTIME sysTime;
    ULONGLONG lifeTime;

    // Установка имени потока в зависимости от индекса
    if (threadIndex == 0)
        SetDlgItemText(hDlg, IDC_THREAD_NAME, _T("Первичный поток"));
    else if (threadIndex == 1)
        SetDlgItemText(hDlg, IDC_THREAD_NAME, _T("Поток 1"));
    else if (threadIndex == 2)
        SetDlgItemText(hDlg, IDC_THREAD_NAME, _T("Поток 2"));
    else if (threadIndex == 3)
        SetDlgItemText(hDlg, IDC_THREAD_NAME, _T("Поток анимации"));

    // Отображение ID потока
    if (threadIndex == 0)
        dwSecThreadId[0] = GetCurrentThreadId();  // ID первичного потока

    if (threadIndex == 3)
        _stprintf(buffer, _T("0x%08X"), dwAnimationThreadId);  // ID потока анимации
    else
        _stprintf(buffer, _T("0x%08X"), dwSecThreadId[threadIndex]);  // ID других потоков
    SetDlgItemText(hDlg, IDC_THREAD_ID, buffer);

    // Отображение дескриптора потока
    if (threadIndex == 0)
        hSecThread[0] = GetCurrentThread();  // Дескриптор первичного потока

    if (threadIndex == 3)
        _stprintf(buffer, _T("0x%p"), hAnimationThread);  // Дескриптор потока анимации
    else
        _stprintf(buffer, _T("0x%p"), hSecThread[threadIndex]);  // Дескриптор других потоков
    SetDlgItemText(hDlg, IDC_THREAD_HANDLE, buffer);

    // Отображение состояния потока
    if (threadIndex == 0)  // Первичный поток всегда активен
    {
        SetDlgItemText(hDlg, IDC_THREAD_STATE, _T("Активный"));
    }
    else if (threadIndex == 3)  // Поток анимации
    {
        if (hAnimationThread && GetExitCodeThread(hAnimationThread, &exitCode))
        {
            if (exitCode == STILL_ACTIVE)  // Поток еще активен
                SetDlgItemText(hDlg, IDC_THREAD_STATE,
                    animationState == 1 ? _T("Активный") : _T("Приостановленный"));
            else  // Поток завершен
            {
                _stprintf(buffer, _T("Завершен (%lu)"), exitCode);
                SetDlgItemText(hDlg, IDC_THREAD_STATE, buffer);
            }
        }
        else  // Поток не создан
        {
            SetDlgItemText(hDlg, IDC_THREAD_STATE, _T("Не создан"));
        }
    }
    else if (hSecThread[threadIndex] && GetExitCodeThread(hSecThread[threadIndex], &exitCode))
    {
        if (exitCode == STILL_ACTIVE)
            SetDlgItemText(hDlg, IDC_THREAD_STATE,
                threadStates[threadIndex] == 1 ? _T("Активный") : _T("Приостановленный"));
        else
        {
            _stprintf(buffer, _T("Завершен (%lu)"), exitCode);
            SetDlgItemText(hDlg, IDC_THREAD_STATE, buffer);
        }
    }
    else
    {
        SetDlgItemText(hDlg, IDC_THREAD_STATE, _T("Не создан"));
    }

    // Отображение приоритета потока
    if (threadIndex == 0)  // Первичный поток
    {
        int priority = GetThreadPriority(GetCurrentThread());
        // Преобразование числового значения приоритета в текстовое представление
        switch (priority)
        {
        case THREAD_PRIORITY_IDLE: _tcscpy(buffer, _T("IDLE")); break;
        case THREAD_PRIORITY_LOWEST: _tcscpy(buffer, _T("LOWEST")); break;
        case THREAD_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("BELOW_NORMAL")); break;
        case THREAD_PRIORITY_NORMAL: _tcscpy(buffer, _T("NORMAL")); break;
        case THREAD_PRIORITY_ABOVE_NORMAL: _tcscpy(buffer, _T("ABOVE_NORMAL")); break;
        case THREAD_PRIORITY_HIGHEST: _tcscpy(buffer, _T("HIGHEST")); break;
        case THREAD_PRIORITY_TIME_CRITICAL: _tcscpy(buffer, _T("TIME_CRITICAL")); break;
        default: _stprintf(buffer, _T("%d"), priority);
        }
        SetDlgItemText(hDlg, IDC_THREAD_PRIORITY, buffer);
    }
    else if (threadIndex == 3)  // Поток анимации
    {
        if (hAnimationThread)
        {
            int priority = GetThreadPriority(hAnimationThread);
            switch (priority)
            {
            case THREAD_PRIORITY_IDLE: _tcscpy(buffer, _T("IDLE")); break;
            case THREAD_PRIORITY_LOWEST: _tcscpy(buffer, _T("LOWEST")); break;
            case THREAD_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("BELOW_NORMAL")); break;
            case THREAD_PRIORITY_NORMAL: _tcscpy(buffer, _T("NORMAL")); break;
            case THREAD_PRIORITY_ABOVE_NORMAL: _tcscpy(buffer, _T("ABOVE_NORMAL")); break;
            case THREAD_PRIORITY_HIGHEST: _tcscpy(buffer, _T("HIGHEST")); break;
            case THREAD_PRIORITY_TIME_CRITICAL: _tcscpy(buffer, _T("TIME_CRITICAL")); break;
            default: _stprintf(buffer, _T("%d"), priority);
            }
            SetDlgItemText(hDlg, IDC_THREAD_PRIORITY, buffer);
        }
        else
        {
            SetDlgItemText(hDlg, IDC_THREAD_PRIORITY, _T("Не доступно"));
        }
    }
    else if (hSecThread[threadIndex])  // Потоки 1 и 2
    {
        int priority = GetThreadPriority(hSecThread[threadIndex]);
        switch (priority)
        {
        case THREAD_PRIORITY_IDLE: _tcscpy(buffer, _T("IDLE")); break;
        case THREAD_PRIORITY_LOWEST: _tcscpy(buffer, _T("LOWEST")); break;
        case THREAD_PRIORITY_BELOW_NORMAL: _tcscpy(buffer, _T("BELOW_NORMAL")); break;
        case THREAD_PRIORITY_NORMAL: _tcscpy(buffer, _T("NORMAL")); break;
        case THREAD_PRIORITY_ABOVE_NORMAL: _tcscpy(buffer, _T("ABOVE_NORMAL")); break;
        case THREAD_PRIORITY_HIGHEST: _tcscpy(buffer, _T("HIGHEST")); break;
        case THREAD_PRIORITY_TIME_CRITICAL: _tcscpy(buffer, _T("TIME_CRITICAL")); break;
        default: _stprintf(buffer, _T("%d"), priority);
        }
        SetDlgItemText(hDlg, IDC_THREAD_PRIORITY, buffer);
    }
    else
    {
        SetDlgItemText(hDlg, IDC_THREAD_PRIORITY, _T("Не доступно"));
    }

    // Отображение времени работы потока
    HANDLE hThread = NULL;
    if (threadIndex == 0)
        hThread = GetCurrentThread();  // Первичный поток
    else if (threadIndex == 3)
        hThread = hAnimationThread;    // Поток анимации
    else
        hThread = hSecThread[threadIndex]; // Потоки 1 и 2

    if (hThread && GetThreadTimes(hThread, &creationTime, &exitTime, &kernelTime, &userTime))
    {
        // Вычисление времени работы потока в миллисекундах
        ULARGE_INTEGER ulCreation;
        ulCreation.LowPart = creationTime.dwLowDateTime;
        ulCreation.HighPart = creationTime.dwHighDateTime;

        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        ULARGE_INTEGER ulNow;
        ulNow.LowPart = now.dwLowDateTime;
        ulNow.HighPart = now.dwHighDateTime;

        // Преобразование из 100-наносекундных интервалов в миллисекунды
        lifeTime = (ulNow.QuadPart - ulCreation.QuadPart) / 10000;
        _stprintf(buffer, _T("%llu мс"), lifeTime);
        SetDlgItemText(hDlg, IDC_THREAD_LIFETIME, buffer);
    }
    else
    {
        SetDlgItemText(hDlg, IDC_THREAD_LIFETIME, _T("Не доступно"));
    }
}

// ==================== ФУНКЦИИ ДЛЯ WAITABLE TIMER (ВАРИАНТ Т3) ====================

// Создание ждущего таймера
void CreateWaitableTimer()
{
    if (hWaitableTimer != NULL)
    {
        MessageBox(NULL, _T("Таймер уже создан!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    // Создание ждущего таймера
    hWaitableTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (hWaitableTimer)
    {
        MessageBox(NULL, _T("Ждущий таймер создан успешно!"), _T("Таймер"), MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL, _T("Ошибка создания таймера!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    }
}

// Запуск таймера
void StartWaitableTimer()
{
    if (hWaitableTimer == NULL)
    {
        MessageBox(NULL, _T("Сначала создайте таймер!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    if (g_timerEnabled)
    {
        MessageBox(NULL, _T("Таймер уже запущен!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    LARGE_INTEGER liDueTime;
    const int nTimerUnitsPerSecond = 10000000; // 100-наносекундные интервалы в секунде

    // Установка времени первого срабатывания: через 15 секунд
    liDueTime.QuadPart = -(15 * nTimerUnitsPerSecond);

    // Установка таймера: первый сигнал через 15 секунд, затем каждые 15 секунд
    if (SetWaitableTimer(hWaitableTimer, &liDueTime, 15 * 1000, NULL, NULL, FALSE))
    {
        g_timerEnabled = TRUE;
        g_timerCounter = 0;

        // Создание потока для обработки таймера
        HANDLE hTimerThread = CreateThread(NULL, 0, TimerThreadProc, NULL, 0, NULL);
        if (hTimerThread)
        {
            CloseHandle(hTimerThread); // Закрываем дескриптор, так как он нам не нужен
        }

        MessageBox(NULL, _T("Таймер запущен!\nКаждые 15 секунд будет появляться MessageBox."),
            _T("Таймер"), MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL, _T("Ошибка запуска таймера!"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    }
}

// Остановка таймера
void StopWaitableTimer()
{
    if (hWaitableTimer && g_timerEnabled)
    {
        // Отмена таймера
        CancelWaitableTimer(hWaitableTimer);
        g_timerEnabled = FALSE;
        MessageBox(NULL, _T("Таймер остановлен!"), _T("Таймер"), MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL, _T("Таймер не активен!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
    }
}

// Функция потока для обработки таймера
DWORD WINAPI TimerThreadProc(LPVOID lpParam)
{
    while (g_timerEnabled)
    {
        // Ожидание сигнала от таймера с таймаутом 15 секунд
        DWORD dwResult = WaitForSingleObject(hWaitableTimer, 15000);

        if (dwResult == WAIT_OBJECT_0) // Таймер сработал
        {
            // Таймер сработал - показываем MessageBox
            ShowTimerMessageBox();
        }
        else if (dwResult == WAIT_TIMEOUT) // Таймаут 15 секунд
        {
            // Если ответ задерживается более 15 секунд - считаем это как RETRY
            g_timerCounter++;
            // Можно добавить логирование или другие действия
        }
        else if (dwResult == WAIT_ABANDONED) // Объект abandoned
        {
            break; // Выход из цикла
        }

        // Проверка флага на каждой итерации
        if (!g_timerEnabled)
            break;
    }
    return 0;
}

// Показать MessageBox с выбором действия
void ShowTimerMessageBox()
{
    g_timerCounter++; // Увеличиваем счетчик вызовов

    // Формирование текста сообщения
    TCHAR message[256];
    _stprintf(message, _T("Вызов номер %d\n\nПродолжить - RETRY\nЗавершить работу - ABORT"), g_timerCounter);

    // Показ MessageBox с таймаутом 15 секунд
    int result = MessageBox(NULL, message, _T("Ждущий таймер Т3"),
        MB_ABORTRETRYIGNORE | MB_ICONQUESTION | MB_DEFBUTTON1);

    // Обработка результата
    switch (result)
    {
    case IDABORT: // Завершить работу
        MessageBox(NULL, _T("Работа приложения завершается по запросу пользователя."),
            _T("Завершение"), MB_OK | MB_ICONINFORMATION);
        PostQuitMessage(0); // Завершение приложения
        break;

    case IDRETRY: // Продолжить
    case IDIGNORE: // Игнорировать (тоже продолжаем)
        // Просто продолжаем работу
        break;

    default:
        // Любой другой случай (например, закрытие крестиком) - продолжаем
        break;
    }
}