#include "Car.h"
#include <iostream>

Car::Car() : price(0.0), requiredExperience(0) {}

Car::Car(const std::string& model, double price, int requiredExperience, const std::string& tariff)
    : model(model), price(price), requiredExperience(requiredExperience), tariff(tariff) {}

const std::string& Car::getModel() const {
    return model;
}

double Car::getPrice() const {
    return price;
}

int Car::getRequiredExperience() const {
    return requiredExperience;
}

const std::string& Car::getTariff() const {
    return tariff;
}

void Car::display() const {
    std::cout << "Модель: " << model << "\nЦена: " << price 
              << "\nТребуемый стаж: " << requiredExperience 
              << "\nТариф: " << tariff << "\n";
}
