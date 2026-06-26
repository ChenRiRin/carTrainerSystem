#pragma once
#include <string>
#include <memory>

class Vehicle {
public:
    virtual ~Vehicle() = default;
    [[nodiscard]] virtual double calculateFee(int minutes) const = 0;
    [[nodiscard]] virtual const char* typeName() const = 0;
    static std::unique_ptr<Vehicle> create(const std::string& type);
};

class Car final : public Vehicle {
public:
    [[nodiscard]] double calculateFee(int minutes) const override;
    [[nodiscard]] const char* typeName() const override { return "轿车"; }
};

class SUV final : public Vehicle {
public:
    [[nodiscard]] double calculateFee(int minutes) const override;
    [[nodiscard]] const char* typeName() const override { return "SUV"; }
};

class Truck final : public Vehicle {
public:
    [[nodiscard]] double calculateFee(int minutes) const override;
    [[nodiscard]] const char* typeName() const override { return "卡车"; }
};
