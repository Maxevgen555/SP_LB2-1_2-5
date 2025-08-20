#include "ProcessManager.h"  // Подключение заголовочного файла с определениями
#include <stdio.h>           // Стандартные функции ввода-вывода
#include <psapi.h>           // Функции для работы с процессами и модулями

// Указывает линкеру использовать оконный подсистему, точку входа wWinMainCRTStartup
// Это позволяет использовать Unicode версию WinMain (_tWinMain)

// ================= ИНИЦИАЛИЗАЦИЯ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ =================

// Дескриптор экземпляра приложения (будет установлен в WinMain)
HINSTANCE hInst;

// Имя класса главного окна приложения
TCHAR szClassName[] = _T("ProcessManagerApp");

// Заголовок главного окна с информацией о студенте и группе
TCHAR szTitle[100] = _T("SP-LB2-1 Гончаров М.Е. Гр. 40322");

// Массивы для хранения информации о процессах (4 процесса: 0-3)
HANDLE ProcHandle[4] = { NULL };        // Дескрипторы процессов
DWORD ProcId[4] = { 0 };                // Идентификаторы процессов (PID)
HANDLE ThreadHandle[4] = { NULL };      // Дескрипторы основных потоков
DWORD ThreadId[4] = { 0 };              // Идентификаторы потоков (TID)
LPTSTR ProcImage[4] = { NULL };         // Пути к исполняемым файлам
TCHAR CmdParam[4][260] = { {0} };       // Параметры командной строки

// ================= ТОЧКА ВХОДА ПРИЛОЖЕНИЯ =================

// Главная функция Windows приложения
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;  // Структура для хранения сообщений

    // Инициализация данных о процессах - заполнение массивов начальными значениями
    InitializeProcessData();

    // Регистрация класса окна - создание шаблона для главного окна
    if (!InitApplication(hInstance)) {
        return FALSE;  // Выход при ошибке регистрации
    }

    // Создание и отображение главного окна приложения
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;  // Выход при ошибке создания окна
    }

    // Основной цикл обработки сообщений приложения
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  // Преобразование сообщений клавиш
        DispatchMessage(&msg);   // Передача сообщений процедуре окна
    }

    // Возврат кода завершения приложения
    return (int)msg.wParam;
}

// ================= ИНИЦИАЛИЗАЦИЯ ДАННЫХ О ПРОЦЕССАХ =================

void InitializeProcessData() {
    // Текущий процесс (индекс 0) - само приложение
    ProcId[0] = GetCurrentProcessId();  // Получение PID текущего процесса
    // Открытие дескриптора текущего процесса с правами на запрос информации и чтение памяти
    ProcHandle[0] = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcId[0]);
    ThreadId[0] = GetCurrentThreadId();  // Получение TID текущего потока
    // Открытие дескриптора текущего потока с правами на запрос информации
    ThreadHandle[0] = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId[0]);

    // Получение пути к исполняемому файлу текущего процесса
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    ProcImage[0] = _tcsdup(path);  // Копирование строки в динамическую память

    // Получение и сохранение командной строки текущего процесса
    LPTSTR cmdLine = GetCommandLine();
    _tcscpy_s(CmdParam[0], _countof(CmdParam[0]), cmdLine);

    // Процесс Блокнот (индекс 1) - обычный блокнот без параметров
    ProcImage[1] = _tcsdup(_T("notepad.exe"));  // Путь к исполняемому файлу
    _tcscpy_s(CmdParam[1], _countof(CmdParam[1]), _T(""));  // Пустые параметры

    // Процесс Блокнот с текстом (индекс 2) - блокнот с открытием файла
    ProcImage[2] = _tcsdup(_T("notepad.exe"));
    _tcscpy_s(CmdParam[2], _countof(CmdParam[2]), _T("Test.txt"));  // Параметр - файл для открытия

    // Процесс Калькулятор (индекс 3) - стандартный калькулятор Windows
    ProcImage[3] = _tcsdup(_T("calc.exe"));
    _tcscpy_s(CmdParam[3], _countof(CmdParam[3]), _T(""));  // Без параметров
}

// ================= РЕГИСТРАЦИЯ КЛАССА ОКНА =================

BOOL InitApplication(HINSTANCE hInstance) {
    WNDCLASSEX wcex;  // Структура для описания класса окна

    // Заполнение структуры WNDCLASSEX
    wcex.cbSize = sizeof(WNDCLASSEX);          // Размер структуры
    wcex.style = CS_HREDRAW | CS_VREDRAW;      // Стиль - перерисовка при изменении размера
    wcex.lpfnWndProc = WndProc;                // Указатель на процедуру окна
    wcex.cbClsExtra = 0;                       // Дополнительная память для класса
    wcex.cbWndExtra = 0;                       // Дополнительная память для окна
    wcex.hInstance = hInstance;                // Дескриптор экземпляра
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // Стандартная иконка
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);    // Стандартный курсор
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // Цвет фона окна
    wcex.lpszMenuName = NULL;                  // Меню будет создано позже
    wcex.lpszClassName = szClassName;          // Имя класса окна
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);  // Маленькая иконка

    // Регистрация класса окна в системе
    return RegisterClassEx(&wcex);
}

// ================= СОЗДАНИЕ ГЛАВНОГО ОКНА =================

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hWnd;  // Дескриптор создаваемого окна

    hInst = hInstance;  // Сохранение дескриптора экземпляра

    // Создание главного окна приложения
    hWnd = CreateWindow(szClassName,           // Имя класса окна
        szTitle,               // Заголовок окна
        WS_OVERLAPPEDWINDOW,   // Стиль окна
        CW_USEDEFAULT,         // Позиция X по умолчанию
        CW_USEDEFAULT,         // Позиция Y по умолчанию
        800,                   // Ширина окна
        600,                   // Высота окна
        NULL,                  // Родительское окно
        NULL,                  // Меню
        hInstance,             // Дескриптор экземпляра
        NULL);                 // Дополнительные параметры

    if (!hWnd) {
        return FALSE;  // Выход при ошибке создания окна
    }

    // ================= СОЗДАНИЕ МЕНЮ =================

    HMENU hMenu = CreateMenu();                // Главное меню
    HMENU hProcessMenu = CreatePopupMenu();    // Подменю "Процессы"
    HMENU hInfoMenu = CreatePopupMenu();       // Подменю "Информация о процессах"

    // Добавление пунктов в меню "Процессы"
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_NOTEPAD, _T("Блокнот"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_NOTEPAD_TEXT, _T("Блокнот с текстом"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_CALC, _T("Калькулятор"));
    AppendMenu(hProcessMenu, MF_STRING, IDM_PROCESS_CLOSE_NOTEPAD, _T("Закрыть Блокнот"));

    // Добавление пунктов в меню "Информация о процессах"
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS0, _T("Текущий процесс(0)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS1, _T("Блокнот (1)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS2, _T("Блокнот с текстом (2)"));
    AppendMenu(hInfoMenu, MF_STRING, IDM_INFO_PROCESS3, _T("Калькулятор (3)"));

    // Добавление подменю в главное меню
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hProcessMenu, _T("Процессы"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hInfoMenu, _T("Информация о процессах"));

    // Установка меню для окна
    SetMenu(hWnd, hMenu);

    // Отображение и обновление окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// ================= ФУНКЦИЯ ПОИСКА И ЗАКРЫТИЯ БЛОКНОТА =================

BOOL FindAndCloseNotepad() {
    // Создание снимка всех процессов в системе
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;  // Ошибка создания снимка
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);  // Важно установить размер структуры

    // Получение информации о первом процессе в снимке
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return FALSE;  // Ошибка получения информации
    }

    BOOL found = FALSE;
    // Перебор всех процессов в снимке
    do {
        // Проверка имени процесса (без учета регистра)
        if (_tcsicmp(pe32.szExeFile, _T("notepad.exe")) == 0) {
            // Открытие процесса с правами на завершение
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                // Принудительное завершение процесса с кодом 40322 (номер группы)
                TerminateProcess(hProcess, 40322);
                CloseHandle(hProcess);
                found = TRUE;  // Процесс найден и закрыт
            }
        }
    } while (Process32Next(hSnapshot, &pe32));  // Переход к следующему процессу

    CloseHandle(hSnapshot);  // Закрытие handle снимка
    return found;
}

// ================= ЗАПУСК ПРОЦЕССА =================

BOOL StartProcess(int index) {
    PROCESS_INFORMATION pi;  // Информация о созданном процессе
    STARTUPINFO si;          // Начальная информация для процесса

    // Инициализация структур нулями
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);  // Размер структуры обязателен
    ZeroMemory(&pi, sizeof(pi));

    TCHAR commandLine[520];
    // Формирование командной строки
    if (_tcslen(CmdParam[index]) > 0) {
        // Если есть параметры - добавляем их в кавычках
        _stprintf_s(commandLine, _countof(commandLine), _T("\"%s\" %s"), ProcImage[index], CmdParam[index]);
    }
    else {
        // Без параметров - просто путь к exe
        _tcscpy_s(commandLine, _countof(commandLine), ProcImage[index]);
    }

    // Создание нового процесса
    if (!CreateProcess(NULL,           // Имя модуля (используется commandLine)
        commandLine,     // Командная строка
        NULL,            // Атрибуты безопасности процесса
        NULL,            // Атрибуты безопасности потока
        FALSE,           // Наследование handle
        0,               // Флаги создания
        NULL,            // Окружение
        NULL,            // Текущий каталог
        &si,             // STARTUPINFO
        &pi)) {          // PROCESS_INFORMATION
        // Обработка ошибки создания процесса
        DWORD error = GetLastError();
        TCHAR errorMsg[256];
        _stprintf_s(errorMsg, _countof(errorMsg), _T("Не удалось запустить процесс. Код ошибки: %lu"), error);
        MessageBox(NULL, errorMsg, _T("Ошибка"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Сохранение информации о созданном процессе
    ProcHandle[index] = pi.hProcess;    // Дескриптор процесса
    ProcId[index] = pi.dwProcessId;     // ID процесса
    ThreadHandle[index] = pi.hThread;   // Дескриптор основного потока
    ThreadId[index] = pi.dwThreadId;    // ID основного потока

    // Закрываем дескриптор потока - он не нужен для дальнейшей работы
    CloseHandle(pi.hThread);
    ThreadHandle[index] = NULL;

    return TRUE;
}

// ================= ПРОЦЕДУРА ГЛАВНОГО ОКНА =================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:  // Обработка команд меню
    {
        int wmId = LOWORD(wParam);  // ID команды меню
        switch (wmId) {
        case IDM_PROCESS_NOTEPAD:        // Запуск обычного блокнота
            StartProcess(1);
            break;
        case IDM_PROCESS_NOTEPAD_TEXT:   // Запуск блокнота с файлом
            StartProcess(2);
            break;
        case IDM_PROCESS_CALC:           // Запуск калькулятора
            StartProcess(3);
            break;
        case IDM_PROCESS_CLOSE_NOTEPAD:  // Закрытие всех блокнотов
            if (!FindAndCloseNotepad()) {
                MessageBox(hWnd, _T("Не удалось найти или закрыть Блокнот"), _T("Ошибка"), MB_OK | MB_ICONERROR);
            }
            else {
                // Сброс информации о процессе Блокнота (индекс 1)
                if (ProcHandle[1]) {
                    CloseHandle(ProcHandle[1]);
                    ProcHandle[1] = NULL;
                }
                ProcId[1] = 0;
                ThreadId[1] = 0;
            }
            break;
        case IDM_INFO_PROCESS0:  // Информация о текущем процессе
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 0);
            break;
        case IDM_INFO_PROCESS1:  // Информация о процессе Блокнота
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 1);
            break;
        case IDM_INFO_PROCESS2:  // Информация о процессе Блокнота с текстом
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 2);
            break;
        case IDM_INFO_PROCESS3:  // Информация о процессе Калькулятора
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PROCESS_INFO), hWnd, ProcessInfoDlgProc, 3);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:  // Закрытие окна
        // Очистка ресурсов - закрытие всех открытых дескрипторов
        for (int i = 0; i < 4; i++) {
            if (ProcHandle[i] && i != 0) {  // Не закрываем дескриптор текущего процесса
                DWORD exitCode;
                // Проверка активности процесса перед закрытием дескриптора
                if (GetExitCodeProcess(ProcHandle[i], &exitCode) && exitCode == STILL_ACTIVE) {
                    // Процесс еще активен - можно предложить пользователю закрыть его
                }
                CloseHandle(ProcHandle[i]);
            }
            if (ThreadHandle[i] && i != 0) {  // Не закрываем дескриптор текущего потока
                CloseHandle(ThreadHandle[i]);
            }
            if (ProcImage[i]) {
                free(ProcImage[i]);  // Освобождение памяти, выделенной под пути
            }
        }
        PostQuitMessage(0);  // Завершение цикла сообщений
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ================= ПРОЦЕДУРА ДИАЛОГОВОГО ОКНА =================

INT_PTR CALLBACK ProcessInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int index;  // Сохранение индекса процесса между вызовами

    switch (message) {
    case WM_INITDIALOG:  // Инициализация диалогового окна
        index = (int)lParam;  // Получение индекса процесса из параметра
        FillProcessInfo(hDlg, index);  // Заполнение окна информацией
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {  // Нажатие кнопки OK
            EndDialog(hDlg, 0);  // Закрытие диалогового окна
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ================= ЗАПОЛНЕНИЕ ИНФОРМАЦИИ О ПРОЦЕССЕ =================

void FillProcessInfo(HWND hDlg, int index) {
    HANDLE hProcess = ProcHandle[index];    // Дескриптор процесса
    HANDLE hThread = ThreadHandle[index];   // Дескриптор потока
    BOOL needCloseProcess = FALSE;          // Флаг для временно открытых дескрипторов
    BOOL needCloseThread = FALSE;
    BOOL processActive = FALSE;             // Флаг активности процесса

    // Специальная обработка для процесса Блокнота (индекс 1)
    if (index == 1 && hProcess == NULL && ProcId[1] != 0) {
        // Попытка открыть дескриптор процесса Блокнота, если он был закрыт
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcId[1]);
        if (hProcess) {
            needCloseProcess = TRUE;
            // Проверка активности процесса
            DWORD exitCode;
            if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                ProcHandle[1] = hProcess;      // Сохранение дескриптора
                needCloseProcess = FALSE;      // Не закрывать - сохранили в массиве
            }
        }
    }

    // Аналогичная обработка для дескриптора потока
    if (index == 1 && hThread == NULL && ThreadId[1] != 0) {
        hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId[1]);
        if (hThread) {
            needCloseThread = TRUE;
        }
    }

    TCHAR buffer[260];  // Буфер для форматирования строк

    // Заполнение полей диалогового окна информацией о процессе

    // Имя исполняемого файла
    SetDlgItemText(hDlg, IDC_EDIT_FILENAME, ProcImage[index]);

    // Параметры командной строки
    SetDlgItemText(hDlg, IDC_EDIT_PARAMS, CmdParam[index]);

    // Дескриптор процесса в шестнадцатеричном формате
    _stprintf_s(buffer, _countof(buffer), _T("0x%p"), hProcess);
    SetDlgItemText(hDlg, IDC_EDIT_PROC_HANDLE, buffer);

    // ID процесса (PID)
    _stprintf_s(buffer, _countof(buffer), _T("%lu"), ProcId[index]);
    SetDlgItemText(hDlg, IDC_EDIT_PROC_ID, buffer);

    // Дескриптор потока
    _stprintf_s(buffer, _countof(buffer), _T("0x%p"), hThread);
    SetDlgItemText(hDlg, IDC_EDIT_THREAD_HANDLE, buffer);

    // ID потока (TID)
    _stprintf_s(buffer, _countof(buffer), _T("%lu"), ThreadId[index]);
    SetDlgItemText(hDlg, IDC_EDIT_THREAD_ID, buffer);

    // Код завершения процесса
    DWORD exitCode;
    if (hProcess && GetExitCodeProcess(hProcess, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, _T("Состояние - Активен"));
            processActive = TRUE;  // Процесс активен
        }
        else {
            _stprintf_s(buffer, _countof(buffer), _T("%lu"), exitCode);
            SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, buffer);
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_PROC_EXIT_CODE, _T("Не доступно"));
    }

    // Код завершения потока
    if (hThread && GetExitCodeThread(hThread, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, _T("Состояние - Активен"));
        }
        else {
            _stprintf_s(buffer, _countof(buffer), _T("%lu"), exitCode);
            SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, buffer);
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_THREAD_EXIT_CODE, _T("Не доступно"));
    }

    // Класс приоритета процесса
    if (hProcess) {
        DWORD priorityClass = GetPriorityClass(hProcess);
        if (priorityClass) {
            switch (priorityClass) {
            case IDLE_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("IDLE"));
                break;
            case NORMAL_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("NORMAL"));
                break;
            case HIGH_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("HIGH"));
                break;
            case REALTIME_PRIORITY_CLASS:
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("REALTIME"));
                break;
            default:
                _stprintf_s(buffer, _countof(buffer), _T("%lu"), priorityClass);
                SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, buffer);
                break;
            }
        }
        else {
            SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("Не доступно"));
        }
    }
    else {
        SetDlgItemText(hDlg, IDC_EDIT_PRIORITY_CLASS, _T("Не доступно"));
    }

    // Временные характеристики процесса (только для активных процессов)
    if (hProcess && processActive) {
        FILETIME createTime, exitTime, kernelTime, userTime;
        if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
            // Преобразование FILETIME в 64-битные значения для вычислений
            ULARGE_INTEGER ulCreateTime, ulKernelTime, ulUserTime;

            ulCreateTime.LowPart = createTime.dwLowDateTime;
            ulCreateTime.HighPart = createTime.dwHighDateTime;

            ulKernelTime.LowPart = kernelTime.dwLowDateTime;
            ulKernelTime.HighPart = kernelTime.dwHighDateTime;

            ulUserTime.LowPart = userTime.dwLowDateTime;
            ulUserTime.HighPart = userTime.dwHighDateTime;

            // Получение текущего системного времени
            FILETIME now;
            GetSystemTimeAsFileTime(&now);
            ULARGE_INTEGER ulNow;
            ulNow.LowPart = now.dwLowDateTime;
            ulNow.HighPart = now.dwHighDateTime;

            // Вычисление времени жизни процесса (в миллисекундах)
            ULONGLONG lifeTime = (ulNow.QuadPart - ulCreateTime.QuadPart) / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu мс"), lifeTime);
            SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, buffer);

            // Время выполнения в режиме пользователя
            ULONGLONG userTimeMs = ulUserTime.QuadPart / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu мс"), userTimeMs);
            SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, buffer);

            // Время выполнения в режиме ядра
            ULONGLONG kernelTimeMs = ulKernelTime.QuadPart / 10000;
            _stprintf_s(buffer, _countof(buffer), _T("%llu мс"), kernelTimeMs);
            SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, buffer);

            // Время простоя (общее время минус время CPU)
            ULONGLONG idleTime = lifeTime - (userTimeMs + kernelTimeMs);
            _stprintf_s(buffer, _countof(buffer), _T("%llu мс"), idleTime);
            SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, buffer);
        }
        else {
            // Ошибка получения временных характеристик
            SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, _T("Процесс не активен"));
            SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, _T("Процесс не активен"));
            SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, _T("Процесс не активен"));
            SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, _T("Процесс не активен"));
        }
    }
    else {
        // Процесс не активен или недоступен
        SetDlgItemText(hDlg, IDC_EDIT_LIFE_TIME, _T("Процесс не активен"));
        SetDlgItemText(hDlg, IDC_EDIT_USER_TIME, _T("Процесс не активен"));
        SetDlgItemText(hDlg, IDC_EDIT_KERNEL_TIME, _T("Процесс не активен"));
        SetDlgItemText(hDlg, IDC_EDIT_IDLE_TIME, _T("Процесс не активен"));
    }

    // Закрытие временно открытых дескрипторов
    if (needCloseProcess) {
        CloseHandle(hProcess);
    }
    if (needCloseThread) {
        CloseHandle(hThread);
    }
}