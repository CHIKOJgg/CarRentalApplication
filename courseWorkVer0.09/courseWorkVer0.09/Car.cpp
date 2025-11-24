#include "Car.h"
#include "User.h"
#include <iostream>

Car::Car(const std::string& model, double basePrice, int minAge)
    : model(model), basePrice(basePrice), minAge(minAge) {}

Car::~Car() {}

std::string Car::getType() const { return ""; }
double Car::getPrice(const User&) const { return basePrice; }
void Car::displayInfo() const {
    std::cout << "Тип: " << getType()
        << "\nМодель: " << model
        << "\nБазовая цена: " << basePrice
        << "\nМин.возраст: " << minAge << "\n";
}
int Car::getMinAge() const { return minAge; }
std::string Car::getModel() const { return model; }
double Car::getBasePrice() const { return basePrice; }

EconomyCar::EconomyCar(const std::string& model, double basePrice)
    : Car(model, basePrice, 18) {}
std::string EconomyCar::getType() const { return "Эконом"; }
double EconomyCar::getPrice(const User& user) const {
    return user.experience < 1.0 ? basePrice * 1.1 : basePrice;
}

ComfortCar::ComfortCar(const std::string& model, double basePrice)
    : Car(model, basePrice, 21) {}
std::string ComfortCar::getType() const { return "Комфорт"; }

BusinessCar::BusinessCar(const std::string& model, double basePrice)
    : Car(model, basePrice, 21) {}
std::string BusinessCar::getType() const { return "Бизнес"; }

PremiumCar::PremiumCar(const std::string& model, double basePrice)
    : Car(model, basePrice, 25) {}
std::string PremiumCar::getType() const { return "Премиум"; }
