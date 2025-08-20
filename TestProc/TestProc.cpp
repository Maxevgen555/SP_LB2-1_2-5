#include "TestProc.h"
#include <string>
#include <sstream>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")

// Глобальные переменные приложения
HINSTANCE g_hInstance;          // Дескриптор экземпляра приложения
HWND g_hEditInput;              // Дескриптор поля ввода текста
HWND g_hStaticText;             // Дескриптор статического текстового поля
int g_clickCount = 0;           // Счетчик нажатий кнопки
std::wstring g_displayText = L"Добро пожаловать в TestProc!"; // Текст для отображения

// Точка входа приложения - функция wWinMain для Unicode приложения
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    // Сохраняем дескриптор экземпляра приложения в глобальной переменной
    g_hInstance = hInstance;

    // Параметры по умолчанию для окна
    int x = CW_USEDEFAULT;      // Позиция X по умолчанию (Windows сама выберет)
    int y = CW_USEDEFAULT;      // Позиция Y по умолчанию
    int width = 400;            // Ширина окна по умолчанию
    int height = 300;           // Высота окна по умолчанию

    // Парсим параметры командной строки, если они переданы
    // Ожидаемый формат: "x y width height"
    if (wcslen(lpCmdLine) > 0) {
        swscanf_s(lpCmdLine, L"%d %d %d %d", &x, &y, &width, &height);
    }

    // Регистрируем класс окна в системе
    if (!RegisterWindowClass(hInstance))
    {
        MessageBox(NULL, L"Ошибка регистрации класса окна", L"Ошибка", MB_ICONERROR);
        return 1; // Возвращаем код ошибки
    }

    // Создаем главное окно с указанными параметрами позиции и размера
    HWND hWnd = CreateMainWindow(hInstance, nCmdShow, x, y, width, height);
    if (!hWnd)
    {
        MessageBox(NULL, L"Ошибка создания окна", L"Ошибка", MB_ICONERROR);
        return 1; // Возвращаем код ошибки
    }

    // Основной цикл обработки сообщений приложения
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);  // Преобразуем сообщения клавиш
        DispatchMessage(&msg);   // Передаем сообщения оконной процедуре
    }

    // Возвращаем код завершения из параметра wParam последнего сообщения
    return (int)msg.wParam;
}

// Функция регистрации класса окна в системе Windows
BOOL RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex; // Структура для описания класса окна

    // Заполняем структуру WNDCLASSEX
    wcex.cbSize = sizeof(WNDCLASSEX);           // Размер структуры
    wcex.style = CS_HREDRAW | CS_VREDRAW;       // Стиль - перерисовка при изменении размера
    wcex.lpfnWndProc = WndProc;                 // Указатель на оконную процедуру
    wcex.cbClsExtra = 0;                        // Дополнительная память для класса
    wcex.cbWndExtra = 0;                        // Дополнительная память для окна
    wcex.hInstance = hInstance;                 // Дескриптор экземпляра приложения
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Стандартная иконка
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   // Стандартный курсор
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Цвет фона окна
    wcex.lpszMenuName = NULL;                   // Нет меню
    wcex.lpszClassName = L"TestProcClass";      // Имя класса окна
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Маленькая иконка

    // Регистрируем класс и возвращаем результат
    return RegisterClassEx(&wcex);
}

// Функция создания главного окна приложения
HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, int x, int y, int width, int height)
{
    // Создаем окно с указанными параметрами
    HWND hWnd = CreateWindowEx(
        0,                               // Расширенный стиль окна
        L"TestProcClass",                // Имя класса окна
        L"TestProc - Тестовое приложение", // Заголовок окна
        WS_OVERLAPPEDWINDOW,             // Стиль окна - стандартное перекрывающееся окно
        x, y,                            // Позиция окна (X, Y)
        width, height,                   // Размер окна (ширина, высота)
        NULL,                            // Родительское окно (нет)
        NULL,                            // Меню (нет)
        hInstance,                       // Дескриптор экземпляра приложения
        NULL                             // Дополнительные параметры
    );

    // Если окно успешно создано, показываем и обновляем его
    if (hWnd)
    {
        ShowWindow(hWnd, nCmdShow);  // Показываем окно
        UpdateWindow(hWnd);          // Обновляем окно (вызываем WM_PAINT)
    }

    return hWnd; // Возвращаем дескриптор созданного окна
}

// Оконная процедура - обрабатывает все сообщения для окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Обрабатываем сообщения в зависимости от их типа
    switch (message)
    {
    case WM_CREATE:      // Сообщение создания окна
        OnCreate(hwnd);  // Вызываем обработчик создания
        break;

    case WM_COMMAND:     // Сообщение команды (кнопки, меню и т.д.)
        // wParam содержит ID элемента и код уведомления
        // lParam содержит дескриптор элемента управления
        OnCommand(hwnd, LOWORD(wParam), (HWND)lParam, HIWORD(wParam));
        break;

    case WM_PAINT:       // Сообщение перерисовки окна
        OnPaint(hwnd);   // Вызываем обработчик перерисовки
        break;

    case WM_DESTROY:     // Сообщение уничтожения окна
        OnDestroy(hwnd); // Вызываем обработчик уничтожения
        break;

    default:
        // Все остальные сообщения передаем стандартной обработке Windows
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0; // Возвращаем 0 для обработанных сообщений
}

// Обработчик сообщения WM_CREATE - создание элементов управления
void OnCreate(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);      // Получаем размеры клиентской области
    int clientWidth = rect.right - rect.left; // Вычисляем ширину клиентской области

    // Создаем статический текст "Введите текст:"
    CreateWindow(
        L"STATIC",                   // Класс элемента - статический текст
        L"Введите текст:",           // Текст элемента
        WS_CHILD | WS_VISIBLE | SS_LEFT, // Стиль - дочерний, видимый, выравнивание по левому краю
        20, 20,                      // Позиция (X, Y)
        100, 20,                     // Размер (ширина, высота)
        hwnd,                        // Родительское окно
        NULL,                        // Идентификатор (не нужен)
        g_hInstance,                 // Дескриптор экземпляра
        NULL                         // Дополнительные параметры
    );

    // Создаем поле ввода текста
    g_hEditInput = CreateWindow(
        L"EDIT",                     // Класс элемента - поле редактирования
        L"",                         // Начальный текст (пустой)
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, // Стили: дочерний, видимый, с рамкой, выравнивание по левому краю
        130, 20,                     // Позиция (X, Y)
        clientWidth - 150, 20,       // Размер (ширина адаптивная, высота)
        hwnd,                        // Родительское окно
        (HMENU)IDC_EDIT_INPUT,       // Идентификатор элемента
        g_hInstance,                 // Дескриптор экземпляра
        NULL                         // Дополнительные параметры
    );

    // Создаем кнопку по центру окна
    int buttonWidth = 100;           // Ширина кнопки
    int buttonX = (clientWidth - buttonWidth) / 2; // Позиция X для центрирования
    CreateWindow(
        L"BUTTON",                   // Класс элемента - кнопка
        L"Нажми меня!",              // Текст на кнопке
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Стили: дочерний, видимый, кнопка
        buttonX, 50,                 // Позиция (X, Y)
        buttonWidth, 30,             // Размер (ширина, высота)
        hwnd,                        // Родительское окно
        (HMENU)IDC_BUTTON_CLICK,     // Идентификатор элемента
        g_hInstance,                 // Дескриптор экземпляра
        NULL                         // Дополнительные параметры
    );

    // Создаем статическое текстовое поле для отображения информации
    g_hStaticText = CreateWindow(
        L"STATIC",                   // Класс элемента - статический текст
        g_displayText.c_str(),       // Начальный текст
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER, // Стили
        20, 100,                     // Позиция (X, Y)
        clientWidth - 40, 100,       // Размер (ширина адаптивная, высота)
        hwnd,                        // Родительское окно
        (HMENU)IDC_STATIC_TEXT,      // Идентификатор элемента
        g_hInstance,                 // Дескриптор экземпляра
        NULL                         // Дополнительные параметры
    );
}

// Обработчик сообщения WM_COMMAND - обработка команд от элементов управления
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    // Обрабатываем команды в зависимости от ID элемента
    switch (id)
    {
    case IDC_BUTTON_CLICK: // Обработка нажатия кнопки
    {
        g_clickCount++; // Увеличиваем счетчик нажатий

        // Получаем текст из поля ввода
        wchar_t buffer[256]; // Буфер для текста
        GetWindowText(g_hEditInput, buffer, 256); // Читаем текст из поля ввода

        // Формируем строку для отображения с использованием stringstream
        std::wstringstream ss;
        ss << L"Кнопка нажата: " << g_clickCount << L" раз(а)\n";
        ss << L"Введенный текст: " << buffer;

        // Сохраняем сформированный текст
        g_displayText = ss.str();

        // Обновляем текст в статическом поле
        SetWindowText(g_hStaticText, g_displayText.c_str());

        // Запрашиваем перерисовку окна
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    }
}

// Обработчик сообщения WM_PAINT - перерисовка окна
void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps); // Начинаем перерисовку

    // Получаем размеры клиентской области
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Создаем кисть для фона светло-серого цвета
    HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &rect, hBrush);    // Заливаем фон
    DeleteObject(hBrush);            // Удаляем кисть

    // Рисуем информационную строку внизу окна
    rect.top = rect.bottom - 30;     // Устанавливаем верхнюю границу для текста
    SetTextColor(hdc, RGB(0, 0, 255)); // Устанавливаем синий цвет текста
    SetBkMode(hdc, TRANSPARENT);     // Устанавливаем прозрачный фон для текста

    // Выводим текст по центру внизу окна
    DrawText(hdc,
        L"TestProc v1.0 - Простое тестовое приложение", // Текст
        -1,                    // Длина текста (автоматически)
        &rect,                 // Область для вывода
        DT_CENTER | DT_VCENTER // Выравнивание по центру по горизонтали и вертикали
    );

    EndPaint(hwnd, &ps); // Заканчиваем перерисовку
}

// Обработчик сообщения WM_DESTROY - закрытие окна
void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0); // Отправляем сообщение о выходе из цикла сообщений
}