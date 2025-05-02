#pragma once

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

std::ostream& dashesManip(std::ostream& os);
std::ostream& equalsignManip(std::ostream& os);
std::string hashPassword(const std::string& password);
bool isValidPassword(const std::string& password);
