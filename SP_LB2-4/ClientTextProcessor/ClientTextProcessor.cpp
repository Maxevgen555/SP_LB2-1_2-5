// ClientTextProcessor.cpp
// ������ ��� ���������� ��������� ������
// ���������� ������� ����������

#include <iostream>
#include <windows.h> // ��� ShellExecute
#include "../TextProcessor/TextProcessor.h" // ���������� ��������� ����������

int main() {
    const char* filename = "test.txt"; // ��� ����� ��� ���������

    std::cout << "��������� ����� '" << filename << "'..." << std::endl;

    // �������� ������� �� DLL
    ProcessFileAndAppendWordCount(filename);

    std::cout << "��������� ���������. ��������� ������� � ����� �����." << std::endl;
    std::cout << "������ �������� (Notepad) ��� ���������..." << std::endl;

    // ��������� ������� ��� �������� ������������� �����
    // ���������: HWND, �������� ("open"), ���� ��� �������� (notepad.exe),
    // ��������� (��� ������ �����), ������� ������� (�� ���������),
    // ������� ��� ����������� ���� (�������� ���������)
    HINSTANCE result = ShellExecuteA(NULL, "open", "notepad.exe", filename, NULL, SW_SHOWNORMAL);

    // �������� �� ������ (���� �������� HINSTANCE <= 32 �������� ������)
    if ((int)result <= 32) {
        std::cerr << "������ ��� ������� ��������!" << std::endl;
    }

    system("pause");
    return 0;
}