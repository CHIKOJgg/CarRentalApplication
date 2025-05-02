#include "App.h"
#include <windows.h>
#include <clocale>
#include <locale>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif

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

int main() {
    setupRussLocale();
    ShowCursor(FALSE);
    App app;
    app.run();
    return 0;
}
