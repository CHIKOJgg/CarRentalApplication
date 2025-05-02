#pragma once
#include <string>
#include <memory>
#include <vector>

class User;

class Car {
protected:
    std::string model;
    double basePrice;
    int minAge;

public:
    Car(const std::string& model, double basePrice, int minAge);
    virtual ~Car();
    virtual std::string getType() const = 0;
    virtual double getPrice(const User& user) const;
    virtual void displayInfo() const;
    int getMinAge() const;
    std::string getModel() const;
    double getBasePrice() const;
};

class EconomyCar : public Car {
public:
    EconomyCar(const std::string& model, double basePrice);
    std::string getType() const override;
    double getPrice(const User& user) const override;
};

class ComfortCar : public Car {
public:
    ComfortCar(const std::string& model, double basePrice);
    std::string getType() const override;
};

class BusinessCar : public Car {
public:
    BusinessCar(const std::string& model, double basePrice);
    std::string getType() const override;
};

class PremiumCar : public Car {
public:
    PremiumCar(const std::string& model, double basePrice);
    std::string getType() const override;
};
