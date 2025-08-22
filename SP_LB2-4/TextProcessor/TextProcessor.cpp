// TextProcessor.cpp
// Реализация функций для обработки текста

#define TEXTPROC_EXPORTS
#include "TextProcessor.h"
#include <fstream>   // для работы с файлами (ifstream, ofstream)
#include <string>    // для использования класса string
#include <sstream>   // для использования stringstream

TP_API void ProcessFileAndAppendWordCount(const char* filename) {
    // 1. Открываем файл для чтения
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // Если файл не открылся, просто выходим из функции
        return;
    }

    // 2. Считываем все содержимое файла в строку
    std::stringstream buffer;
    buffer << inFile.rdbuf(); // Считываем весь буфер файла в поток
    std::string content = buffer.str(); // Преобразуем поток в строку
    inFile.close(); // Закрываем файл после чтения

    // 3. Подсчитываем слова
    std::istringstream iss(content); // Создаем поток для чтения из строки
    std::string word;
    int wordCount = 0;

    // Оператор >> для istringstream читает одно слово (до пробела)
    while (iss >> word) {
        wordCount++; // Увеличиваем счетчик на каждое считанное слово
    }

    // 4. Открываем файл СНОВА, но теперь для дозаписи в конец (app - append)
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        // Добавляем результат в конец файла
        outFile << "\n\nКоличество слов в файле: " << wordCount;
        outFile.close();
    }
    // Если не открылся, ничего не делаем
}