#include "User.h"
#include "Utilities.h"

User::User()
    : age(0), experience(0.0), rating(0), isBlocked(false), isAdmin(false) {}

User::User(const std::string& uname, const std::string& pass, int age, double exp,
    int rating, bool blocked, bool isAdmin)
    : username(uname),
    hashedPassword(hashPassword(pass)),
    age(age),
    experience(exp),
    rating(rating),
    isBlocked(blocked),
    isAdmin(isAdmin) {}

bool User::checkPassword(const std::string& pass) const {
    return hashedPassword == hashPassword(pass);
}
