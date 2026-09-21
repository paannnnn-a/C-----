#include <iostream>
#include <string>

#include "include/InventoryManager.h"
#include "include/InventoryMenu.h"
#include "include/FeedingManager.h"
#include "include/FeedingMenu.h"

int main() {
    InventoryManager inventory;
    const std::string filename = "data/foods.txt";
    const std::string planFilename = "data/feeding_plans.txt";
    const std::string recordFilename = "data/feeding_records.txt";

    if (!inventory.loadFromFile(filename)) {
        std::cout << "库存加载失败，请检查数据文件。\n";
        return 1;
    }
    

    // 两个模块共用同一份库存
    FeedingManager feeding(inventory);
    // 必须先加载库存，再加载引用这些食物的计划
if (!feeding.loadFromFile(planFilename)) {
    std::cout << "计划加载失败，请检查计划文件。\n";
    return 1;
}
if (!feeding.loadRecordsFromFile(recordFilename)) {
    std::cout << "喂养记录加载失败，请检查记录文件。\n";
    return 1;
}

    InventoryMenu inventoryMenu(inventory, filename);
    FeedingMenu feedingMenu(feeding);

    bool shouldExit = false;

    while (!shouldExit) {
        std::cout << "\n========== 饲养与物资管理 ==========\n";
        std::cout << "1. 食物库存管理\n";
        std::cout << "2. 喂养计划管理\n";
        std::cout << "Q. 退出系统\n";
        std::cout << "请选择：";

        std::string choice;

        if (!std::getline(std::cin, choice)) {
            break;
        }

        if (choice == "1" || choice == "2") {
            char result;

            if (choice == "1") {
                result = inventoryMenu.run();
            } else {
                result = feedingMenu.run();
            }

            // 离开子菜单后，保存可能发生变化的库存
            if (!inventory.saveToFile(filename)) {
                std::cout << "库存保存失败，请检查文件路径"
                             "和写入权限。\n";
            }
            if (!feeding.saveToFile(planFilename)) {
                 std::cout << "计划保存失败，请检查文件路径"
                 "和写入权限。\n";
            }
             if (!feeding.saveRecordsToFile(recordFilename)) {
                 std::cout << "喂养记录保存失败，请检查文件路径"
                 "和写入权限。\n";
          }

            if (result == 'Q') {
                shouldExit = true;
            }

            // 返回0或M时，继续显示当前上级菜单
        } else if (choice == "Q" || choice == "q") {
            std::cout << "确定退出吗？"
                         "输入Y确认，其他输入取消：";

            std::string answer;

            if (!std::getline(std::cin, answer)) {
                break;
            }

            if (answer == "Y" || answer == "y") {
                shouldExit = true;
            }
        } else {
            std::cout << "无效选项，请重新输入。\n";
        }
    }

    if (!inventory.saveToFile(filename)) {
        std::cout << "退出前库存保存失败。\n";
        return 1;
    }

    if (!feeding.saveToFile(planFilename)) {
    std::cout << "退出前计划保存失败。\n";
    return 1;
}
if (!feeding.saveRecordsToFile(recordFilename)) {
    std::cout << "退出前喂养记录保存失败。\n";
    return 1;
}
    std::cout << "程序已结束。\n";
    return 0;
}