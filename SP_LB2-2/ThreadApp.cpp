#define _CRT_SECURE_NO_WARNINGS  // Отключаем предупреждения о безопасности CRT функций
#define WIN32_LEAN_AND_MEAN      // Исключаем редко используемые компоненты Windows
#include <windows.h>             // Основной заголовочный файл Windows API
#include <tchar.h>               // Поддержка Unicode/ANSI символов
#include <stdio.h>               // Стандартный ввод-вывод
#include <time.h>                // Функции работы со временем
#include <stdlib.h>  // Для функций rand() и srand()

#pragma comment(linker, "/subsystem:windows")  // Указываем линкеру создать Windows приложение (не консольное)

// Глобальные переменные
HINSTANCE hInst;                            // Дескриптор экземпляра приложения (устанавливается в WinMain)
TCHAR szClassName[] = _T("ThreadAppClass"); // Имя класса главного окна
TCHAR szTitle[100] = _T("Гончаров_thread_lb"); // Заголовок окна (замените на свою фамилию)

HANDLE hAnimationThread = NULL;    // Дескриптор потока анимации
DWORD dwAnimationThreadId = 0;     // ID потока анимации
int animationState = 0;            // Состояние потока анимации (0-не создан, 1-активен, 2-приостановлен)
int animationPriority = 0;         // Приоритет потока анимации

// Параметры анимации шарика
int ballX = 100;                   // Позиция X шарика
int ballY = 100;                   // Позиция Y шарика
int ballSize = 30;                 // Размер шарика
int ballSpeedX = 3;                // Скорость по X
int ballSpeedY = 2;                // Скорость по Y
COLORREF ballColor = RGB(255, 0, 0); // Цвет шарика (красный)

// Переменные для управления потоками
HANDLE hSecThread[3] = { NULL };      // Массив дескрипторов потоков: [0] - первичный, [1] - поток 1, [2] - поток 2
DWORD dwSecThreadId[3] = { 0 };       // Массив идентификаторов потоков (Thread ID)
int g_uThCount = 0;                   // Счетчик созданных вторичных потоков
int threadStates[3] = { 0 };          // Состояния потоков: 0 - не создан, 1 - активен, 2 - приостановлен
int threadPriorities[3] = { 0 };      // Приоритеты потоков (значения из THREAD_PRIORITY_*)

// Позиции для бегущих строк
int thread1Pos = 0;  // Текущая позиция X для строки потока 1
int thread2Pos = 0;  // Текущая позиция X для строки потока 2

// Прототипы функций (объявления функций перед их использованием)
BOOL InitApplication(HINSTANCE hInstance);  // Инициализация приложения (регистрация класса окна)
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);  // Создание главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // Обработчик сообщений главного окна
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);  // Обработчик диалога "О программе"
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);  // Обработчик диалога информации о потоке
void UpdateThreadInfo(HWND hDlg, int threadIndex);  // Обновление информации о потоке в диалоге

DWORD WINAPI AnimationThread(LPVOID lpParam);
void CreateAnimationThread(BOOL suspended);
void SuspendAnimationThread();
void ResumeAnimationThread();
void TerminateAnimationThread();
void IncreaseAnimationPriority();
void DecreaseAnimationPriority();
// Функции потоков (точки входа для вторичных потоков)
DWORD WINAPI ThreadFunc1(LPVOID lpParam);  // Функция потока 1 (бегущая строка)
DWORD WINAPI ThreadFunc2(LPVOID lpParam);  // Функция потока 2 (бегущая строка)

// Функции управления потоками
void CreateThread1(BOOL suspended);  // Создание потока 1 (suspended = TRUE для ждущего потока)
void CreateThread2(BOOL suspended);  // Создание потока 2
void SuspendThread1();               // Приостановка потока 1
void ResumeThread1();                // Возобновление потока 1
void TerminateThread1();             // Завершение потока 1
void IncreasePriority1();            // Увеличение приоритета потока 1
void DecreasePriority1();            // Уменьшение приоритета потока 1
void SuspendThread2();               // Приостановка потока 2
void ResumeThread2();                // Возобновление потока 2
void TerminateThread2();             // Завершение потока 2
void IncreasePriority2();            // Увеличение приоритета потока 2
void DecreasePriority2();            // Уменьшение приоритета потока 2

// Точка входа Windows приложения (аналог main() для консольных программ)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;  // Структура для хранения сообщений

    // Инициализация приложения: регистрация класса окна
    if (!InitApplication(hInstance))
        return FALSE;  // Выход при ошибке инициализации

    // Создание и отображение главного окна
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;  // Выход при ошибке создания окна

    // Основной цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0))  // Получение сообщения из очереди
    {
        TranslateMessage(&msg);  // Преобразование сообщений клавиш (например, WM_KEYDOWN -> WM_CHAR)
        DispatchMessage(&msg);   // Передача сообщения соответствующей оконной процедуре
    }

    return (int)msg.wParam;  // Возврат кода завершения
}

// Инициализация приложения - регистрация класса окна
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;  // Структура для описания класса окна

    wcex.cbSize = sizeof(WNDCLASSEX);           // Размер структуры (обязательно)
    wcex.style = CS_HREDRAW | CS_VREDRAW;       // Стиль: перерисовка при изменении размера
    wcex.lpfnWndProc = WndProc;                 // Указатель на функцию обработки сообщений
    wcex.cbClsExtra = 0;                        // Дополнительная память для класса
    wcex.cbWndExtra = 0;                        // Дополнительная память для каждого окна
    wcex.hInstance = hInstance;                 // Дескриптор экземпляра приложения
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Стандартная иконка
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   // Стандартный курсор
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Цвет фона (стандартный цвет окна)
    wcex.lpszMenuName = MAKEINTRESOURCE(101);   // Идентификатор меню (из ресурсов)
    wcex.lpszClassName = szClassName;           // Имя класса окна
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Маленькая иконка

    return RegisterClassEx(&wcex);  // Регистрация класса окна в системе
}

// Создание и отображение главного окна
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;  // Дескриптор создаваемого окна

    hInst = hInstance;  // Сохранение дескриптора экземпляра в глобальной переменной

    // Создание главного окна
    hWnd = CreateWindow(szClassName,           // Имя класса окна
        szTitle,               // Заголовок окна
        WS_OVERLAPPEDWINDOW,   // Стиль окна (стандартное перекрывающееся окно)
        CW_USEDEFAULT,         // Позиция X по умолчанию
        CW_USEDEFAULT,         // Позиция Y по умолчанию
        800,                   // Ширина окна
        600,                   // Высота окна
        NULL,                  // Родительское окно (нет)
        NULL,                  // Меню (уже указано в классе окна)
        hInstance,             // Дескриптор экземпляра
        NULL);                 // Дополнительные параметры создания

    if (!hWnd)  // Проверка успешности создания окна
        return FALSE;

    // Отображение и обновление окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Функция потока 1 - выводит бегущую строку в главном окне
DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;  // Получаем дескриптор главного окна из параметра
    HDC hdc;                    // Контекст устройства для рисования
    RECT rect;                  // Прямоугольник для хранения размеров клиентской области
    TCHAR szMsg[] = _T("Вторичный поток 1 создал Гончаров"); // Текст для отображения

    // Бесконечный цикл потока (завершается принудительно)
    while (TRUE)
    {
        // Проверяем состояние потока - рисуем только если поток активен
        if (threadStates[1] == 1) // Если поток активен
        {
            hdc = GetDC(hWnd);  // Получаем контекст устройства для рисования
            if (hdc)
            {
                GetClientRect(hWnd, &rect);  // Получаем размеры клиентской области окна

                // Очистка предыдущей позиции текста (затираем старый текст)
                SetBkColor(hdc, GetSysColor(COLOR_WINDOW));     // Цвет фона = цвет окна
                SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT)); // Цвет текста = стандартный
                TextOut(hdc, thread1Pos - 1, 50, _T(""), 0);    // Затираем старый текст

                // Вывод нового текста
                SetBkColor(hdc, RGB(255, 255, 0));  // Желтый фон
                SetTextColor(hdc, RGB(0, 0, 255));  // Синий текст
                TextOut(hdc, thread1Pos, 50, szMsg, _tcslen(szMsg));  // Рисуем текст

                ReleaseDC(hWnd, hdc);  // Освобождаем контекст устройства
            }

            thread1Pos += 5;  // Перемещаем позицию текста
            if (thread1Pos > rect.right)  // Если текст ушел за правую границу
                thread1Pos = -300;        // Возвращаем в начало (с запасом для плавности)
        }

        Sleep(100);  // Задержка для контроля скорости движения
    }

    return 0;  // Код завершения (никогда не выполняется из-за бесконечного цикла)
}

// Функция потока 2 - аналогична потоку 1, но с другими параметрами
DWORD WINAPI ThreadFunc2(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    HDC hdc;
    RECT rect;
    TCHAR szMsg[] = _T("Вторичный поток 2 создал Гончаров");

    while (TRUE)
    {
        if (threadStates[2] == 1) // Если поток активен
        {
            hdc = GetDC(hWnd);
            if (hdc)
            {
                GetClientRect(hWnd, &rect);

                // Очистка предыдущей позиции
                SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
                SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
                TextOut(hdc, thread2Pos - 1, 100, _T(""), 0);

                // Вывод текста (другие цвета и позиция)
                SetBkColor(hdc, RGB(255, 200, 200));  // Светло-красный фон
                SetTextColor(hdc, RGB(255, 0, 0));    // Красный текст
                TextOut(hdc, thread2Pos, 100, szMsg, _tcslen(szMsg));

                ReleaseDC(hWnd, hdc);
            }

            thread2Pos += 5;  // Другая скорость движения
            if (thread2Pos > rect.right)
                thread2Pos = -300;
        }

        Sleep(100);
    }

    return 0;
}

DWORD WINAPI AnimationThread(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    HDC hdc;
    RECT rect;

    while (TRUE)
    {
        if (animationState == 1) // Если поток анимации активен
        {
            hdc = GetDC(hWnd);
            if (hdc)
            {
                GetClientRect(hWnd, &rect);

                // Стираем предыдущее положение шарика
                HBRUSH hWhiteBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hWhiteBrush);
                HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
                Ellipse(hdc, ballX - 1, ballY - 1, ballX + ballSize + 1, ballY + ballSize + 1);

                // Обновляем позицию шарика
                ballX += ballSpeedX;
                ballY += ballSpeedY;

                // Проверяем столкновение с границами
                if (ballX <= 0 || ballX + ballSize >= rect.right)
                {
                    ballSpeedX = -ballSpeedX; // Отскок по X
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // Меняем цвет
                }
                if (ballY <= 0 || ballY + ballSize >= rect.bottom)
                {
                    ballSpeedY = -ballSpeedY; // Отскок по Y
                    ballColor = RGB(rand() % 256, rand() % 256, rand() % 256); // Меняем цвет
                }

                // Рисуем шарик
                HBRUSH hBallBrush = CreateSolidBrush(ballColor);
                SelectObject(hdc, hBallBrush);
                Ellipse(hdc, ballX, ballY, ballX + ballSize, ballY + ballSize);

                // Восстанавливаем и удаляем объекты
                SelectObject(hdc, hOldBrush);
                SelectObject(hdc, hOldPen);
                DeleteObject(hWhiteBrush);
                DeleteObject(hBallBrush);

                ReleaseDC(hWnd, hdc);
            }
        }
        Sleep(30); // Задержка для плавности анимации
    }
    return 0;
}

// Создание потока 1
void CreateThread1(BOOL suspended)
{
    // Проверка: если поток уже создан, показываем сообщение об ошибке
    if (hSecThread[1] != NULL)
    {
        MessageBox(NULL, _T("Поток 1 уже создан!"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
        return;
    }

    // Получаем дескриптор главного окна
    HWND hWnd = FindWindow(szClassName, NULL);
    // Создаем поток: NULL - безопасность по умолчанию, 0 - размер стека по умолчанию,
    // ThreadFunc1 - функция потока, hWnd - параметр, передаваемый в функцию,
    // CREATE_SUSPENDED или 0 - флаги создания, &dwSecThreadId[1] - получаем ID потока
    hSecThread[1] = CreateThread(NULL, 0, ThreadFunc1, hWnd, suspended ? CREATE_SUSPENDED : 0, &dwSecThreadId[1]);

    if (hSecThread[1])  // Если поток успешно создан
    {
        // Устанавливаем состояние и приоритет
        threadStates[1] = suspended ? 2 : 1;  // 2 - приостановлен, 1 - активен
        threadPriorities[1] = THREAD_PRIORITY_NORMAL;  // Нормальный приоритет
        SetThreadPriority(hSecThread[1], threadPriorities[1]);  // Устанавливаем приоритет
        g_uThCount++;  // Увеличиваем счетчик потоков
    }
}

// Создание потока 2 (аналогично потоку 1)
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

// Приостановка потока 1
void SuspendThread1()
{
    // Проверяем: если поток существует и активен
    if (hSecThread[1] && threadStates[1] == 1)
    {
        SuspendThread(hSecThread[1]);  // Приостанавливаем поток
        threadStates[1] = 2;           // Устанавливаем состояние "приостановлен"
    }
}

// Возобновление потока 1
void ResumeThread1()
{
    // Проверяем: если поток существует и приостановлен
    if (hSecThread[1] && threadStates[1] == 2)
    {
        ResumeThread(hSecThread[1]);  // Возобновляем поток
        threadStates[1] = 1;          // Устанавливаем состояние "активен"
    }
}

// Завершение потока 1
void TerminateThread1()
{
    if (hSecThread[1])
    {
        TerminateThread(hSecThread[1], 0);  // Принудительное завершение потока с кодом 0
        CloseHandle(hSecThread[1]);         // Закрываем дескриптор потока
        hSecThread[1] = NULL;               // Обнуляем дескриптор
        dwSecThreadId[1] = 0;               // Обнуляем ID потока
        threadStates[1] = 0;                // Устанавливаем состояние "не создан"
        g_uThCount--;                       // Уменьшаем счетчик потоков

        // Очистка области вывода (перерисовываем окно)
        HWND hWnd = FindWindow(szClassName, NULL);
        InvalidateRect(hWnd, NULL, TRUE);  // Отправляем сообщение WM_PAINT
    }
}

// Увеличение приоритета потока 1
void IncreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);  // Текущий приоритет
        // Проверяем, можно ли увеличить приоритет (не выше HIGHEST)
        if (currentPriority < THREAD_PRIORITY_HIGHEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority + 1);  // Увеличиваем приоритет
            threadPriorities[1] = currentPriority + 1;              // Сохраняем новое значение
        }
    }
}

// Уменьшение приоритета потока 1
void DecreasePriority1()
{
    if (hSecThread[1])
    {
        int currentPriority = GetThreadPriority(hSecThread[1]);
        // Проверяем, можно ли уменьшить приоритет (не ниже LOWEST)
        if (currentPriority > THREAD_PRIORITY_LOWEST)
        {
            SetThreadPriority(hSecThread[1], currentPriority - 1);
            threadPriorities[1] = currentPriority - 1;
        }
    }
}

// Функции для потока 2 (аналогичны функциям потока 1)
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

        // Очистка области анимации
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

// Обработчик сообщений главного окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:  // Обработка команд меню и других элементов управления
    {
        int wmId = LOWORD(wParam);  // Извлекаем ID команды
        switch (wmId)
        {
        case 1001: // IDM_ABOUT - "О программе"
            DialogBox(hInst, MAKEINTRESOURCE(102), hWnd, AboutDlgProc);
            break;

        case 1002: // IDM_EXIT - "Выход"
            DestroyWindow(hWnd);  // Закрываем окно
            break;

            // Команды для потока 1
        case 1101: // IDM_THREAD1_CREATE - "Создать поток"
            CreateThread1(FALSE);  // Создаем активный поток
            break;
        case 1102: // IDM_THREAD1_CREATE_SUSPENDED - "Создать ждущий поток"
            CreateThread1(TRUE);   // Создаем приостановленный поток
            break;
        case 1103: // IDM_THREAD1_SUSPEND - "Приостановить поток"
            SuspendThread1();
            break;
        case 1104: // IDM_THREAD1_RESUME - "Продолжить работу потока"
            ResumeThread1();
            break;
        case 1105: // IDM_THREAD1_TERMINATE - "Уничтожить поток"
            TerminateThread1();
            break;
        case 1106: // IDM_THREAD1_INCREASE_PRIORITY - "Увеличить приоритет"
            IncreasePriority1();
            break;
        case 1107: // IDM_THREAD1_DECREASE_PRIORITY - "Уменьшить приоритет"
            DecreasePriority1();
            break;

            // Команды для потока 2 (аналогичны потоку 1)
        case 1201: // IDM_THREAD2_CREATE
            CreateThread2(FALSE);
            break;
        case 1202: // IDM_THREAD2_CREATE_SUSPENDED
            CreateThread2(TRUE);
            break;
        case 1203: // IDM_THREAD2_SUSPEND
            SuspendThread2();
            break;
        case 1204: // IDM_THREAD2_RESUME
            ResumeThread2();
            break;
        case 1205: // IDM_THREAD2_TERMINATE
            TerminateThread2();
            break;
        case 1206: // IDM_THREAD2_INCREASE_PRIORITY
            IncreasePriority2();
            break;
        case 1207: // IDM_THREAD2_DECREASE_PRIORITY
            DecreasePriority2();
            break;

            // Команды информации о потоках
        case 1301: // IDM_INFO_PRIMARY - "Первичный поток"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 0);
            break;
        case 1302: // IDM_INFO_THREAD1 - "Поток 1"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 1);
            break;
        case 1303: // IDM_INFO_THREAD2 - "Поток 2"
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 2);
            break;
        case 1401: // IDM_ANIMATION_CREATE
            CreateAnimationThread(FALSE);
            break;
        case 1402: // IDM_ANIMATION_CREATE_SUSPENDED
            CreateAnimationThread(TRUE);
            break;
        case 1403: // IDM_ANIMATION_SUSPEND
            SuspendAnimationThread();
            break;
        case 1404: // IDM_ANIMATION_RESUME
            ResumeAnimationThread();
            break;
        case 1405: // IDM_ANIMATION_TERMINATE
            TerminateAnimationThread();
            break;
        case 1406: // IDM_ANIMATION_INCREASE_PRIORITY
            IncreaseAnimationPriority();
            break;
        case 1407: // IDM_ANIMATION_DECREASE_PRIORITY
            DecreaseAnimationPriority();
            break;
        case 1408: // IDM_INFO_ANIMATION
            DialogBoxParam(hInst, MAKEINTRESOURCE(103), hWnd, ThreadInfoDlgProc, 3);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);  // Стандартная обработка
        }
    }
    break;

    case WM_PAINT:  // Сообщение перерисовки окна
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);  // Начало перерисовки
        // Здесь можно добавить дополнительную отрисовку
        EndPaint(hWnd, &ps);  // Завершение перерисовки
    }
    break;

    case WM_DESTROY:  // Сообщение закрытия окна
        // Завершение всех потоков перед выходом
        if (hSecThread[1]) TerminateThread1();
        if (hSecThread[2]) TerminateThread2();
        if (hAnimationThread) TerminateAnimationThread();
        PostQuitMessage(0);  // Отправляем сообщение WM_QUIT для выхода из цикла
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);  // Стандартная обработка остальных сообщений
    }
    return 0;
}

// Обработчик диалога "О программе"
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:  // Инициализация диалога
        return TRUE;

    case WM_COMMAND:  // Обработка команд в диалоге
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));  // Закрываем диалог
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Обработчик диалога информации о потоке
INT_PTR CALLBACK ThreadInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int threadIndex;  // Сохраняем индекс потока между вызовами

    switch (message)
    {
    case WM_INITDIALOG:  // Инициализация диалога
        threadIndex = (int)lParam;  // Получаем индекс потока из параметра
        UpdateThreadInfo(hDlg, threadIndex);  // Обновляем информацию о потоке
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Обновление информации о потоке в диалоговом окне
void UpdateThreadInfo(HWND hDlg, int threadIndex)
{
    TCHAR buffer[256];
    DWORD exitCode;
    FILETIME creationTime, exitTime, kernelTime, userTime;
    SYSTEMTIME sysTime;
    ULONGLONG lifeTime;

    // Заголовок - имя потока
    if (threadIndex == 0)
        SetDlgItemText(hDlg, 1401, _T("Первичный поток"));
    else if (threadIndex == 1)
        SetDlgItemText(hDlg, 1401, _T("Поток 1"));
    else if (threadIndex == 2)
        SetDlgItemText(hDlg, 1401, _T("Поток 2"));
    else if (threadIndex == 3)
        SetDlgItemText(hDlg, 1401, _T("Поток анимации"));
    else
    {
        _stprintf(buffer, _T("Поток %d"), threadIndex);
        SetDlgItemText(hDlg, 1401, buffer);
    }

    // ID потока
    if (threadIndex == 0)
        dwSecThreadId[0] = GetCurrentThreadId();

    if (threadIndex == 3) // Поток анимации
    {
        _stprintf(buffer, _T("0x%08X"), dwAnimationThreadId);
    }
    else
    {
        _stprintf(buffer, _T("0x%08X"), dwSecThreadId[threadIndex]);
    }
    SetDlgItemText(hDlg, 1402, buffer);

    // Дескриптор потока
    if (threadIndex == 0)
        hSecThread[0] = GetCurrentThread();

    if (threadIndex == 3) // Поток анимации
    {
        _stprintf(buffer, _T("0x%p"), hAnimationThread);
    }
    else
    {
        _stprintf(buffer, _T("0x%p"), hSecThread[threadIndex]);
    }
    SetDlgItemText(hDlg, 1403, buffer);

    // Состояние потока
    if (threadIndex == 0)
    {
        SetDlgItemText(hDlg, 1404, _T("Активный"));
    }
    else if (threadIndex == 3) // Поток анимации
    {
        if (hAnimationThread)
        {
            if (GetExitCodeThread(hAnimationThread, &exitCode))
            {
                if (exitCode == STILL_ACTIVE)
                {
                    if (animationState == 1)
                        SetDlgItemText(hDlg, 1404, _T("Активный"));
                    else
                        SetDlgItemText(hDlg, 1404, _T("Приостановленный"));
                }
                else
                {
                    _stprintf(buffer, _T("Завершен (%lu)"), exitCode);
                    SetDlgItemText(hDlg, 1404, buffer);
                }
            }
        }
        else
        {
            SetDlgItemText(hDlg, 1404, _T("Не создан"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        if (GetExitCodeThread(hSecThread[threadIndex], &exitCode))
        {
            if (exitCode == STILL_ACTIVE)
            {
                if (threadStates[threadIndex] == 1)
                    SetDlgItemText(hDlg, 1404, _T("Активный"));
                else
                    SetDlgItemText(hDlg, 1404, _T("Приостановленный"));
            }
            else
            {
                _stprintf(buffer, _T("Завершен (%lu)"), exitCode);
                SetDlgItemText(hDlg, 1404, buffer);
            }
        }
    }
    else
    {
        SetDlgItemText(hDlg, 1404, _T("Не создан"));
    }

    // Приоритет потока
    if (threadIndex == 0)
    {
        int priority = GetThreadPriority(GetCurrentThread());
        // ... обработка приоритета
    }
    else if (threadIndex == 3) // Поток анимации
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
            SetDlgItemText(hDlg, 1405, buffer);
        }
        else
        {
            SetDlgItemText(hDlg, 1405, _T("Не доступно"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        // ... существующая обработка для потоков 1 и 2
    }
    else
    {
        SetDlgItemText(hDlg, 1405, _T("Не доступно"));
    }

    // Время работы потока
    if (threadIndex == 0)
    {
        HANDLE hThread = GetCurrentThread();
        // ... обработка времени работы
    }
    else if (threadIndex == 3) // Поток анимации
    {
        if (hAnimationThread)
        {
            if (GetThreadTimes(hAnimationThread, &creationTime, &exitTime, &kernelTime, &userTime))
            {
                FileTimeToSystemTime(&creationTime, &sysTime);
                ULARGE_INTEGER ulCreation;
                ulCreation.LowPart = creationTime.dwLowDateTime;
                ulCreation.HighPart = creationTime.dwHighDateTime;

                FILETIME now;
                GetSystemTimeAsFileTime(&now);
                ULARGE_INTEGER ulNow;
                ulNow.LowPart = now.dwLowDateTime;
                ulNow.HighPart = now.dwHighDateTime;

                lifeTime = (ulNow.QuadPart - ulCreation.QuadPart) / 10000;
                _stprintf(buffer, _T("%llu мс"), lifeTime);
                SetDlgItemText(hDlg, 1406, buffer);
            }
            else
            {
                SetDlgItemText(hDlg, 1406, _T("Не доступно"));
            }
        }
        else
        {
            SetDlgItemText(hDlg, 1406, _T("Не доступно"));
        }
    }
    else if (hSecThread[threadIndex])
    {
        // ... существующая обработка для потоков 1 и 2
    }
    else
    {
        SetDlgItemText(hDlg, 1406, _T("Не доступно"));
    }
}