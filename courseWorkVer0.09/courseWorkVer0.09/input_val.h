// input_validation.h
#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <cctype>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::regex;
using std::smatch;
using std::stoi;
using std::stod;
using std::string;

#if defined(_WIN32)
#include <windows.h>
#endif
#include <clocale>
#include <clocale>
#include <locale>
#include <iostream>


inline bool isValidUsername(const string& input) {
    if (input.empty()) return false;
    if (input.find(' ') != string::npos) return false;
    return true;
}


inline bool isValidChar(const string& input) {
    if (input.length() != 1) return false;
    char c = input[0];
    if (isspace(c) || !isprint(c)) return false;
    return true;
}


inline bool isValidInt(const string& input) {
    if (input.empty()) return false;
    size_t start = 0;
    if (input[0] == '+' || input[0] == '-') { start = 1; if (input.length() == 1) return false; }
    for (size_t i = start; i < input.size(); ++i) if (!isdigit(input[i])) return false;
    try { stoi(input); }
    catch (...) { return false; }
    return true;
}



inline bool isValidDouble(const string& input) {
    if (input.empty()) return false;
    size_t start = 0; int dots = 0; bool hasDigits = false;
    if (input[0] == '+' || input[0] == '-') { start = 1; if (input.length() == 1) return false; }
    for (size_t i = start; i < input.size(); ++i) {
        if (input[i] == '.') { if (dots++ > 0) return false; }
        else if (!isdigit(input[i])) return false;
        else hasDigits = true;
    }
    if (!hasDigits || dots > 1) return false;
    try { stod(input); }
    catch (...) { return false; }
    return true;
}


inline bool isValidString(const string& input) {
    if (input.empty()) return false;
    for (char c : input) if (!isspace(c)) return true;
    return false;
}


inline bool isValidEmail(const string& input) {
    if (input.empty()) return false;
    const regex pattern(R"((^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))");
    return regex_match(input, pattern);
}

// YYYY-MM-DD
inline bool isValidDate(const string& input) {
    const regex pattern(R"((\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01]))");
    smatch m;
    if (!regex_match(input, m, pattern)) return false;
    int y = stoi(m[1]), mo = stoi(m[2]), d = stoi(m[3]);
    static int md[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (mo == 2 && leap) return d <= 29;
    return d <= md[mo];
}
inline bool isValidAdultAge(const std::string& input) {
    if (!isValidInt(input)) return false;
    int age = std::stoi(input);
    return age > 18;
}

inline bool isValidBool(const string& input) {
    return input == "true" || input == "false";
}


inline bool isValidPassword(const string& input) {
    if (input.length() < 6) return false;
    bool d = false, l = false;
    for (char c : input) { if (isdigit(c)) d = true; if (isalpha(c)) l = true; }
    return d && l;
}


template<typename T>
T getValidatedInput(const string& prompt, bool(*validator)(const string&), T(*converter)(const string&)) {
    string in;

    while (true) {
        cout << prompt;
        getline(cin, in);
        if (validator(in)) {
            try { return converter(in); }
            catch (...) { cerr << "conversion error.\n"; }
        }
        else cerr << "Incorrect input. Tru again.\n";
    }
}


inline int convertToInt(const string& s) { return stoi(s); }
inline double convertToDouble(const string& s) { return stod(s); }
inline string convertToString(const string& s) { return s; }
inline char convertToChar(const string& s) { return s[0]; }
inline bool convertToBool(const string& s) { return s == "true"; }

#endif 
