#include "Vehicle.h"

double Car::calculateFee(int minutes) const {
    return minutes * 1.0;
}

double SUV::calculateFee(int minutes) const {
    return minutes * 1.5;
}

double Truck::calculateFee(int minutes) const {
    return minutes * 2.0;
}

std::unique_ptr<Vehicle> Vehicle::create(const std::string& type) {
    if (type == "轿车" || type == "Car") return std::make_unique<Car>();
    if (type == "SUV") return std::make_unique<SUV>();
    if (type == "卡车" || type == "Truck") return std::make_unique<Truck>();
    return std::make_unique<Car>();
}
