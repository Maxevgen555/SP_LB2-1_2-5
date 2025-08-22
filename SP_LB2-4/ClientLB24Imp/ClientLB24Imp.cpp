// ClientLB24Imp.cpp
// Клиентское приложение с неявным подключением библиотеки
// Для работы требует наличия .lib файла библиотеки в настройках компоновщика

#include <iostream>
#include "../Lb24_Goncharov/Lb24_Goncharov.h" // Подключаем заголовок библиотеки

int main() {
    std::cout << "Клиент с неявной компоновкой (Implicit Linking):" << std::endl;

    // Перед вызовом функций выводим значения глобальных переменных из DLL
    std::cout << "g_nDllCallsCount (до вызовов): " << g_nDllCallsCount << std::endl;
    std::cout << "g_nFnCallsCount (до вызовов): " << g_nFnCallsCount << std::endl;
    std::cout << "--- Вызов функций ---" << std::endl;

    // Вызываем функцию Fun31 и выводим результат
    int result1 = Fun31(5.5, 3.2);
    std::cout << "Fun31(5.5, 3.2) = " << result1 << std::endl;

    // Вызываем функцию Fun32 и выводим результат
    float result2 = Fun32(10, 20, 30);
    std::cout << "Fun32(10, 20, 30) = " << result2 << std::endl;

    // Вызываем функцию Fun33. Результат передается через указатель.
    int outputVal;
    Fun33(15, &outputVal);
    std::cout << "Fun33(15, &out); out = " << outputVal << std::endl;

    std::cout << "--- После вызовов функций ---" << std::endl;
    // Снова выводим значения глобальных переменных.
    // g_nDllCallsCount не изменился, т.к. библиотека была загружена 1 раз.
    // g_nFnCallsCount увеличился на 3.
    std::cout << "g_nDllCallsCount: " << g_nDllCallsCount << std::endl;
    std::cout << "g_nFnCallsCount: " << g_nFnCallsCount << std::endl;

    system("pause"); // Пауза, чтобы окно консоли не закрылось сразу
    return 0;
}