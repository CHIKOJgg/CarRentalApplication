#ifndef CAR_H
#define CAR_H

#include <string>

class Car {
private:
    std::string model;
    double price;
    int requiredExperience; // Требуемый стаж (например, в годах)
    std::string tariff;     // Тарифный план

public:
    Car();
    Car(const std::string& model, double price, int requiredExperience, const std::string& tariff);
    
    // Геттеры
    const std::string& getModel() const;
    double getPrice() const;
    int getRequiredExperience() const;
    const std::string& getTariff() const;
    
    // Вывод информации об автомобиле
    void display() const;
};

#endif // CAR_H
