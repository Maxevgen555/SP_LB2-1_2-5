// ClientLB24Imp.cpp
// ���������� ���������� � ������� ������������ ����������
// ��� ������ ������� ������� .lib ����� ���������� � ���������� ������������

#include <iostream>
#include "../Lb24_Goncharov/Lb24_Goncharov.h" // ���������� ��������� ����������

int main() {
    std::cout << "������ � ������� ����������� (Implicit Linking):" << std::endl;

    // ����� ������� ������� ������� �������� ���������� ���������� �� DLL
    std::cout << "g_nDllCallsCount (�� �������): " << g_nDllCallsCount << std::endl;
    std::cout << "g_nFnCallsCount (�� �������): " << g_nFnCallsCount << std::endl;
    std::cout << "--- ����� ������� ---" << std::endl;

    // �������� ������� Fun31 � ������� ���������
    int result1 = Fun31(5.5, 3.2);
    std::cout << "Fun31(5.5, 3.2) = " << result1 << std::endl;

    // �������� ������� Fun32 � ������� ���������
    float result2 = Fun32(10, 20, 30);
    std::cout << "Fun32(10, 20, 30) = " << result2 << std::endl;

    // �������� ������� Fun33. ��������� ���������� ����� ���������.
    int outputVal;
    Fun33(15, &outputVal);
    std::cout << "Fun33(15, &out); out = " << outputVal << std::endl;

    std::cout << "--- ����� ������� ������� ---" << std::endl;
    // ����� ������� �������� ���������� ����������.
    // g_nDllCallsCount �� ���������, �.�. ���������� ���� ��������� 1 ���.
    // g_nFnCallsCount ���������� �� 3.
    std::cout << "g_nDllCallsCount: " << g_nDllCallsCount << std::endl;
    std::cout << "g_nFnCallsCount: " << g_nFnCallsCount << std::endl;

    system("pause"); // �����, ����� ���� ������� �� ��������� �����
    return 0;
}