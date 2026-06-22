#pragma once
#include <string>

struct Schedule {
    int id{};
    std::string studentId;
    std::string date;
    int durationMinutes{};
    double cost{};
    std::string status = "active";

    Schedule() = default;
    Schedule(int id, std::string studentId, std::string date, int minutes, double cost);
};
