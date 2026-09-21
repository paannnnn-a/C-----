#include "../include/FeedingMenu.h"

#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
// 使用已有的饲养管理器
FeedingMenu::FeedingMenu(FeedingManager& manager)
    : feeding(manager) {
}

// 显示菜单
void FeedingMenu::showMenu() const {
    std::cout << "\n========== 喂养管理 ==========\n";
    std::cout << "1. 新增喂养计划\n";
    std::cout << "2. 查看全部计划\n";
    std::cout << "3. 按日期查询计划\n";
    std::cout << "4. 按动物编号查询计划\n";
    std::cout << "5. 完成喂养计划\n";
    std::cout << "6. 查看重新加载说明\n";
    std::cout << "7. 查看全部喂养记录\n";
    std::cout << "8. 按动物查询喂养记录\n";
    std::cout << "9. 查看今日喂养任务\n";
    std::cout << "10. 查看食物消耗统计\n";
    std::cout << "11. 按日期统计食物消耗\n";
    std::cout << "0. 返回上一页\n";
    std::cout << "M. 返回主菜单\n";
    std::cout << "Q. 退出系统\n";
    std::cout << "请选择：";
}

// 按日期查询计划
void FeedingMenu::queryByDate() {
    std::string date;

    std::cout << "请输入日期，例如2026-10-01"
                 "（输入0取消）：";

    if (!std::getline(std::cin, date)) {
        return;
    }

    if (date == "0") {
        std::cout << "已取消查询。\n";
        return;
    }

    feeding.showPlansByDate(date);
}

// 按动物编号查询计划
void FeedingMenu::queryByAnimal() {
    std::string animalId;

    std::cout << "请输入动物编号（输入0取消）：";

    if (!std::getline(std::cin, animalId)) {
        return;
    }

    if (animalId == "0") {
        std::cout << "已取消查询。\n";
        return;
    }

    feeding.showPlansByAnimal(animalId);
}

// 菜单循环
char FeedingMenu::run() {
    std::string choice;

    while (true) {
        showMenu();

        if (!std::getline(std::cin, choice)) {
            return 'Q';
        }

        if (choice == "1") {
            addPlan();
        } else if (choice == "2") {
            feeding.showAllPlans();
        } else if (choice == "3") {
            queryByDate();
        } else if (choice == "4") {
            queryByAnimal();
        } else if (choice == "5") {
            completePlan();
        } else if (choice == "6") {
            std::cout << "返回主菜单或正常退出时，"
                         "程序会保存库存、计划和记录。\n";
            std::cout << "确认保存成功后，"
                         "重新启动程序即可加载数据。\n";
        } else if (choice == "7") {
            feeding.showAllRecords();
        } else if (choice == "8") {
            std::string animalId;

            std::cout << "请输入动物编号（输入0取消）：";

            if (!std::getline(std::cin, animalId)) {
                return 'Q';
            }

            if (animalId == "0") {
                std::cout << "已取消查询。\n";
            } else {
                feeding.showRecordsByAnimal(animalId);
            }
        } else if (choice == "9") {
            feeding.showTodayPlans();
        } else if (choice == "10") {
            feeding.showFoodConsumption();
        } else if (choice == "11") {
            std::string date;

            std::cout << "请输入实际完成日期"
                         "（YYYY-MM-DD，输入0取消）：";

            if (!std::getline(std::cin, date)) {
                return 'Q';
            }

            if (date == "0") {
                std::cout << "已取消统计。\n";
            } else {
                feeding.showFoodConsumptionByDate(date);
            }
        } else if (choice == "0") {
            return '0';
        } else if (choice == "M" || choice == "m") {
            return 'M';
        } else if (choice == "Q" || choice == "q") {
            std::string answer;

            std::cout << "确定退出系统吗？"
                         "输入Y确认，其他输入取消：";

            if (!std::getline(std::cin, answer)) {
                return 'Q';
            }

            if (answer == "Y" || answer == "y") {
                return 'Q';
            }

            std::cout << "已取消退出。\n";
        } else {
            std::cout << "无效选项，请重新输入。\n";
        }
    }
}
// 新增喂养计划
void FeedingMenu::addPlan() {
    std::cout << "\n========== 新增喂养计划 ==========\n";
    std::cout << "任意录入步骤输入 /cancel 可取消。\n";

    // 读取非空文本，禁止空白和文件分隔符
    auto readText = [](const std::string& prompt,
                       std::string& value) -> bool {
        while (true) {
            std::cout << prompt;

            if (!std::getline(std::cin, value)) {
                return false;
            }

            if (value == "/cancel") {
                std::cout << "已取消添加。\n";
                return false;
            }

            if (value.empty() ||
                value.find_first_of(" \t\r\n|")
                    != std::string::npos) {
                std::cout << "内容不能为空，"
                             "也不能包含空白或竖线。\n";
                continue;
            }

            return true;
        }
    };

    // 检查指定位置是否全部为数字
    auto allDigits = [](const std::string& text,
                        std::size_t start,
                        std::size_t count) -> bool {
        for (std::size_t i = start; i < start + count; ++i) {
            if (text[i] < '0' || text[i] > '9') {
                return false;
            }
        }

        return true;
    };

    std::string planId;
    std::string animalId;
    std::string foodId;
    std::string date;
    std::string time;
    double amount = 0;

    if (!readText("计划编号（例如FP0001）：", planId)) {
        return;
    }

    if (!readText("动物编号（例如P001）：", animalId)) {
        return;
    }

    if (!readText("食物编号（例如F001）：", foodId)) {
        return;
    }

    // 检查日期格式和实际天数
    while (true) {
        if (!readText("喂养日期（YYYY-MM-DD）：", date)) {
            return;
        }

        if (date.size() != 10 ||
            date[4] != '-' || date[7] != '-' ||
            !allDigits(date, 0, 4) ||
            !allDigits(date, 5, 2) ||
            !allDigits(date, 8, 2)) {
            std::cout << "日期格式错误，例如2026-10-01。\n";
            continue;
        }

        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (year < 1 || month < 1 || month > 12) {
            std::cout << "年份或月份不合法。\n";
            continue;
        }

        int daysInMonth[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        bool leapYear =
            (year % 400 == 0) ||
            (year % 4 == 0 && year % 100 != 0);

        if (leapYear) {
            daysInMonth[1] = 29;
        }

        if (day < 1 || day > daysInMonth[month - 1]) {
            std::cout << "该月份不存在这一天，请重新输入。\n";
            continue;
        }

        break;
    }

    // 检查24小时制时间
    while (true) {
        if (!readText("喂养时间（HH:MM，例如08:00）：", time)) {
            return;
        }

        if (time.size() != 5 || time[2] != ':' ||
            !allDigits(time, 0, 2) ||
            !allDigits(time, 3, 2)) {
            std::cout << "时间格式错误，例如08:00。\n";
            continue;
        }

        int hour = std::stoi(time.substr(0, 2));
        int minute = std::stoi(time.substr(3, 2));

        if (hour > 23 || minute > 59) {
            std::cout << "小时应为00~23，分钟应为00~59。\n";
            continue;
        }

        break;
    }

    // 检查喂养数量
    while (true) {
        std::string input;

        if (!readText(
                "喂养数量（单位与对应食物一致）：",
                input)) {
            return;
        }

        std::istringstream stream(input);

        if (!(stream >> amount)) {
            std::cout << "请输入有效数字。\n";
            continue;
        }

        stream >> std::ws;

        if (!stream.eof() ||
            !std::isfinite(amount) || amount <= 0) {
            std::cout << "数量必须是大于0的有限数值。\n";
            continue;
        }

        break;
    }

    FeedingPlan plan(
        planId, animalId, foodId, date, time, amount
    );

    std::cout << "\n请核对计划信息：\n";
    plan.display();

    std::cout << "输入Y确认添加，其他输入取消：";
    std::string answer;

    if (!std::getline(std::cin, answer)) {
        return;
    }

    if (answer != "Y" && answer != "y") {
        std::cout << "已取消添加。\n";
        return;
    }

    // 管理器继续检查计划编号是否重复、食物是否存在
    feeding.addPlan(plan);
}
// 确认后执行喂养计划
void FeedingMenu::completePlan() {
    std::cout << "\n========== 完成喂养计划 ==========\n";

    // 展示计划，便于核对编号、食物和数量
    feeding.showAllPlans();

    std::cout << "请输入要完成的计划编号"
                 "（输入0或/cancel取消）：";

    std::string planId;

    if (!std::getline(std::cin, planId)) {
        return;
    }

    if (planId == "0" || planId == "/cancel") {
        std::cout << "已取消操作。\n";
        return;
    }

    if (planId.empty() ||
        planId.find_first_of(" \t\r\n|") != std::string::npos) {
        std::cout << "计划编号不能为空，"
                     "也不能包含空白或竖线。\n";
        return;
    }

    std::cout << "\n准备完成计划：" << planId << "\n";
    std::cout << "执行成功后，将扣除该计划对应的食物库存，"
                 "并标记为已完成。\n";
    std::cout << "请确认实际喂养已经完成。\n";
    std::cout << "输入Y确认，其他输入取消：";

    std::string answer;

    if (!std::getline(std::cin, answer)) {
        return;
    }

    if (answer != "Y" && answer != "y") {
        std::cout << "已取消操作，计划和库存均未改变。\n";
        return;
    }

    std::string keeperName;
    std::cout << "请输入操作人员姓名"
                 "（输入/cancel取消）：";

    if (!std::getline(std::cin, keeperName)) {
        return;
    }

    if (keeperName == "/cancel") {
        std::cout << "已取消操作，计划和库存均未改变。\n";
        return;
    }

    if (!feeding.completePlan(planId, keeperName)) {
        std::cout << "本次操作未完成，请查看上方提示。\n";
    }
}
