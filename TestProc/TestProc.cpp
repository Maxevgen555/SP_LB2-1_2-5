#include "TestProc.h"
#include <string>
#include <sstream>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")

// ���������� ���������� ����������
HINSTANCE g_hInstance;          // ���������� ���������� ����������
HWND g_hEditInput;              // ���������� ���� ����� ������
HWND g_hStaticText;             // ���������� ������������ ���������� ����
int g_clickCount = 0;           // ������� ������� ������
std::wstring g_displayText = L"����� ���������� � TestProc!"; // ����� ��� �����������

// ����� ����� ���������� - ������� wWinMain ��� Unicode ����������
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    // ��������� ���������� ���������� ���������� � ���������� ����������
    g_hInstance = hInstance;

    // ��������� �� ��������� ��� ����
    int x = CW_USEDEFAULT;      // ������� X �� ��������� (Windows ���� �������)
    int y = CW_USEDEFAULT;      // ������� Y �� ���������
    int width = 400;            // ������ ���� �� ���������
    int height = 300;           // ������ ���� �� ���������

    // ������ ��������� ��������� ������, ���� ��� ��������
    // ��������� ������: "x y width height"
    if (wcslen(lpCmdLine) > 0) {
        swscanf_s(lpCmdLine, L"%d %d %d %d", &x, &y, &width, &height);
    }

    // ������������ ����� ���� � �������
    if (!RegisterWindowClass(hInstance))
    {
        MessageBox(NULL, L"������ ����������� ������ ����", L"������", MB_ICONERROR);
        return 1; // ���������� ��� ������
    }

    // ������� ������� ���� � ���������� ����������� ������� � �������
    HWND hWnd = CreateMainWindow(hInstance, nCmdShow, x, y, width, height);
    if (!hWnd)
    {
        MessageBox(NULL, L"������ �������� ����", L"������", MB_ICONERROR);
        return 1; // ���������� ��� ������
    }

    // �������� ���� ��������� ��������� ����������
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);  // ����������� ��������� ������
        DispatchMessage(&msg);   // �������� ��������� ������� ���������
    }

    // ���������� ��� ���������� �� ��������� wParam ���������� ���������
    return (int)msg.wParam;
}

// ������� ����������� ������ ���� � ������� Windows
BOOL RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex; // ��������� ��� �������� ������ ����

    // ��������� ��������� WNDCLASSEX
    wcex.cbSize = sizeof(WNDCLASSEX);           // ������ ���������
    wcex.style = CS_HREDRAW | CS_VREDRAW;       // ����� - ����������� ��� ��������� �������
    wcex.lpfnWndProc = WndProc;                 // ��������� �� ������� ���������
    wcex.cbClsExtra = 0;                        // �������������� ������ ��� ������
    wcex.cbWndExtra = 0;                        // �������������� ������ ��� ����
    wcex.hInstance = hInstance;                 // ���������� ���������� ����������
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // ����������� ������
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);   // ����������� ������
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ���� ���� ����
    wcex.lpszMenuName = NULL;                   // ��� ����
    wcex.lpszClassName = L"TestProcClass";      // ��� ������ ����
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // ��������� ������

    // ������������ ����� � ���������� ���������
    return RegisterClassEx(&wcex);
}

// ������� �������� �������� ���� ����������
HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, int x, int y, int width, int height)
{
    // ������� ���� � ���������� �����������
    HWND hWnd = CreateWindowEx(
        0,                               // ����������� ����� ����
        L"TestProcClass",                // ��� ������ ����
        L"TestProc - �������� ����������", // ��������� ����
        WS_OVERLAPPEDWINDOW,             // ����� ���� - ����������� ��������������� ����
        x, y,                            // ������� ���� (X, Y)
        width, height,                   // ������ ���� (������, ������)
        NULL,                            // ������������ ���� (���)
        NULL,                            // ���� (���)
        hInstance,                       // ���������� ���������� ����������
        NULL                             // �������������� ���������
    );

    // ���� ���� ������� �������, ���������� � ��������� ���
    if (hWnd)
    {
        ShowWindow(hWnd, nCmdShow);  // ���������� ����
        UpdateWindow(hWnd);          // ��������� ���� (�������� WM_PAINT)
    }

    return hWnd; // ���������� ���������� ���������� ����
}

// ������� ��������� - ������������ ��� ��������� ��� ����
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ������������ ��������� � ����������� �� �� ����
    switch (message)
    {
    case WM_CREATE:      // ��������� �������� ����
        OnCreate(hwnd);  // �������� ���������� ��������
        break;

    case WM_COMMAND:     // ��������� ������� (������, ���� � �.�.)
        // wParam �������� ID �������� � ��� �����������
        // lParam �������� ���������� �������� ����������
        OnCommand(hwnd, LOWORD(wParam), (HWND)lParam, HIWORD(wParam));
        break;

    case WM_PAINT:       // ��������� ����������� ����
        OnPaint(hwnd);   // �������� ���������� �����������
        break;

    case WM_DESTROY:     // ��������� ����������� ����
        OnDestroy(hwnd); // �������� ���������� �����������
        break;

    default:
        // ��� ��������� ��������� �������� ����������� ��������� Windows
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0; // ���������� 0 ��� ������������ ���������
}

// ���������� ��������� WM_CREATE - �������� ��������� ����������
void OnCreate(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);      // �������� ������� ���������� �������
    int clientWidth = rect.right - rect.left; // ��������� ������ ���������� �������

    // ������� ����������� ����� "������� �����:"
    CreateWindow(
        L"STATIC",                   // ����� �������� - ����������� �����
        L"������� �����:",           // ����� ��������
        WS_CHILD | WS_VISIBLE | SS_LEFT, // ����� - ��������, �������, ������������ �� ������ ����
        20, 20,                      // ������� (X, Y)
        100, 20,                     // ������ (������, ������)
        hwnd,                        // ������������ ����
        NULL,                        // ������������� (�� �����)
        g_hInstance,                 // ���������� ����������
        NULL                         // �������������� ���������
    );

    // ������� ���� ����� ������
    g_hEditInput = CreateWindow(
        L"EDIT",                     // ����� �������� - ���� ��������������
        L"",                         // ��������� ����� (������)
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, // �����: ��������, �������, � ������, ������������ �� ������ ����
        130, 20,                     // ������� (X, Y)
        clientWidth - 150, 20,       // ������ (������ ����������, ������)
        hwnd,                        // ������������ ����
        (HMENU)IDC_EDIT_INPUT,       // ������������� ��������
        g_hInstance,                 // ���������� ����������
        NULL                         // �������������� ���������
    );

    // ������� ������ �� ������ ����
    int buttonWidth = 100;           // ������ ������
    int buttonX = (clientWidth - buttonWidth) / 2; // ������� X ��� �������������
    CreateWindow(
        L"BUTTON",                   // ����� �������� - ������
        L"����� ����!",              // ����� �� ������
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // �����: ��������, �������, ������
        buttonX, 50,                 // ������� (X, Y)
        buttonWidth, 30,             // ������ (������, ������)
        hwnd,                        // ������������ ����
        (HMENU)IDC_BUTTON_CLICK,     // ������������� ��������
        g_hInstance,                 // ���������� ����������
        NULL                         // �������������� ���������
    );

    // ������� ����������� ��������� ���� ��� ����������� ����������
    g_hStaticText = CreateWindow(
        L"STATIC",                   // ����� �������� - ����������� �����
        g_displayText.c_str(),       // ��������� �����
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER, // �����
        20, 100,                     // ������� (X, Y)
        clientWidth - 40, 100,       // ������ (������ ����������, ������)
        hwnd,                        // ������������ ����
        (HMENU)IDC_STATIC_TEXT,      // ������������� ��������
        g_hInstance,                 // ���������� ����������
        NULL                         // �������������� ���������
    );
}

// ���������� ��������� WM_COMMAND - ��������� ������ �� ��������� ����������
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    // ������������ ������� � ����������� �� ID ��������
    switch (id)
    {
    case IDC_BUTTON_CLICK: // ��������� ������� ������
    {
        g_clickCount++; // ����������� ������� �������

        // �������� ����� �� ���� �����
        wchar_t buffer[256]; // ����� ��� ������
        GetWindowText(g_hEditInput, buffer, 256); // ������ ����� �� ���� �����

        // ��������� ������ ��� ����������� � �������������� stringstream
        std::wstringstream ss;
        ss << L"������ ������: " << g_clickCount << L" ���(�)\n";
        ss << L"��������� �����: " << buffer;

        // ��������� �������������� �����
        g_displayText = ss.str();

        // ��������� ����� � ����������� ����
        SetWindowText(g_hStaticText, g_displayText.c_str());

        // ����������� ����������� ����
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    }
}

// ���������� ��������� WM_PAINT - ����������� ����
void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps); // �������� �����������

    // �������� ������� ���������� �������
    RECT rect;
    GetClientRect(hwnd, &rect);

    // ������� ����� ��� ���� ������-������ �����
    HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &rect, hBrush);    // �������� ���
    DeleteObject(hBrush);            // ������� �����

    // ������ �������������� ������ ����� ����
    rect.top = rect.bottom - 30;     // ������������� ������� ������� ��� ������
    SetTextColor(hdc, RGB(0, 0, 255)); // ������������� ����� ���� ������
    SetBkMode(hdc, TRANSPARENT);     // ������������� ���������� ��� ��� ������

    // ������� ����� �� ������ ����� ����
    DrawText(hdc,
        L"TestProc v1.0 - ������� �������� ����������", // �����
        -1,                    // ����� ������ (�������������)
        &rect,                 // ������� ��� ������
        DT_CENTER | DT_VCENTER // ������������ �� ������ �� ����������� � ���������
    );

    EndPaint(hwnd, &ps); // ����������� �����������
}

// ���������� ��������� WM_DESTROY - �������� ����
void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0); // ���������� ��������� � ������ �� ����� ���������
}