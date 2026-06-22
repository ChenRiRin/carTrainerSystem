#include "Student.h"

Student::Student(std::string id, std::string name, std::string phone,
                 std::string vehicleType, double hours)
    : id(std::move(id)), name(std::move(name)), phone(std::move(phone)),
      vehicleType(std::move(vehicleType)), remainingHours(hours) {}
