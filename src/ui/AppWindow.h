#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <functional>
#include "core/Manager.h"

class StudentTable : public Fl_Table_Row {
    Manager& manager;
    int contextRow = -1;
    int handle(int event) override;
protected:
    void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override;
public:
    StudentTable(int X, int Y, int W, int H, Manager& mgr);
    void refresh();
    std::function<void(const std::string&)> onRightClick;
};

class ScheduleTable : public Fl_Table_Row {
    Manager& manager;
    int contextRow = -1;
    int handle(int event) override;
protected:
    void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override;
public:
    ScheduleTable(int X, int Y, int W, int H, Manager& mgr);
    void refresh();
    std::function<void(int)> onModifyClick;
    std::function<void(int)> onRefundClick;
};

class AppWindow : public Fl_Window {
public:
    enum MenuAction {
        MENU_ADD_RESERVATION = 1,
        MENU_MODIFY_DURATION,
        MENU_REFUND,
        MENU_ADD_STUDENT,
        MENU_SORT_STUDENTS,
        MENU_CLEAN_EXPIRED,
        MENU_SAVE,
        MENU_EXIT
    };

    static void menu_cb(Fl_Widget* w, void* data);
    static void onCleanTimeout(void* data);

    AppWindow();

    Manager manager;

private:
    Fl_Menu_Bar* menu;
    StudentTable* studentTable;
    ScheduleTable* scheduleTable;
    Fl_Output* statusBar;
    Fl_Box* statusCounts;

    void handleMenu(int action);
    void showAddReservation(const std::string& preselectedId = {});
    void showModifyDuration(int preselectedId = -1);
    void showRefund(int preselectedId = -1);
    void showAddStudent();
    void showSortedStudents();
    void doCleanExpired();
    void doSave();
    void updateStatus(const char* msg);
    void refreshTables();
};
