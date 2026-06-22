#include "Database.h"
#include <cstring>
#include <iostream>
#include <sstream>

struct BinarySchedule {
    char id[32];
    char studentId[32];
    char date[32];
    int32_t durationMinutes;
    double cost;
    char status[16];
};

static BinarySchedule toBinary(const Schedule& s) {
    BinarySchedule bs{};
    std::strncpy(bs.id, std::to_string(s.id).c_str(), sizeof(bs.id) - 1);
    std::strncpy(bs.studentId, s.studentId.c_str(), sizeof(bs.studentId) - 1);
    std::strncpy(bs.date, s.date.c_str(), sizeof(bs.date) - 1);
    bs.durationMinutes = s.durationMinutes;
    bs.cost = s.cost;
    std::strncpy(bs.status, s.status.c_str(), sizeof(bs.status) - 1);
    return bs;
}

static Schedule fromBinary(const BinarySchedule& bs) {
    Schedule s;
    s.id = std::stoi(bs.id);
    s.studentId = bs.studentId;
    s.date = bs.date;
    s.durationMinutes = bs.durationMinutes;
    s.cost = bs.cost;
    s.status = bs.status;
    return s;
}

Database::Database(const std::string& path) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

void Database::exec(const std::string& sql) {
    if (!db) return;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void Database::initTables() {
    exec(
        "CREATE TABLE IF NOT EXISTS students ("
        "id TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "phone TEXT NOT NULL, "
        "vehicle_type TEXT NOT NULL, "
        "remaining_hours REAL DEFAULT 20.0)"
    );
    exec(
        "CREATE TABLE IF NOT EXISTS schedules ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id TEXT NOT NULL, "
        "date TEXT NOT NULL, "
        "duration_minutes INTEGER NOT NULL, "
        "cost REAL NOT NULL, "
        "status TEXT DEFAULT 'active', "
        "raw_blob BLOB)"
    );
}

void Database::saveStudent(const Student& s) {
    if (!db) return;
    auto sql = "INSERT OR REPLACE INTO students (id, name, phone, vehicle_type, remaining_hours) "
               "VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, s.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, s.vehicleType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, s.remainingHours);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::saveSchedule(const Schedule& s) {
    if (!db) return;
    auto sql = "INSERT OR REPLACE INTO schedules (id, student_id, date, duration_minutes, cost, status, raw_blob) "
               "VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    auto bs = toBinary(s);
    sqlite3_bind_int(stmt, 1, s.id);
    sqlite3_bind_text(stmt, 2, s.studentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, s.durationMinutes);
    sqlite3_bind_double(stmt, 5, s.cost);
    sqlite3_bind_text(stmt, 6, s.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 7, &bs, sizeof(bs), SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<Student> Database::loadStudents() {
    std::vector<Student> result;
    if (!db) return result;
    auto sql = "SELECT id, name, phone, vehicle_type, remaining_hours FROM students";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Student s;
        s.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        s.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        s.phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        s.vehicleType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        s.remainingHours = sqlite3_column_double(stmt, 4);
        result.push_back(s);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Schedule> Database::loadSchedules() {
    std::vector<Schedule> result;
    if (!db) return result;
    auto sql = "SELECT id, student_id, date, duration_minutes, cost, status, raw_blob FROM schedules";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_type(stmt, 6) == SQLITE_BLOB) {
            auto* bs = static_cast<const BinarySchedule*>(sqlite3_column_blob(stmt, 6));
            Schedule s = fromBinary(*bs);
            result.push_back(s);
        } else {
            Schedule s;
            s.id = sqlite3_column_int(stmt, 0);
            s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            s.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            s.durationMinutes = sqlite3_column_int(stmt, 3);
            s.cost = sqlite3_column_double(stmt, 4);
            s.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            result.push_back(s);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

void Database::updateStudent(const Student& s) {
    saveStudent(s);
}

void Database::updateSchedule(const Schedule& s) {
    saveSchedule(s);
}

int Database::deleteExpiredSchedules(const std::string& now) {
    if (!db) return 0;
    auto sql = "DELETE FROM schedules WHERE date < ? AND status = 'active'";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    int count = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return count;
}

int Database::deleteSchedule(int scheduleId) {
    if (!db) return 0;
    auto sql = "DELETE FROM schedules WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, scheduleId);
    sqlite3_step(stmt);
    int count = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return count;
}
