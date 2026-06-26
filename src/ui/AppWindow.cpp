#include "AppWindow.h"
#include "core/TimeUtils.h"
#include "core/Vehicle.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/fl_draw.H>
#include <sstream>
#include <iomanip>

StudentTable::StudentTable(int X, int Y, int W, int H, Manager& mgr)
    : Fl_Table_Row(X, Y, W, H), manager(mgr)
{
    col_header(1);
    col_resize(1);
    Fl_Table::cols(4);
    col_width(0, 180);
    col_width(1, 200);
    col_width(2, 120);
    col_width(3, 460);
    when(FL_WHEN_RELEASE);
    end();
}

void StudentTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch (context) {
        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(109, 109, 114));
            fl_font(FL_HELVETICA, 12);
            switch (C) {
                case 0: fl_draw("ID", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 1: fl_draw("姓名", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 2: fl_draw("车型", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 3: fl_draw("剩余课时", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                default: ;
            }
            fl_pop_clip();
            break;
        case CONTEXT_CELL: {
            auto& students = manager.getStudents();
            if (R >= (int)students.size()) break;
            const auto& s = students[R];
            fl_push_clip(X, Y, W, H);
            if (row_selected(R)) {
                fl_color(fl_rgb_color(229, 229, 234));
            } else {
                fl_color(FL_WHITE);
            }
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(33, 33, 33));
            fl_font(FL_HELVETICA, 13);
            char buf[64];
            switch (C) {
                case 0: snprintf(buf, sizeof(buf), "%s", s.id.c_str()); break;
                case 1: snprintf(buf, sizeof(buf), "%s", s.name.c_str()); break;
                case 2: snprintf(buf, sizeof(buf), "%s", s.vehicleType.c_str()); break;
                case 3: snprintf(buf, sizeof(buf), "%.1fh", s.remainingHours); break;
                default: ;
            }
            fl_draw(buf, X + 15, Y, W - 15, H, FL_ALIGN_LEFT);
            if (!row_selected(R)) {
                fl_color(fl_rgb_color(198, 198, 200));
                fl_xyline(X + 15, Y + H - 1, X + W);
            }
            fl_pop_clip();
            break;
        }
        default:
            break;
    }
}

int StudentTable::handle(int event) {
    if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE) {
        int headerH = col_header() ? col_header_height() : 0;
        int rowH = row_height(0);
        int y = Fl::event_y() - this->y() - headerH;
        contextRow = rowH > 0 ? y / rowH : -1;
        if (contextRow < 0 || contextRow >= rows()) return 0;
        for (int r = 0; r < rows(); ++r) select_row(r, r == contextRow);
        redraw();
        constexpr Fl_Menu_Item menu[] = {
            {"新增预约", 0, nullptr, nullptr},
            {nullptr}
        };
        if (auto* m = menu->popup(Fl::event_x(), Fl::event_y(), nullptr, nullptr, nullptr); m && onRightClick) {
            onRightClick(manager.getStudents()[contextRow].id);
        }
        return 1;
    }
    return Fl_Table_Row::handle(event);
}

void StudentTable::refresh() {
    rows(static_cast<int>(manager.getStudents().size()));
    redraw();
}

ScheduleTable::ScheduleTable(int X, int Y, int W, int H, Manager& mgr)
    : Fl_Table_Row(X, Y, W, H), manager(mgr)
{
    col_header(1);
    col_resize(1);
    Fl_Table::cols(5);
    col_width(0, 50);
    col_width(1, 200);
    col_width(2, 200);
    col_width(3, 100);
    col_width(4, 410);
    end();
}

void ScheduleTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) {
    switch (context) {
        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(109, 109, 114));
            fl_font(FL_HELVETICA, 12);
            switch (C) {
                case 0: fl_draw("ID", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 1: fl_draw("学员ID", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 2: fl_draw("日期", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 3: fl_draw("时长", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                case 4: fl_draw("状态", X + 15, Y, W - 15, H, FL_ALIGN_LEFT); break;
                default: ;
            }
            fl_pop_clip();
            break;
        case CONTEXT_CELL: {
            auto& allSchedules = manager.getSchedules();
            if (R >= (int)allSchedules.size()) break;
            const auto& s = allSchedules[R];
            fl_push_clip(X, Y, W, H);
            if (row_selected(R)) {
                fl_color(fl_rgb_color(229, 229, 234));
            } else {
                fl_color(FL_WHITE);
            }
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(33, 33, 33));
            fl_font(FL_HELVETICA, 13);
            char buf[64];
            switch (C) {
                case 0: snprintf(buf, sizeof(buf), "%d", s.id); break;
                case 1: snprintf(buf, sizeof(buf), "%s", s.studentId.c_str()); break;
                case 2: snprintf(buf, sizeof(buf), "%s", s.date.c_str()); break;
                case 3: snprintf(buf, sizeof(buf), "%s", time_utils::format(s.durationMinutes).c_str()); break;
                case 4: snprintf(buf, sizeof(buf), "%s", s.status.c_str()); break;
                default: ;
            }
            fl_draw(buf, X + 15, Y, W - 15, H, FL_ALIGN_LEFT);
            if (!row_selected(R)) {
                fl_color(fl_rgb_color(198, 198, 200));
                fl_xyline(X + 15, Y + H - 1, X + W);
            }
            fl_pop_clip();
            break;
        }
        default:
            break;
    }
}

int ScheduleTable::handle(int event) {
    if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE) {
        int headerH = col_header() ? col_header_height() : 0;
        int rowH = row_height(0);
        int y = Fl::event_y() - this->y() - headerH;
        contextRow = rowH > 0 ? y / rowH : -1;
        if (contextRow < 0 || contextRow >= rows()) return 0;
        for (int r = 0; r < rows(); ++r) select_row(r, r == contextRow);
        redraw();
        Fl_Menu_Item menu[] = {
            {"修改时长", 0, nullptr, nullptr},
            {"退费", 0, nullptr, nullptr},
            {nullptr}
        };
        auto* m = menu->popup(Fl::event_x(), Fl::event_y(), nullptr, nullptr, nullptr);
        if (m) {
            int scheduleId = manager.getSchedules()[contextRow].id;
            if (m == &menu[0] && onModifyClick) onModifyClick(scheduleId);
            else if (m == &menu[1] && onRefundClick) onRefundClick(scheduleId);
        }
        return 1;
    }
    return Fl_Table_Row::handle(event);
}

void ScheduleTable::refresh() {
    rows(static_cast<int>(manager.getSchedules().size()));
    redraw();
}

static Fl_Menu_Item menu_items[] = {
    {"预约管理", 0, nullptr, nullptr, FL_SUBMENU},
        {"新增预约", FL_COMMAND + 'n', AppWindow::menu_cb, (void*)AppWindow::MENU_ADD_RESERVATION},
        {"修改时长", 0, AppWindow::menu_cb, (void*)AppWindow::MENU_MODIFY_DURATION},
        {"退费", 0, AppWindow::menu_cb, (void*)AppWindow::MENU_REFUND},
        {nullptr},
    {"学员管理", 0, nullptr, nullptr, FL_SUBMENU},
        {"新增学员", 0, AppWindow::menu_cb, (void*)AppWindow::MENU_ADD_STUDENT},
        {"按剩余课时排序", 0, AppWindow::menu_cb, (void*)AppWindow::MENU_SORT_STUDENTS},
        {nullptr},
    {"系统", 0, nullptr, nullptr, FL_SUBMENU},
        {"清理过期预约", 0, AppWindow::menu_cb, (void*)AppWindow::MENU_CLEAN_EXPIRED},
        {"保存", FL_COMMAND + 's', AppWindow::menu_cb, (void*)AppWindow::MENU_SAVE},
        {"退出", FL_COMMAND + 'q', AppWindow::menu_cb, (void*)AppWindow::MENU_EXIT},
        {nullptr},
    {nullptr}
};

void AppWindow::menu_cb(Fl_Widget* w, void* data) {
    if (auto* win = dynamic_cast<AppWindow*>(w->window())) win->handleMenu((intptr_t)data);
}

void AppWindow::onCleanTimeout(void* data) {
    auto* win = static_cast<AppWindow*>(data);
    int count = win->manager.cleanExpired();
    if (count > 0) {
        win->manager.save();
        win->refreshTables();
        char buf[64];
        snprintf(buf, sizeof(buf), "自动清理: %d 条过期预约", count);
        win->updateStatus(buf);
    }
    Fl::repeat_timeout(300.0, onCleanTimeout, data);
}

AppWindow::AppWindow()
    : Fl_Window(960, 640, "驾校多车型计时计费管理系统"),
      manager("car_trainer.db")
{
    menu = new Fl_Menu_Bar(0, 0, 960, 25);
    menu->copy(menu_items);

    auto* studentLabel = new Fl_Box(15, 30, 200, 16, "学员列表");
    studentLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    studentLabel->labelcolor(fl_rgb_color(109, 109, 114));
    studentLabel->labelsize(13);
    studentLabel->labelfont(FL_HELVETICA_BOLD);

    studentTable = new StudentTable(0, 48, 960, 242, manager);

    auto* scheduleLabel = new Fl_Box(15, 298, 200, 16, "预约列表");
    scheduleLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    scheduleLabel->labelcolor(fl_rgb_color(109, 109, 114));
    scheduleLabel->labelsize(13);
    scheduleLabel->labelfont(FL_HELVETICA_BOLD);

    scheduleTable = new ScheduleTable(0, 312, 960, 258, manager);

    statusBar = new Fl_Output(0, 580, 480, 30, "");
    statusBar->box(FL_FLAT_BOX);

    statusCounts = new Fl_Box(480, 580, 480, 30);
    statusCounts->box(FL_FLAT_BOX);
    statusCounts->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    statusCounts->color(FL_BACKGROUND2_COLOR);

    studentTable->onRightClick = [this](const std::string& id) {
        showAddReservation(id);
    };
    scheduleTable->onModifyClick = [this](int id) {
        showModifyDuration(id);
    };
    scheduleTable->onRefundClick = [this](int id) {
        showRefund(id);
    };

    resizable(scheduleTable);
    size_range(800, 500);
    end();

    Fl::add_timeout(300.0, onCleanTimeout, this);

    int expired = manager.cleanExpired();
    if (expired > 0) {
        manager.save();
        char buf[64];
        snprintf(buf, sizeof(buf), "启动时清理了 %d 条过期预约", expired);
        updateStatus(buf);
    } else {
        updateStatus("就绪");
    }
    refreshTables();
}

void AppWindow::handleMenu(int action) {
    try {
        switch (action) {
            case MENU_ADD_RESERVATION: showAddReservation(); break;
            case MENU_MODIFY_DURATION: showModifyDuration(); break;
            case MENU_REFUND: showRefund(); break;
            case MENU_ADD_STUDENT: showAddStudent(); break;
            case MENU_SORT_STUDENTS: showSortedStudents(); break;
            case MENU_CLEAN_EXPIRED: doCleanExpired(); break;
            case MENU_SAVE: doSave(); break;
            case MENU_EXIT: hide(); break;
            default: ;
        }
    } catch (const std::exception& e) {
        fl_alert("错误: %s", e.what());
    }
    refreshTables();
}

void AppWindow::showAddStudent() {
    struct Dlg {
        Fl_Window* win{};
        Fl_Input* name{};
        Fl_Input* phone{};
        Fl_Choice* type{};
        Fl_Input* hours{};
        bool confirmed{};
    };

    auto* d = new Dlg();
    d->win = new Fl_Window(350, 220, "新增学员");
    d->name = new Fl_Input(100, 15, 200, 25, "姓名:");
    d->phone = new Fl_Input(100, 50, 200, 25, "手机:");
    d->type = new Fl_Choice(100, 85, 200, 25, "车型:");
    d->type->add("轿车|SUV|卡车");
    d->type->value(0);
    d->hours = new Fl_Input(100, 120, 200, 25, "总课时:");
    d->hours->value("20.0");

    auto* cancelBtn = new Fl_Button(100, 170, 90, 30, "取消");
    auto* okBtn = new Fl_Return_Button(200, 170, 90, 30, "确定");

    okBtn->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        std::string phone = dlg->phone->value();
        std::string name = dlg->name->value();
        double hours;
        try { hours = std::stod(std::string(dlg->hours->value())); }
        catch (...) { fl_alert("总课时必须为数字"); return; }
        if (phone.length() != 11 ||
            phone.find_first_not_of("0123456789") != std::string::npos) {
            fl_alert("手机号格式错误，请输入11位数字"); return;
        }
        if (name.empty()) { fl_alert("姓名不能为空"); return; }
        if (hours <= 0) { fl_alert("总课时必须为正数"); return; }
        dlg->confirmed = true;
        dlg->win->hide();
    }, d);

    cancelBtn->callback([](Fl_Widget*, void* data) {
        static_cast<Dlg*>(data)->win->hide();
    }, d);

    d->win->set_modal();
    d->win->end();
    d->win->show();

    while (d->win->shown()) Fl::wait();

    if (!d->confirmed) { delete d; return; }

    double totalHours;
    try { totalHours = std::stod(d->hours->value()); }
    catch (...) { fl_alert("总课时必须为数字"); delete d; return; }
    const auto& s = manager.addStudent(d->phone->value(), d->name->value(),
                                  d->type->text(), totalHours);
    char buf[96];
    snprintf(buf, sizeof(buf), "学员 %s(%s) 注册成功, 剩余 %.1f 课时",
             s.name.c_str(), s.id.c_str(), s.remainingHours);
    updateStatus(buf);
    manager.save();
    delete d;
}

void AppWindow::showAddReservation(const std::string& preselectedId) {
    struct Dlg {
        Fl_Window* win{};
        Fl_Input* search{};
        Fl_Browser* browser{};
        Fl_Output* selectedOut{};
        Fl_Input* date{};
        Fl_Input* duration{};
        Fl_Output* feeOutput{};
        Manager* mgr{};
        AppWindow* app{};
        std::string selectedId;
        std::function<void()> updateFee;
    };

    auto* d = new Dlg();
    d->mgr = &manager;
    d->app = this;
    d->win = new Fl_Window(560, 520, "新增预约");

    d->search = new Fl_Input(90, 10, 430, 25, "查找:");
    d->search->tooltip("输入手机号 / 姓名 / ID 搜索");

    d->browser = new Fl_Browser(90, 45, 430, 230);
    d->browser->type(FL_HOLD_BROWSER);

    d->selectedOut = new Fl_Output(90, 290, 430, 25, "已选:");
    d->selectedOut->box(FL_DOWN_BOX);

    d->date = new Fl_Input(90, 330, 430, 25, "日期:");
    d->date->value(time_utils::currentDateTime().c_str());

    d->duration = new Fl_Input(90, 365, 430, 25, "时长(分):");

    d->feeOutput = new Fl_Output(90, 400, 430, 25, "预估费用:");
    d->feeOutput->box(FL_DOWN_BOX);

    auto* cancelBtn = new Fl_Button(170, 450, 100, 30, "取消");
    auto* okBtn = new Fl_Return_Button(290, 450, 100, 30, "确定");

    d->updateFee = [d]() {
        if (d->selectedId.empty() || strlen(d->duration->value()) == 0) {
            d->feeOutput->value(""); return;
        }
        int minutes;
        try { minutes = std::stoi(d->duration->value()); }
        catch (...) { d->feeOutput->value(""); return; }
        if (minutes <= 0) { d->feeOutput->value(""); return; }
        for (const auto& s : d->mgr->getStudents()) {
            if (s.id == d->selectedId) {
                auto vehicle = Vehicle::create(s.vehicleType);
                double fee = vehicle->calculateFee(minutes);
                char buf[32];
                snprintf(buf, sizeof(buf), "¥%.2f", fee);
                d->feeOutput->value(buf);
                return;
            }
        }
        d->feeOutput->value("");
    };

    d->search->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        dlg->browser->clear();
        std::string q = dlg->search->value();
        for (const auto& s : dlg->mgr->getStudents()) {
            if (q.empty() ||
                s.id.find(q) != std::string::npos ||
                s.name.find(q) != std::string::npos ||
                s.phone.find(q) != std::string::npos) {
                char buf[256];
                snprintf(buf, sizeof(buf), "%s  %s  %s  剩余 %.1fh",
                         s.id.c_str(), s.name.c_str(), s.vehicleType.c_str(), s.remainingHours);
                dlg->browser->add(buf);
            }
        }
        dlg->browser->redraw();
    }, d);

    d->browser->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        int idx = dlg->browser->value();
        if (idx > 0) {
            const char* text = dlg->browser->text(idx);
            std::string line(text);
            auto space = line.find(' ');
            dlg->selectedId = line.substr(0, space);
            dlg->selectedOut->value(dlg->selectedId.c_str());
        }
        if (dlg->updateFee) dlg->updateFee();
    }, d);

    d->duration->when(FL_WHEN_CHANGED);
    d->duration->callback([](Fl_Widget*, void* data) {
        static_cast<Dlg*>(data)->updateFee();
    }, d);

    cancelBtn->callback([](Fl_Widget*, void* data) {
        static_cast<Dlg*>(data)->win->hide();
    }, d);

    okBtn->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        if (dlg->selectedId.empty()) {
            fl_alert("请先在列表中点击选择学员"); return;
        }
        std::string date = dlg->date->value();
        if (!time_utils::isValidDate(date)) {
            fl_alert("日期格式错误或日期不存在"); return;
        }
        if (time_utils::isExpired(date)) {
            fl_alert("不能预约过去的时间"); return;
        }
        std::string dur = dlg->duration->value();
        if (dur.empty()) { fl_alert("请输入时长"); return; }
        int minutes = std::stoi(dur);
        if (minutes <= 0) { fl_alert("时长必须为正数"); return; }
        try {
            auto s = dlg->mgr->addReservation(dlg->selectedId, date, minutes);
            dlg->mgr->save();
            double remaining = 0;
            for (const auto& st : dlg->mgr->getStudents()) {
                if (st.id == dlg->selectedId) { remaining = st.remainingHours; break; }
            }
            char buf[128];
            snprintf(buf, sizeof(buf), "预约成功! 费用: ¥%.2f, 剩余课时: %.1fh", s.cost, remaining);
            dlg->app->updateStatus(buf);
            dlg->app->refreshTables();
            dlg->win->hide();
        } catch (const std::exception& e) {
            fl_alert("错误: %s", e.what());
        }
    }, d);

    d->win->set_modal();
    d->win->end();
    d->win->show();

    if (!preselectedId.empty()) {
        d->search->value(preselectedId.c_str());
        d->search->do_callback();
        for (int i = 1; i <= d->browser->size(); ++i) {
            std::string text(d->browser->text(i));
            if (text.find(preselectedId) == 0) {
                d->browser->select(i);
                d->browser->do_callback();
                break;
            }
        }
    } else {
        d->search->do_callback();
    }

    d->updateFee();

    while (d->win->shown()) Fl::wait();
    delete d;
}

void AppWindow::showModifyDuration(int preselectedId) {
    struct Dlg {
        Fl_Window* win;
        Fl_Input* sid;
        Fl_Input* dur;
        Manager* mgr;
        AppWindow* app;
    };

    auto* d = new Dlg();
    d->mgr = &manager;
    d->app = this;
    d->win = new Fl_Window(350, 180, "修改时长");
    d->sid = new Fl_Input(100, 20, 200, 25, "预约ID:");
    d->dur = new Fl_Input(100, 55, 200, 25, "新时长(分):");

    if (preselectedId > 0) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", preselectedId);
        d->sid->value(buf);
    }

    auto* btn = new Fl_Return_Button(130, 120, 100, 30, "确定");
    btn->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        std::string dur = dlg->dur->value();
        if (dur.empty()) { fl_alert("请输入时长"); return; }
        int newMinutes = std::stoi(dur);
        if (newMinutes <= 0) { fl_alert("时长必须为正数"); return; }
        std::string id = dlg->sid->value();
        if (id.empty()) { fl_alert("请输入预约ID"); return; }
        int scheduleId = std::stoi(id);
        try {
            if (dlg->mgr->modifyDuration(scheduleId, newMinutes)) {
                dlg->mgr->save();
                char buf[64];
                snprintf(buf, sizeof(buf), "修改成功! 预约 #%d 时长为 %d 分钟", scheduleId, newMinutes);
                dlg->app->updateStatus(buf);
                dlg->app->refreshTables();
                dlg->win->hide();
            } else {
                fl_alert("修改失败: 预约不存在或已取消");
            }
        } catch (const std::exception& e) {
            fl_alert("错误: %s", e.what());
        }
    }, d);

    d->win->set_modal();
    d->win->end();
    d->win->show();
    while (d->win->shown()) Fl::wait();
    delete d;
}

void AppWindow::showRefund(int preselectedId) {
    struct Dlg {
        Fl_Window* win;
        Fl_Input* sid;
        Manager* mgr;
        AppWindow* app;
    };

    auto* d = new Dlg();
    d->mgr = &manager;
    d->app = this;
    d->win = new Fl_Window(350, 150, "退费");
    d->sid = new Fl_Input(100, 20, 200, 25, "预约ID:");

    if (preselectedId > 0) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", preselectedId);
        d->sid->value(buf);
    }

    auto* btn = new Fl_Return_Button(130, 70, 100, 30, "确定");
    btn->callback([](Fl_Widget*, void* data) {
        auto* dlg = static_cast<Dlg*>(data);
        std::string id = dlg->sid->value();
        if (id.empty()) { fl_alert("请输入预约ID"); return; }
        int scheduleId = std::stoi(id);
        try {
            double refundAmount = dlg->mgr->refund(scheduleId);
            if (refundAmount > 0) {
                dlg->mgr->save();
                char buf[64];
                snprintf(buf, sizeof(buf), "退费完成! 退费金额: ¥%.2f", refundAmount);
                dlg->app->updateStatus(buf);
                dlg->app->refreshTables();
                dlg->win->hide();
            } else {
                fl_alert("退费失败: 预约不存在或已退费");
            }
        } catch (const std::exception& e) {
            fl_alert("错误: %s", e.what());
        }
    }, d);

    d->win->set_modal();
    d->win->end();
    d->win->show();
    while (d->win->shown()) Fl::wait();
    delete d;
}

void AppWindow::showSortedStudents() {
    auto sorted = manager.getSortedStudents();
    std::ostringstream oss;
    oss << "按剩余课时排序:\n\n";
    oss << std::left
        << std::setw(20) << "ID"
        << std::setw(12) << "姓名"
        << std::setw(10) << "车型"
        << "剩余课时\n";
    oss << std::string(52, '-') << "\n";
    for (const auto& s : sorted) {
        oss << std::left
            << std::setw(20) << s.id
            << std::setw(12) << s.name
            << std::setw(10) << s.vehicleType
            << std::setw(6) << std::fixed << std::setprecision(1) << s.remainingHours << "h\n";
    }

    auto* win = new Fl_Window(550, 400, "学员排序");
    auto* display = new Fl_Multiline_Output(10, 10, 530, 350);
    display->value(oss.str().c_str());
    display->textfont(FL_COURIER);
    auto* btn = new Fl_Button(230, 365, 80, 25, "关闭");
    btn->callback([](Fl_Widget*, void* d) {
        ((Fl_Window*)d)->hide();
    }, win);
    win->set_modal();
    win->end();
    win->show();

    updateStatus("已按剩余课时降序排列学员");
}

void AppWindow::doCleanExpired() {
    int count = manager.cleanExpired();
    char buf[64];
    snprintf(buf, sizeof(buf), "清理完成! 已删除 %d 条过期预约", count);
    updateStatus(buf);
    manager.save();
    refreshTables();
}

void AppWindow::doSave() {
    manager.save();
    updateStatus("数据已保存到数据库");
}

void AppWindow::updateStatus(const char* msg) {
    auto& students = manager.getStudents();
    auto& schedules = manager.getSchedules();
    statusBar->value(msg);
    char buf[64];
    snprintf(buf, sizeof(buf), "学员: %zu 人 | 预约: %zu 条",
             students.size(), schedules.size());
    statusCounts->copy_label(buf);
}

void AppWindow::refreshTables() {
    studentTable->refresh();
    scheduleTable->refresh();
}
