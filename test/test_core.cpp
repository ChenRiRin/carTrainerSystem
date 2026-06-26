#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include "core/Vehicle.h"
#include "core/TimeUtils.h"
#include "core/Student.h"
#include "core/Schedule.h"
#include "core/Manager.h"

static int testsPassed = 0;
static int testsFailed = 0;

#define TEST(name) \
    do { \
        try { \
            name(); \
            ++testsPassed; \
            std::cout << "  PASS: " #name << std::endl; \
        } catch (const std::exception& e) { \
            ++testsFailed; \
            std::cout << "  FAIL: " #name " - " << e.what() << std::endl; \
        } catch (...) { \
            ++testsFailed; \
            std::cout << "  FAIL: " #name " - unknown error" << std::endl; \
        } \
    } while(0)

void test_vehicle_fees() {
    Car car;
    SUV suv;
    Truck truck;

    assert(std::abs(car.calculateFee(90) - 90.0) < 0.001);
    assert(std::abs(suv.calculateFee(60) - 90.0) < 0.001);
    assert(std::abs(truck.calculateFee(30) - 60.0) < 0.001);
    assert(std::abs(car.calculateFee(0) - 0.0) < 0.001);

    auto v = Vehicle::create("轿车");
    assert(std::abs(v->calculateFee(90) - 90.0) < 0.001);
    assert(std::string(v->typeName()) == "轿车");

    v = Vehicle::create("SUV");
    assert(std::abs(v->calculateFee(60) - 90.0) < 0.001);

    v = Vehicle::create("卡车");
    assert(std::abs(v->calculateFee(30) - 60.0) < 0.001);
}

void test_time_utils() {
    assert(time_utils::toMinutes(2, 30) == 150);
    assert(time_utils::toMinutes(0, 45) == 45);

    assert(std::abs(time_utils::toHours(150) - 2.5) < 0.001);
    assert(std::abs(time_utils::toHours(45) - 0.75) < 0.001);

    assert(time_utils::format(150) == "2h30m");
    assert(time_utils::format(45) == "45m");
    assert(time_utils::format(0) == "0m");

    double refund = time_utils::calculateRefund(100.0, 10, 100);
    assert(refund > 0);
    refund = time_utils::calculateRefund(100.0, 60, 100);
    assert(std::abs(refund) < 0.001);
}

void test_valid_dates() {
    assert(time_utils::isValidDate("2099-12-31 10:00"));
    assert(time_utils::isValidDate("2024-02-29 10:00"));
    assert(time_utils::isValidDate("2026-06-22 16:38"));
    assert(time_utils::isValidDate("2000-01-01 00:00"));

    assert(!time_utils::isValidDate(""));
    assert(!time_utils::isValidDate("abc"));
    assert(!time_utils::isValidDate("2026-02-29 10:00"));
    assert(!time_utils::isValidDate("2099-13-01 10:00"));
    assert(!time_utils::isValidDate("2099-12-01 24:00"));
    assert(!time_utils::isValidDate("2099-12-01 10:60"));
    assert(!time_utils::isValidDate("2099-12-01"));
    assert(!time_utils::isValidDate("2026-06-22 6:38"));

}

void test_expired() {
    assert(!time_utils::isExpired("2099-12-31 23:59"));
    assert(time_utils::isExpired("2000-01-01 00:00"));
}

void test_student_compare() {
    Student a("13800138000-01", "Alice", "13800138000", "Car", 15.0);
    Student b("13800138000-01", "Alice", "13800138000", "Car", 15.0);
    Student c("13912345678-01", "Bob", "13912345678", "SUV", 8.0);
    Student d("13800138000-01", "Alice", "13800138000", "SUV", 15.0);

    assert(a == b);
    assert(!(a == c));
    assert(!(a == d));
    assert(a != c);
    assert((a <=> c) != 0);
}

void test_manager_operations() {
    Manager mgr(":memory:");

    auto& s1 = mgr.addStudent("13800138000", "张三", "轿车", 10.0);
    assert(s1.id == "13800138000-01");
    assert(std::abs(s1.remainingHours - 10.0) < 0.001);

    auto& s2 = mgr.addStudent("13912345678", "李四", "SUV", 5.0);
    assert(s2.id == "13912345678-01");

    auto& s3 = mgr.addStudent("13800138000", "张三妻子", "轿车", 8.0);
    assert(s3.id == "13800138000-02");
    assert(s3.name == "张三妻子");

    auto schedule = mgr.addReservation("13800138000-01", "2099-12-31 10:00", 60);
    assert(schedule.id == 1);
    assert(std::abs(schedule.cost - 60.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 9.0) < 0.001);

    auto sorted = mgr.getSortedStudents();
    assert(sorted[0].remainingHours >= sorted[1].remainingHours);

    int count = mgr.cleanExpired();
    assert(count >= 0);
}

void test_negative_duration() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "Test", "轿车", 10.0);

    bool caught = false;
    try {
        mgr.addReservation("13800138000-01", "2099-12-31 10:00", -30);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);

    caught = false;
    try {
        mgr.modifyDuration(1, -10);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);
}

void test_modify_and_refund() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "张三", "轿车", 10.0);
    mgr.addReservation("13800138000-01", "2099-12-31 10:00", 60);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 9.0) < 0.001);

    bool ok = mgr.modifyDuration(1, 90);
    assert(ok);
    assert(mgr.getSchedules()[0].durationMinutes == 90);
    assert(std::abs(mgr.getSchedules()[0].cost - 90.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 8.5) < 0.001);

    ok = mgr.modifyDuration(1, 30);
    assert(ok);
    assert(mgr.getSchedules()[0].durationMinutes == 30);
    assert(std::abs(mgr.getSchedules()[0].cost - 30.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 9.5) < 0.001);

    ok = mgr.modifyDuration(999, 30);
    assert(!ok);

    bool caught = false;
    try {
        mgr.modifyDuration(1, -5);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);

    double refundAmount = mgr.refund(1);
    assert(refundAmount >= 0);
    assert(mgr.getSchedules()[0].status == "refunded");

    double refundAgain = mgr.refund(1);
    assert(std::abs(refundAgain) < 0.001);
}

void test_insufficient_hours() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "张三", "轿车", 1.0);

    bool caught = false;
    try {
        mgr.addReservation("13800138000-01", "2099-12-31 10:00", 120);
    } catch (const std::runtime_error& e) {
        caught = true;
        assert(std::string(e.what()) == "剩余课时不足");
    }
    assert(caught);
}

void test_student_not_found() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "张三", "轿车", 10.0);

    bool caught = false;
    try {
        mgr.addReservation("99999999-01", "2099-12-31 10:00", 60);
    } catch (const std::runtime_error& e) {
        caught = true;
        assert(std::string(e.what()) == "学员不存在");
    }
    assert(caught);
}

void test_persistence() {
    const char* testDb = "test_persistence.db";
    std::remove(testDb);

    {
        Manager mgr(testDb);
        mgr.addStudent("13800138000", "张三", "轿车", 10.0);
        mgr.addStudent("13912345678", "李四", "SUV", 5.0);
        mgr.addReservation("13800138000-01", "2099-12-31 10:00", 60);
        mgr.save();
    }

    {
        Manager mgr(testDb);
        assert(mgr.getStudents().size() == 2);
        assert(mgr.getStudents()[0].name == "张三");
        assert(mgr.getStudents()[1].name == "李四");
        assert(mgr.getStudents()[0].vehicleType == "轿车");
        assert(mgr.getSchedules().size() == 1);
        assert(mgr.getSchedules()[0].durationMinutes == 60);
        assert(std::abs(mgr.getSchedules()[0].cost - 60.0) < 0.001);
    }

    std::remove(testDb);
}

void test_existing_student_reservation_deducts() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "张三", "卡车", 20.0);

    auto s1 = mgr.addReservation("13800138000-01", "2099-12-31 10:00", 30);
    assert(std::abs(s1.cost - 60.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 19.5) < 0.001);

    auto s2 = mgr.addReservation("13800138000-01", "2099-12-31 11:00", 45);
    assert(std::abs(s2.cost - 90.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 18.75) < 0.001);
}

void test_reservation_conflict() {
    Manager mgr(":memory:");
    mgr.addStudent("13800138000", "张三", "轿车", 10.0);
    mgr.addReservation("13800138000-01", "2099-12-31 10:00", 60);

    bool caught = false;
    try {
        mgr.addReservation("13800138000-01", "2099-12-31 10:30", 60);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);

    mgr.addReservation("13800138000-01", "2099-12-31 13:00", 60);

    mgr.addStudent("13912345678", "李四", "SUV", 10.0);
    mgr.addReservation("13912345678-01", "2099-12-31 10:00", 60);
}

int main() {
    std::cout << "Running core tests..." << std::endl;

    TEST(test_vehicle_fees);
    TEST(test_time_utils);
    TEST(test_valid_dates);
    TEST(test_expired);
    TEST(test_student_compare);
    TEST(test_manager_operations);
    TEST(test_negative_duration);
    TEST(test_modify_and_refund);
    TEST(test_insufficient_hours);
    TEST(test_student_not_found);
    TEST(test_persistence);
    TEST(test_existing_student_reservation_deducts);
    TEST(test_reservation_conflict);

    std::cout << std::endl;
    std::cout << "Results: " << (testsPassed + testsFailed)
              << " total, " << testsPassed << " passed, "
              << testsFailed << " failed" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
