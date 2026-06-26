#include "Manager.h"
#include "Vehicle.h"
#include "TimeUtils.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

Manager::Manager(const std::string& dbPath) : db(dbPath) {
    db.initTables();
    load();
}

void Manager::load() {
    students = db.loadStudents();
    schedules = db.loadSchedules();
    for (const auto& s : schedules) {
        if (s.id >= nextScheduleId) nextScheduleId = s.id + 1;
    }
    for (const auto& s : students) {
        std::string phone = s.phone;
        int seq = 0;
        auto dash = s.id.rfind('-');
        if (dash != std::string::npos) {
            seq = std::stoi(s.id.substr(dash + 1));
        }
        auto& cur = phoneSeq[phone];
        if (seq > cur) cur = seq;
    }
}

void Manager::save() {
    for (const auto& s : students) db.saveStudent(s);
    for (const auto& s : schedules) db.saveSchedule(s);
}

bool Manager::operator==(const Student& s) const {
    for (const auto& existing : students) {
        if (existing.id == s.id) return true;
    }
    return false;
}

Student* Manager::findStudent(const std::string& id) {
    for (auto& s : students) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

Schedule* Manager::findSchedule(int id) {
    for (auto& s : schedules) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

Student& Manager::addStudent(const std::string& phone, const std::string& name,
                             const std::string& vehicleType, double hours) {
    int seq = ++phoneSeq[phone];
    std::ostringstream oss;
    oss << phone << "-" << std::setw(2) << std::setfill('0') << seq;
    std::string id = oss.str();
    Student s(id, name, phone, vehicleType, hours);
    students.push_back(std::move(s));
    db.saveStudent(students.back());
    return students.back();
}

Schedule Manager::addReservation(const std::string& studentId, const std::string& date, int minutes) {
    if (minutes <= 0) {
        throw std::runtime_error("学时必须为正数");
    }
    auto* student = findStudent(studentId);
    if (!student) {
        throw std::runtime_error("学员不存在");
    }
    double hoursNeeded = time_utils::toHours(minutes);
    if (hoursNeeded > student->remainingHours) {
        throw std::runtime_error("剩余课时不足");
    }
    auto vehicle = Vehicle::create(student->vehicleType);
    double cost = vehicle->calculateFee(minutes);
    Schedule s(nextScheduleId++, studentId, date, minutes, cost);
    student->remainingHours -= hoursNeeded;
    schedules.push_back(s);
    db.saveSchedule(s);
    db.updateStudent(*student);
    return s;
}

bool Manager::modifyDuration(int scheduleId, int newMinutes) {
    if (newMinutes <= 0) {
        throw std::runtime_error("时长必须为正数");
    }
    auto* s = findSchedule(scheduleId);
    if (!s || s->status != "active") return false;
    auto* student = findStudent(s->studentId);
    if (!student) return false;
    int diff = newMinutes - s->durationMinutes;
    double hoursDiff = time_utils::toHours(diff);
    if (hoursDiff > student->remainingHours) {
        throw std::runtime_error("剩余课时不足");
    }
    auto vehicle = Vehicle::create(student->vehicleType);
    s->cost = vehicle->calculateFee(newMinutes);
    s->durationMinutes = newMinutes;
    student->remainingHours -= hoursDiff;
    db.updateSchedule(*s);
    db.updateStudent(*student);
    return true;
}

double Manager::refund(int scheduleId) {
    auto* s = findSchedule(scheduleId);
    if (!s || s->status != "active") return 0;
    auto* student = findStudent(s->studentId);
    if (!student) return 0;
    double refundAmount = time_utils::calculateRefund(s->cost, 0, s->durationMinutes);
    double hoursToReturn = time_utils::toHours(s->durationMinutes) * (refundAmount / s->cost);
    student->remainingHours += hoursToReturn;
    s->status = "refunded";
    db.updateSchedule(*s);
    db.updateStudent(*student);
    return refundAmount;
}

std::vector<Student> Manager::getSortedStudents() const {
    std::vector<Student> sorted = students;
    std::sort(sorted.begin(), sorted.end(), [](const Student& a, const Student& b) {
        return a.remainingHours > b.remainingHours;
    });
    return sorted;
}

int Manager::cleanExpired() {
    std::string now = time_utils::currentDateTime();
    int count = 0;
    for (auto& s : schedules) {
        if (s.status == "active" && s.date < now) {
            s.status = "expired";
            ++count;
        }
    }
    db.markExpiredSchedules(now);
    return count;
}

std::vector<Schedule> Manager::getSchedulesForStudent(const std::string& studentId) const {
    std::vector<Schedule> result;
    for (const auto& s : schedules) {
        if (s.studentId == studentId) result.push_back(s);
    }
    return result;
}
