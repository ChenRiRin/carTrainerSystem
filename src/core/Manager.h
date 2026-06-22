#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "core/Student.h"
#include "core/Schedule.h"
#include "db/Database.h"

class Manager {
    std::vector<Student> students;
    std::vector<Schedule> schedules;
    Database db;
    int nextScheduleId = 1;
    std::unordered_map<std::string, int> phoneSeq;

    Student* findStudent(const std::string& id);
    Schedule* findSchedule(int id);

public:
    explicit Manager(const std::string& dbPath);
    void load();
    void save();

    bool operator==(const Student& s) const;
    Student& addStudent(const std::string& phone, const std::string& name,
                        const std::string& vehicleType, double hours = 20.0);
    Schedule addReservation(const std::string& studentId, const std::string& date, int minutes);
    bool modifyDuration(int scheduleId, int newMinutes);
    double refund(int scheduleId);
    std::vector<Student> getSortedStudents() const;
    int cleanExpired();

    const std::vector<Student>& getStudents() const { return students; }
    const std::vector<Schedule>& getSchedules() const { return schedules; }
    std::vector<Schedule> getSchedulesForStudent(const std::string& studentId) const;
};
