#include "TimeUtils.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace time_utils {

int toMinutes(int hours, int minutes) {
    return hours * 60 + minutes;
}

double toHours(int minutes) {
    return minutes / 60.0;
}

std::string format(int minutes) {
    int h = minutes / 60;
    int m = minutes % 60;
    if (h > 0) {
        return std::to_string(h) + "h" + std::to_string(m) + "m";
    }
    return std::to_string(m) + "m";
}

double calculateRefund(double paid, int usedMinutes, int totalMinutes) {
    if (totalMinutes <= 0) return 0;
    double rate = static_cast<double>(usedMinutes) / totalMinutes;
    if (rate >= 0.5) return 0;
    return paid * (1.0 - rate);
}

std::string currentDateTime() {
    auto t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M");
    return oss.str();
}

time_t toTimeT(const std::string& date) {
    std::tm tm = {};
    std::istringstream iss(date);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    return std::mktime(&tm);
}

bool isExpired(const std::string& date) {
    return date < currentDateTime();
}

bool isValidDate(const std::string& date) {
    if (date.size() < 16) return false;
    int y, m, d, h, min;
    char dash1, dash2, space, colon;
    std::istringstream iss(date);
    iss >> y >> dash1 >> m >> dash2 >> d;
    iss.get(space);
    iss >> h >> colon >> min;
    if (iss.fail() || dash1 != '-' || dash2 != '-' || space != ' ' || colon != ':')
        return false;
    if (m < 1 || m > 12 || h < 0 || h > 23 || min < 0 || min > 59)
        return false;
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxDay = daysInMonth[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
        maxDay = 29;
    return d >= 1 && d <= maxDay;
}

}
