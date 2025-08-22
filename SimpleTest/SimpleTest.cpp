#include <windows.h>
#include <stdio.h>

int main() {
    printf("Testing DLL load...\n");

    HINSTANCE hDll = LoadLibrary("Lb24_Goncharov.dll");
    if (!hDll) {
        printf("LoadLibrary FAILED! Error: %lu\n", GetLastError());
        return 1;
    }
    printf("DLL loaded successfully!\n");

    // Проверяем экспорт функций
    FARPROC funcs[] = {
        GetProcAddress(hDll, "Fun31"),
        GetProcAddress(hDll, "Fun32"),
        GetProcAddress(hDll, "Fun33"),
        GetProcAddress(hDll, "GetDllCallsCount"),
        GetProcAddress(hDll, "GetFnCallsCount")
    };

    const char* names[] = { "Fun31", "Fun32", "Fun33", "GetDllCallsCount", "GetFnCallsCount" };

    for (int i = 0; i < 5; i++) {
        if (funcs[i]) {
            printf("✓ %s found at address: %p\n", names[i], funcs[i]);
        }
        else {
            printf("✗ %s NOT found! Error: %lu\n", names[i], GetLastError());
        }
    }

    FreeLibrary(hDll);
    printf("DLL unloaded.\n");
    system("pause");
    return 0;
}