#pragma once
#include <string>

struct Student {
    std::string id;
    std::string name;
    std::string phone;
    std::string vehicleType;
    double remainingHours{};

    Student() = default;
    Student(std::string id, std::string name, std::string phone,
            std::string vehicleType, double hours);

    auto operator<=>(const Student&) const = default;
};
