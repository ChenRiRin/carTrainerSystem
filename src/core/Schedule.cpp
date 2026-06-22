#include "Schedule.h"

Schedule::Schedule(int id, std::string studentId, std::string date, int minutes, double cost)
    : id(id), studentId(std::move(studentId)), date(std::move(date)),
      durationMinutes(minutes), cost(cost) {}
