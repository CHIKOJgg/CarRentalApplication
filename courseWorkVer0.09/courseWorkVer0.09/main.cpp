#include "App.h"
#include <windows.h>
#include "russianLang.h"
#include <clocale>
#include <locale>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif


int main() {
    setupRussLocale();
    ShowCursor(FALSE);
    App app;
    app.run();
    return 0;
}
