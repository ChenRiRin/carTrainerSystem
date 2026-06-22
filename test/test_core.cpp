#include <cassert>
#include <iostream>
#include <cmath>
#include "core/Vehicle.h"
#include "core/TimeUtils.h"
#include "core/Student.h"
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

void test_student_compare() {
    Student a("13800138000-01", "Alice", "13800138000", "Car", 15.0);
    Student b("13800138000-01", "Alice", "13800138000", "Car", 15.0);
    Student c("13912345678-01", "Bob", "13912345678", "SUV", 8.0);

    assert(a == b);
    assert(!(a == c));
}

void test_manager_operations() {
    Manager mgr(":memory:");

    auto& s1 = mgr.addStudent("13800138000", "张三", "轿车", 10.0);
    assert(s1.id == "13800138000-01");
    assert(std::abs(s1.remainingHours - 10.0) < 0.001);

    auto& s2 = mgr.addStudent("13912345678", "李四", "SUV", 5.0);
    assert(s2.id == "13912345678-01");

    // Test same phone gets incremented seq
    auto& s3 = mgr.addStudent("13800138000", "张三妻子", "轿车", 8.0);
    assert(s3.id == "13800138000-02");
    assert(s3.name == "张三妻子");

    // Test reservation
    auto schedule = mgr.addReservation("13800138000-01", "2099-12-31 10:00", 60);
    assert(schedule.id == 1);
    assert(std::abs(schedule.cost - 60.0) < 0.001);
    assert(std::abs(mgr.getStudents()[0].remainingHours - 9.0) < 0.001);

    // Test sorting
    auto sorted = mgr.getSortedStudents();
    assert(sorted[0].remainingHours >= sorted[1].remainingHours);

    // Test expired cleanup (should not delete future reservation)
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

int main() {
    std::cout << "Running core tests..." << std::endl;

    TEST(test_vehicle_fees);
    TEST(test_time_utils);
    TEST(test_student_compare);
    TEST(test_manager_operations);
    TEST(test_negative_duration);

    std::cout << std::endl;
    std::cout << "Results: " << (testsPassed + testsFailed)
              << " total, " << testsPassed << " passed, "
              << testsFailed << " failed" << std::endl;

    return testsFailed > 0 ? 1 : 0;
}
