#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>

// Проверяет, что строка содержит только цифры (для целочисленных значений)
bool isIntCorrect(const std::string& input);

// Проверяет, что строка не пуста и не состоит только из пробельных символов
bool isStringCorrect(const std::string& input);

#endif // INPUT_VALIDATOR_H
