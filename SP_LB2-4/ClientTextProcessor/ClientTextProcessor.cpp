// ClientTextProcessor.cpp
// Клиент для библиотеки обработки текста
// Использует неявную компоновку

#include <iostream>
#include <windows.h> // Для ShellExecute
#include "../TextProcessor/TextProcessor.h" // Подключаем заголовок библиотеки

int main() {
    const char* filename = "test.txt"; // Имя файла для обработки

    std::cout << "Обработка файла '" << filename << "'..." << std::endl;

    // Вызываем функцию из DLL
    ProcessFileAndAppendWordCount(filename);

    std::cout << "Обработка завершена. Результат записан в конец файла." << std::endl;
    std::cout << "Запуск Блокнота (Notepad) для просмотра..." << std::endl;

    // Запускаем Блокнот для открытия обработанного файла
    // Параметры: HWND, операция ("open"), файл для открытия (notepad.exe),
    // параметры (имя нашего файла), рабочий каталог (по умолчанию),
    // команда для отображения окна (показать нормально)
    HINSTANCE result = ShellExecuteA(NULL, "open", "notepad.exe", filename, NULL, SW_SHOWNORMAL);

    // Проверка на ошибку (коды возврата HINSTANCE <= 32 означают ошибку)
    if ((int)result <= 32) {
        std::cerr << "Ошибка при запуске Блокнота!" << std::endl;
    }

    system("pause");
    return 0;
}