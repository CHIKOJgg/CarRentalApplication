#include "InputChecker.h"
#include <cctype>

namespace inputChecker {
    bool isIntCorrect(const std::string& input) {
        if (input.empty()) return false;
        if (input.length() > 10) return false;
        for (char c : input)
            if (!std::isdigit(static_cast<unsigned char>(c)))
                return false;
        return true;
    }

    bool isStringCorrect(const std::string& input) {
        if (input.empty()) return false;
        for (char c : input)
            if (!std::isspace(static_cast<unsigned char>(c)))
                return true;
        return false;
    }

    bool isDoubleCorrect(const std::string& input) {
        if (input.empty()) return false;
        size_t start = 0; int dotCount = 0, digitCount = 0;
        if (input[0] == '+' || input[0] == '-') start = 1;
        for (size_t i = start; i < input.size(); ++i) {
            char c = input[i];
            if (c == '.') {
                if (++dotCount > 1) return false;
            }
            else if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
            else digitCount++;
        }
        return digitCount > 0 && dotCount <= 1 &&
            input != "." && input != "+." && input != "-.";
    }
}
