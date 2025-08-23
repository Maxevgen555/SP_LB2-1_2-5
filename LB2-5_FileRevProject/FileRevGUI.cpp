#include <windows.h>
#include <commdlg.h> // ��� GetOpenFileName
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <ctype.h>   // ��� isspace
#include "resource.h"

// ��������� �������
BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnBrowseFile(HWND hWnd);
void OnProcessFile(HWND hWnd);
int CountWordsInBuffer(const char* buffer, size_t bufferSize);
void ReportError(HWND hWnd, const TCHAR* msg);

// ����� �����
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)DlgProc);
}

// ������� ��������� �������
BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON_BROWSE:
            OnBrowseFile(hWnd);
            break;
        case IDC_BUTTON_PROCESS:
            OnProcessFile(hWnd);
            break;
        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        }
        return TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    }
    return FALSE;
}

// ������� ��� ������ �����
void OnBrowseFile(HWND hWnd) {
    TCHAR szFile[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = TEXT("��������� �����\0*.TXT;*.TEXT\0��� �����\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        SetDlgItemText(hWnd, IDC_EDIT_PATH, szFile);
        SetDlgItemText(hWnd, IDC_STATUS, TEXT("���� ������. ������� '����������'."));
    }
}

// ������� ��� �������� ����
int CountWordsInBuffer(const char* buffer, size_t bufferSize) {
    int wordCount = 0;
    bool inWord = false;
    for (size_t i = 0; i < bufferSize; ++i) {
        if (isspace((unsigned char)buffer[i])) {
            inWord = false;
        }
        else if (!inWord) {
            inWord = true;
            wordCount++;
        }
    }
    return wordCount;
}

// ������� ��� ������ ������
void ReportError(HWND hWnd, const TCHAR* msg) {
    TCHAR fullMsg[512];
    DWORD errCode = GetLastError();
    if (errCode != 0) {
        _stprintf_s(fullMsg, TEXT("%s\n��� ������: %d"), msg, errCode);
    }
    else {
        _tcscpy_s(fullMsg, msg);
    }
    MessageBox(hWnd, fullMsg, TEXT("������"), MB_ICONERROR | MB_OK);
    SetDlgItemText(hWnd, IDC_STATUS, fullMsg);
}

// �������� ������� ��������� �����
void OnProcessFile(HWND hWnd) {
    TCHAR filePath[MAX_PATH];
    GetDlgItemText(hWnd, IDC_EDIT_PATH, filePath, MAX_PATH);
    if (lstrlen(filePath) == 0) {
        MessageBox(hWnd, TEXT("������� �������� ����."), TEXT("������"), MB_ICONERROR);
        return;
    }

    SetDlgItemText(hWnd, IDC_STATUS, TEXT("������ ��������� �����..."));

    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapping = NULL;
    LPVOID pData = NULL;
    DWORD dwFileSize = 0;
    DWORD dwError = 0;

    __try {
        // 1. �������� �����
        hFile = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) { ReportError(hWnd, TEXT("CreateFile failed")); __leave; }

        // 2. ����������� �������
        dwFileSize = GetFileSize(hFile, NULL);
        if (dwFileSize == INVALID_FILE_SIZE) { ReportError(hWnd, TEXT("GetFileSize failed")); __leave; }
        if (dwFileSize == 0) { MessageBox(hWnd, TEXT("���� ����."), TEXT("����������"), MB_ICONINFORMATION); __leave; }

        // 3. �������� MAPPING OBJECT
        hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
        if (hMapping == NULL) { ReportError(hWnd, TEXT("CreateFileMapping failed")); __leave; }

        // 4.����������� ����� � ������
        pData = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (pData == NULL) { ReportError(hWnd, TEXT("MapViewOfFile failed")); __leave; }

        // 5. ������� ����
        int wordCount = CountWordsInBuffer((const char*)pData, dwFileSize);

        // 6. ���������� � ������ ����������
        // ��������� ������ ��� ������
        char resultStr[128];
        sprintf_s(resultStr, "\r\n���������� ���� � �����: %d", wordCount);
        size_t resultLen = strlen(resultStr);

        // ��������� ������� �����������, ����� �������� ������ �����
        UnmapViewOfFile(pData); pData = NULL;
        CloseHandle(hMapping); hMapping = NULL;

        // ����������� ������ �����
        DWORD newFileSize = dwFileSize + (DWORD)resultLen;
        SetFilePointer(hFile, newFileSize, NULL, FILE_BEGIN);
        if (!SetEndOfFile(hFile)) { ReportError(hWnd, TEXT("SetEndOfFile failed")); __leave; }

        // 7. ��������� ����������� (��� ������ �������)
        hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, newFileSize, NULL);
        if (hMapping == NULL) { ReportError(hWnd, TEXT("CreateFileMapping (2nd) failed")); __leave; }

        pData = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, newFileSize);
        if (pData == NULL) { ReportError(hWnd, TEXT("MapViewOfFile (2nd) failed")); __leave; }

        // 8. ������ ���������� � ����� �����
        char* pFileEnd = (char*)pData + dwFileSize;
        memcpy(pFileEnd, resultStr, resultLen);
        FlushViewOfFile(pData, newFileSize); // �������������� ������ �� ����

        // 9. �����!
        TCHAR statusMsg[256];
        _stprintf_s(statusMsg, TEXT("��������� ��������� �������!\n����: %s\n���������� ����: %d"), filePath, wordCount);
        SetDlgItemText(hWnd, IDC_STATUS, statusMsg);

        // 10. �������� ���������� � ��������
        TCHAR notepadCmd[MAX_PATH + 50];
        _stprintf_s(notepadCmd, TEXT("notepad.exe %s"), filePath);
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcess(NULL, notepadCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
    __finally {
        // 11. ������� ��������
        if (pData) UnmapViewOfFile(pData);
        if (hMapping) CloseHandle(hMapping);
        if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    }
}