// TextProcessor.cpp
// ���������� ������� ��� ��������� ������

#define TEXTPROC_EXPORTS
#include "TextProcessor.h"
#include <fstream>   // ��� ������ � ������� (ifstream, ofstream)
#include <string>    // ��� ������������� ������ string
#include <sstream>   // ��� ������������� stringstream

TP_API void ProcessFileAndAppendWordCount(const char* filename) {
    // 1. ��������� ���� ��� ������
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // ���� ���� �� ��������, ������ ������� �� �������
        return;
    }

    // 2. ��������� ��� ���������� ����� � ������
    std::stringstream buffer;
    buffer << inFile.rdbuf(); // ��������� ���� ����� ����� � �����
    std::string content = buffer.str(); // ����������� ����� � ������
    inFile.close(); // ��������� ���� ����� ������

    // 3. ������������ �����
    std::istringstream iss(content); // ������� ����� ��� ������ �� ������
    std::string word;
    int wordCount = 0;

    // �������� >> ��� istringstream ������ ���� ����� (�� �������)
    while (iss >> word) {
        wordCount++; // ����������� ������� �� ������ ��������� �����
    }

    // 4. ��������� ���� �����, �� ������ ��� �������� � ����� (app - append)
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        // ��������� ��������� � ����� �����
        outFile << "\n\n���������� ���� � �����: " << wordCount;
        outFile.close();
    }
    // ���� �� ��������, ������ �� ������
}