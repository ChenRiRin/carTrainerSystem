#pragma once
#include <string>
#include <memory>

class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual double calculateFee(int minutes) const = 0;
    virtual const char* typeName() const = 0;
    static std::unique_ptr<Vehicle> create(const std::string& type);
};

class Car : public Vehicle {
public:
    double calculateFee(int minutes) const override;
    const char* typeName() const override { return "轿车"; }
};

class SUV : public Vehicle {
public:
    double calculateFee(int minutes) const override;
    const char* typeName() const override { return "SUV"; }
};

class Truck : public Vehicle {
public:
    double calculateFee(int minutes) const override;
    const char* typeName() const override { return "卡车"; }
};
