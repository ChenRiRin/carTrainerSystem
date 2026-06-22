#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include "core/Student.h"
#include "core/Schedule.h"

class Database {
    sqlite3* db{};
public:
    void exec(const std::string& sql);
    explicit Database(const std::string& path);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void initTables();

    void saveStudent(const Student& s);
    void saveSchedule(const Schedule& s);

    std::vector<Student> loadStudents();
    std::vector<Schedule> loadSchedules();

    void updateStudent(const Student& s);
    void updateSchedule(const Schedule& s);
    int deleteExpiredSchedules(const std::string& now);
    int deleteSchedule(int scheduleId);
};
