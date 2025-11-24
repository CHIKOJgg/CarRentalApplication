#pragma once
#if defined(_WIN32)
#include <windows.h>
#endif
#include <clocale>
#include <clocale>
#include <locale>
#include <iostream>
//Подарок от Канаша

void setupRussLocale() {
#if defined(_WIN32)
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");
    std::wcout.imbue(std::locale("rus_rus.1251"));
#else
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
#endif
}