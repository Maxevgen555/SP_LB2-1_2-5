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
// ����� ������� � TextProcessor.h
extern "C" TP_API int CountWordsInFile(const char* filename);

// ���������� � TextProcessor.cpp
TP_API int CountWordsInFile(const char* filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return -1;
    }

    std::stringstream buffer;
    std::string line;
    int wordCount = 0;

    while (std::getline(inFile, line)) {
        if (line.find("���������� ���� � �����:") != std::string::npos) {
            break; // ���������� ������ ��� ��������� �����������
        }

        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            wordCount++;
        }
    }
    inFile.close();

    return wordCount;
}