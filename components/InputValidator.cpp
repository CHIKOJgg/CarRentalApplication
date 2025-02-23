#include "InputValidator.h"
#include <cctype>

bool isIntCorrect(const std::string& input) {
    if (input.empty()) return false;
    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

bool isStringCorrect(const std::string& input) {
    if(input.empty()) return false;
    for (char c : input) {
        if (!isspace(static_cast<unsigned char>(c))) return true;
    }
    return false;
}
