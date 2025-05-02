#include "Utilities.h"

std::ostream& dashesManip(std::ostream& os) {
    os << std::setw(23) << std::setfill('-');
    return os;
}

std::ostream& equalsignManip(std::ostream& os) {
    os << std::setw(23) << std::setfill('=');
    return os;
}

std::string hashPassword(const std::string& password) {
    const std::string salt = "kurwabober";
    std::string salted = password + salt;
    size_t hash_val = std::hash<std::string>{}(salted);
    std::stringstream ss;
    ss << std::hex << hash_val;
    return ss.str();
}


