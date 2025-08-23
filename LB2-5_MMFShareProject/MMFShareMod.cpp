#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"

// Глобальные переменные для хранения дескриптора проекции и указателя на данные
static HANDLE s_hFileMap = NULL;
static PBYTE s_pbData = NULL;
// Уникальное имя разделяемого объекта
static const TCHAR s_szMapName[] = TEXT("MMFSharedDataMod");

// Прототипы функций
BOOL CALLBACK Dlg_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void CleanUp();
BOOL CreateMapping(HWND hwnd);

// Точка входа
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPTSTR lpCmdLine, int nCmdShow) {
    DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)Dlg_Proc);
    return 0;
}

// Главная процедура диалога
BOOL CALLBACK Dlg_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        HANDLE_MSG(hDlg, WM_INITDIALOG, Dlg_OnInitDialog);
        HANDLE_MSG(hDlg, WM_COMMAND, Dlg_OnCommand);
    case WM_CLOSE:
        CleanUp();
        EndDialog(hDlg, 0);
        return TRUE;
    }
    return FALSE;
}

// Функция очистки ресурсов
void CleanUp() {
    if (s_pbData != NULL) {
        UnmapViewOfFile(s_pbData);
        s_pbData = NULL;
    }
    if (s_hFileMap != NULL) {
        CloseHandle(s_hFileMap);
        s_hFileMap = NULL;
    }
}

// Обработчик инициализации диалога
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
    // Загрузка иконки
    HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MMFSHARE));
    if (hIcon) {
        SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hIcon);
    }

    // Инициализация поля ввода
    Edit_SetText(GetDlgItem(hwnd, IDC_DATA), TEXT("Данные для обмена."));

    // Автоматическое создание проекции при запуске
    if (CreateMapping(hwnd)) {
        // Обновляем состояние кнопок при успешном создании
        Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), FALSE);  // Отключаем "Создать"
        Button_Enable(GetDlgItem(hwnd, IDC_OPENFILE), FALSE);    // Отключаем "Открыть"
        Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE), TRUE);    // Включаем "Закрыть"
        Button_Enable(GetDlgItem(hwnd, IDC_WRITE), TRUE);        // Включаем "Записать"
        Button_Enable(GetDlgItem(hwnd, IDC_READ), TRUE);         // Включаем "Прочитать"
    }
    else {
        // Если не удалось создать, оставляем кнопку "Создать" активной
        Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), TRUE);
        Button_Enable(GetDlgItem(hwnd, IDC_OPENFILE), TRUE);
        Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE), FALSE);
        Button_Enable(GetDlgItem(hwnd, IDC_WRITE), FALSE);
        Button_Enable(GetDlgItem(hwnd, IDC_READ), FALSE);
    }

    return TRUE;
}
//BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
//    // Загрузка иконки
//    HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);
//    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MMFSHARE));
//    if (hIcon) {
//        SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hIcon);
//    }
//
//    // Инициализация поля ввода
//    Edit_SetText(GetDlgItem(hwnd, IDC_DATA), TEXT("Данные для обмена."));
//
//    // АВТОМАТИЧЕСКОЕ СОЗДАНИЕ/ОТКРЫТИЕ ПРОЕКЦИИ ПРИ ЗАПУСКЕ
//    // Пытаемся создать объект проецируемого файла
//    s_hFileMap = CreateFileMapping(
//        INVALID_HANDLE_VALUE,    // Используем файл подкачки (ИСПРАВЛЕНО!)
//        NULL,                   // Атрибуты безопасности по умолчанию
//        PAGE_READWRITE,         // Чтение и запись
//        0,                      // Старшие 32 бита размера
//        4 * 1024,               // Младшие 32 бита размера (4 КБ)
//        s_szMapName);           // Имя объекта
//
//    if (s_hFileMap == NULL) {
//        DWORD dwError = GetLastError();
//        TCHAR szError[256];
//        _stprintf_s(szError, TEXT("Ошибка создания объекта проецируемого файла! Код: %d"), dwError);
//        MessageBox(hwnd, szError, TEXT("Ошибка"), MB_ICONERROR);
//        return TRUE;
//    }
//
//    // Проверяем, существовал ли объект до этого
//    if (GetLastError() == ERROR_ALREADY_EXISTS) {
//        // Объект уже существует - мы подключаемся
//        MessageBox(hwnd, TEXT("Подключение к существующей общей памяти."), TEXT("Информация"), MB_ICONINFORMATION);
//    }
//    else {
//        // Объект создан впервые - мы первый процесс
//        MessageBox(hwnd, TEXT("Создана новая область общей памяти."), TEXT("Информация"), MB_ICONINFORMATION);
//    }
//
//    // Проецируем файл в память
//    s_pbData = (PBYTE)MapViewOfFile(
//        s_hFileMap,             // Дескриптор объекта проецируемого файла
//        FILE_MAP_READ | FILE_MAP_WRITE, // Доступ на чтение и запись
//        0, 0,                   // Смещение от начала файла
//        0);                     // Проецируем весь файл
//
//    if (s_pbData == NULL) {
//        DWORD dwError = GetLastError();
//        TCHAR szError[256];
//        _stprintf_s(szError, TEXT("Ошибка проецирования файла в память! Код: %d"), dwError);
//        MessageBox(hwnd, szError, TEXT("Ошибка"), MB_ICONERROR);
//        CloseHandle(s_hFileMap);
//        s_hFileMap = NULL;
//        return TRUE;
//    }
//
//    // Если мы создали объект (а не подключились к существующему),
//    // инициализируем общую память начальными данными
//    if (GetLastError() != ERROR_ALREADY_EXISTS) {
//        Edit_GetText(GetDlgItem(hwnd, IDC_DATA), (LPTSTR)s_pbData, 4 * 1024);
//    }
//
//    // Обновляем состояние кнопок
//    Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), FALSE);  // Отключаем "Создать"
//    Button_Enable(GetDlgItem(hwnd, IDC_OPENFILE), FALSE);    // Отключаем "Открыть"
//    Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE), TRUE);    // Включаем "Закрыть"
//    Button_Enable(GetDlgItem(hwnd, IDC_WRITE), TRUE);        // Включаем "Записать"
//    Button_Enable(GetDlgItem(hwnd, IDC_READ), TRUE);         // Включаем "Прочитать"
//
//    return TRUE;
//}
// Обработчик команд
void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
    switch (id) {
    case IDC_CREATEFILE: // Теперь РАБОТАЕТ!
        if (codeNotify == BN_CLICKED) {
            if (CreateMapping(hwnd)) {
                Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), FALSE);
                Button_Enable(GetDlgItem(hwnd, IDC_OPENFILE), FALSE);
                Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE), TRUE);
                Button_Enable(GetDlgItem(hwnd, IDC_WRITE), TRUE);
                Button_Enable(GetDlgItem(hwnd, IDC_READ), TRUE);
            }
        }
        break;

    case IDC_CLOSEFILE:
        if (codeNotify == BN_CLICKED) {
            CleanUp();
            Button_Enable(GetDlgItem(hwnd, IDC_CREATEFILE), TRUE);  // Включаем "Создать"
            Button_Enable(GetDlgItem(hwnd, IDC_OPENFILE), TRUE);    // Включаем "Открыть"
            Button_Enable(GetDlgItem(hwnd, IDC_CLOSEFILE), FALSE);  // Отключаем "Закрыть"
            Button_Enable(GetDlgItem(hwnd, IDC_WRITE), FALSE);      // Отключаем "Записать"
            Button_Enable(GetDlgItem(hwnd, IDC_READ), FALSE);       // Отключаем "Прочитать"
            MessageBox(hwnd, TEXT("Проекция закрыта. Можно создать новую."), TEXT("Информация"), MB_ICONINFORMATION);
        }
        break;

    case IDC_OPENFILE: // Можно оставить отключенной или реализовать
        break;

        // НОВАЯ КНОПКА: ЗАПИСЬ
    case IDC_WRITE:
        if (codeNotify == BN_CLICKED) {
            if (s_pbData != NULL) {
                // Копируем текст из поля ввода в общую память
                Edit_GetText(GetDlgItem(hwnd, IDC_DATA), (LPTSTR)s_pbData, 4 * 1024);
                MessageBox(hwnd, TEXT("Данные записаны в общую память."), TEXT("Запись"), MB_ICONINFORMATION);
            }
            else {
                MessageBox(hwnd, TEXT("Нет доступа к общей памяти!"), TEXT("Ошибка"), MB_ICONERROR);
            }
        }
        break;

        // НОВАЯ КНОПКА: ЧТЕНИЕ
    case IDC_READ:
        if (codeNotify == BN_CLICKED) {
            if (s_pbData != NULL) {
                // Копируем текст из общей памяти в поле ввода
                Edit_SetText(GetDlgItem(hwnd, IDC_DATA), (LPTSTR)s_pbData);
                MessageBox(hwnd, TEXT("Данные прочитаны из общей памяти."), TEXT("Чтение"), MB_ICONINFORMATION);
            }
            else {
                MessageBox(hwnd, TEXT("Нет доступа к общей памяти!"), TEXT("Ошибка"), MB_ICONERROR);
            }
        }
        break;

    case IDCANCEL:
        CleanUp();
        EndDialog(hwnd, id);
        break;
    }
}

// Функция для создания/открытия проекции
BOOL CreateMapping(HWND hwnd) {
    // Пытаемся создать объект проецируемого файла
    s_hFileMap = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // Используем файл подкачки
        NULL,                   // Атрибуты безопасности по умолчанию
        PAGE_READWRITE,         // Чтение и запись
        0,                      // Старшие 32 бита размера
        4 * 1024,               // Младшие 32 бита размера (4 КБ)
        s_szMapName);           // Имя объекта

    if (s_hFileMap == NULL) {
        DWORD dwError = GetLastError();
        TCHAR szError[256];
        _stprintf_s(szError, TEXT("Ошибка создания объекта проецируемого файла! Код: %d"), dwError);
        MessageBox(hwnd, szError, TEXT("Ошибка"), MB_ICONERROR);
        return FALSE;
    }

    // Проверяем, существовал ли объект до этого
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(hwnd, TEXT("Подключение к существующей общей памяти."), TEXT("Информация"), MB_ICONINFORMATION);
    }
    else {
        MessageBox(hwnd, TEXT("Создана новая область общей памяти."), TEXT("Информация"), MB_ICONINFORMATION);
    }

    // Проецируем файл в память
    s_pbData = (PBYTE)MapViewOfFile(
        s_hFileMap,             // Дескриптор объекта проецируемого файла
        FILE_MAP_READ | FILE_MAP_WRITE, // Доступ на чтение и запись
        0, 0,                   // Смещение от начала файла
        0);                     // Проецируем весь файл

    if (s_pbData == NULL) {
        DWORD dwError = GetLastError();
        TCHAR szError[256];
        _stprintf_s(szError, TEXT("Ошибка проецирования файла в память! Код: %d"), dwError);
        MessageBox(hwnd, szError, TEXT("Ошибка"), MB_ICONERROR);
        CloseHandle(s_hFileMap);
        s_hFileMap = NULL;
        return FALSE;
    }

    // Если мы создали объект (а не подключились к существующему),
    // инициализируем общую память начальными данными
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        Edit_GetText(GetDlgItem(hwnd, IDC_DATA), (LPTSTR)s_pbData, 4 * 1024);
    }

    return TRUE;
}