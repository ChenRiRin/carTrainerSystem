#pragma once
#include <string>

namespace time_utils {
    int toMinutes(int hours, int minutes);
    double toHours(int minutes);
    std::string format(int minutes);
    double calculateRefund(double paid, int usedMinutes, int totalMinutes);
    std::string currentDateTime();
    bool isExpired(const std::string& date);
    bool isValidDate(const std::string& date);
}
