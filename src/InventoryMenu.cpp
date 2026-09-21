#include "../include/InventoryMenu.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include "../include/PlantFood.h"
#include "../include/MeatFood.h"
#include "../include/FruitFood.h"

#include <memory>
#include <utility>

// 构造函数：使用已有库存管理器，保存数据文件路径
InventoryMenu::InventoryMenu(
    InventoryManager& manager,
    const std::string& filename)
    : inventory(manager), dataFile(filename) {
}

// 显示菜单
void InventoryMenu::showMenu() const {
    std::cout << "\n========== 食物库存管理 ==========\n";
    std::cout << "1. 查看全部库存\n";
    std::cout << "2. 按编号查询食物\n";
    std::cout << "3. 食物入库\n";
    std::cout << "4. 食物出库\n";
    std::cout << "5. 查看库存预警\n";
    std::cout << "6. 保存库存\n";
    std::cout << "7. 添加食物\n";
    std::cout << "8. 修改食物信息\n";
    std::cout << "0. 返回上一页\n";
    std::cout << "M. 返回主菜单\n";
    std::cout << "Q. 退出系统\n";
    std::cout << "请选择：";
}

// 按编号查询食物
void InventoryMenu::queryFood() {
    std::string id;

    std::cout << "请输入食物编号（输入0取消）：";

    if (!std::getline(std::cin, id)) {
        return;
    }

    if (id == "0") {
        std::cout << "已取消查询。\n";
        return;
    }

    Food* food = inventory.findFood(id);

    if (food == nullptr) {
        std::cout << "没有找到该食物，请检查编号。\n";
        return;
    }

    food->displayInfo();
}

// 运行菜单：完成一次操作后重新显示菜单
char InventoryMenu::run() {
    std::string choice;

    while (true) {
        showMenu();

        // 输入流关闭时返回退出请求，避免无限循环
        if (!std::getline(std::cin, choice)) {
            return 'Q';
        }

        if (choice == "1") {
            inventory.showAllFoods();
        } else if (choice == "2") {
            queryFood();
        } else if (choice == "3") {
            changeStock(true);
        } else if (choice == "4") {
            changeStock(false);
        } else if (choice == "5") {
            inventory.showLowStockFoods();
                } else if (choice == "6") {
            inventory.saveToFile(dataFile);
                } else if (choice == "7") {
            addFood();
        } else if (choice == "8") {
            editFood();
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
// 处理库存变化：true为入库，false为出库
void InventoryMenu::changeStock(bool isStockIn) {
    const std::string action = isStockIn ? "入库" : "出库";
    std::string id;

    std::cout << "请输入食物编号（输入0取消）：";

    if (!std::getline(std::cin, id)) {
        return;
    }

    if (id == "0") {
        std::cout << "已取消操作。\n";
        return;
    }

    Food* food = inventory.findFood(id);

    if (food == nullptr) {
        std::cout << "没有找到该食物，请检查编号。\n";
        return;
    }

    food->displayInfo();

    double amount = 0;

    // 输入不合法时，允许重新输入
    while (true) {
        std::cout << "请输入" << action
                  << "数量（单位：" << food->getUnit()
                  << "，输入0取消）：";

        std::string input;

        if (!std::getline(std::cin, input)) {
            return;
        }

        if (input == "0") {
            std::cout << "已取消操作。\n";
            return;
        }

        std::istringstream stream(input);

        if (!(stream >> amount)) {
            std::cout << "输入错误，请输入数字。\n";
            continue;
        }

        stream >> std::ws;

        if (!stream.eof() ||
            !std::isfinite(amount) || amount <= 0) {
            std::cout << "数量必须是大于0的有限数值，"
                         "不能包含其他字符。\n";
            continue;
        }

        if (!isStockIn && amount > food->getStock()) {
            std::cout << "库存不足，当前只有"
                      << food->getStock()
                      << food->getUnit() << "。\n";
            continue;
        }

        if (isStockIn &&
            !std::isfinite(food->getStock() + amount)) {
            std::cout << "数量过大，请重新输入。\n";
            continue;
        }

        break;
    }

    const double newStock = isStockIn
        ? food->getStock() + amount
        : food->getStock() - amount;

    // 确认前展示影响范围，此时尚未修改库存
    std::cout << "\n食物：" << food->getName() << "\n";
    std::cout << "本次" << action << "："
              << amount << food->getUnit() << "\n";
    std::cout << "操作后库存："
              << newStock << food->getUnit() << "\n";

    if (newStock <= food->getWarningStock()) {
        std::cout << "提醒：操作后库存处于预警范围。\n";
    }

    std::cout << "输入Y确认，其他输入取消：";

    std::string answer;

    if (!std::getline(std::cin, answer)) {
        return;
    }

    if (answer != "Y" && answer != "y") {
        std::cout << "已取消操作，库存未改变。\n";
        return;
    }

    bool success = isStockIn
        ? inventory.stockIn(id, amount)
        : inventory.stockOut(id, amount);

    if (!success) {
        return;
    }

    // 库存变化成功后立即保存
    if (!inventory.saveToFile(dataFile)) {
        std::cout << "注意：内存中的库存已改变，"
                     "但文件保存失败。\n";
        std::cout << "请不要关闭程序，排查后选择"
                     "菜单6重新保存。\n";
    }
}
// 录入并添加一种新食物
void InventoryMenu::addFood() {
    std::cout << "\n========== 添加食物 ==========\n";
    std::cout << "任意输入步骤输入 /cancel 可取消。\n";
    std::cout << "库存和预警值允许填写0。\n";

    // 读取文本：检查空白、分隔符和取消命令
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

            if (value.find_first_not_of(" \t\r\n")
                == std::string::npos) {
                std::cout << "内容不能为空，请重新输入。\n";
                continue;
            }

            if (value.find_first_of("|\r\n")
                != std::string::npos) {
                std::cout << "内容不能包含竖线或换行符。\n";
                continue;
            }

            return true;
        }
    };

    // 读取非负数，拒绝负数和带多余字符的输入
    auto readNumber = [](const std::string& prompt,
                         double& value) -> bool {
        while (true) {
            std::cout << prompt;

            std::string input;

            if (!std::getline(std::cin, input)) {
                return false;
            }

            if (input == "/cancel") {
                std::cout << "已取消添加。\n";
                return false;
            }

            std::istringstream stream(input);

            if (!(stream >> value)) {
                std::cout << "请输入有效数字。\n";
                continue;
            }

            stream >> std::ws;

            if (!stream.eof() ||
                !std::isfinite(value) || value < 0) {
                std::cout << "请输入大于或等于0的有限数值。\n";
                continue;
            }

            return true;
        }
    };

    std::string type;
    std::string id;
    std::string name;
    std::string unit;
    double stock = 0;
    double warningStock = 0;

    // 选择类型
    while (true) {
        if (!readText(
                "选择类型：1.植物类 2.肉类 3.水果类：",
                type)) {
            return;
        }

        if (type == "1" || type == "2" || type == "3") {
            break;
        }

        std::cout << "类型不存在，请输入1、2或3。\n";
    }

    // 输入编号并检查是否重复
    while (true) {
        if (!readText("食物编号（例如F004）：", id)) {
            return;
        }

        if (id == "0" ||
            id.find_first_of(" \t") != std::string::npos) {
            std::cout << "编号不能是0，也不能包含空格或制表符。\n";
            continue;
        }

        if (inventory.findFood(id) != nullptr) {
            std::cout << "该编号已经存在，请换一个编号。\n";
            continue;
        }

        break;
    }

    if (!readText("食物名称：", name)) {
        return;
    }

    if (!readText("计量单位（例如kg）：", unit)) {
        return;
    }

    if (!readNumber("初始库存：", stock)) {
        return;
    }

    if (!readNumber("库存预警值：", warningStock)) {
        return;
    }

    // 根据类型创建子类对象
    std::unique_ptr<Food> food;

    if (type == "1") {
        food = std::make_unique<PlantFood>(
            id, name, stock, unit, warningStock
        );
    } else if (type == "2") {
        food = std::make_unique<MeatFood>(
            id, name, stock, unit, warningStock
        );
    } else {
        food = std::make_unique<FruitFood>(
            id, name, stock, unit, warningStock
        );
    }

    // 确认之前，食物尚未加入库存
    std::cout << "\n请核对新食物信息：\n";
    food->displayInfo();

    std::cout << "输入Y确认添加，其他输入取消：";
    std::string answer;

    if (!std::getline(std::cin, answer)) {
        return;
    }

    if (answer != "Y" && answer != "y") {
        std::cout << "已取消添加，库存未改变。\n";
        return;
    }

    if (!inventory.addFood(std::move(food))) {
        return;
    }

    // 添加成功后自动保存
    if (!inventory.saveToFile(dataFile)) {
        std::cout << "食物已添加到内存，但文件保存失败。\n";
        std::cout << "请不要关闭程序，排查后选择菜单6重新保存。\n";
    }
}
// 修改食物名称和库存预警值
void InventoryMenu::editFood() {
    std::cout << "\n========== 修改食物信息 ==========\n";
    std::cout << "输入 /cancel 可取消，确认前不会修改数据。\n";

    std::string id;
    std::cout << "请输入食物编号：";

    if (!std::getline(std::cin, id)) {
        return;
    }

    if (id == "/cancel") {
        std::cout << "已取消修改。\n";
        return;
    }

    Food* food = inventory.findFood(id);

    if (food == nullptr) {
        std::cout << "没有找到该食物。\n";
        return;
    }

    std::cout << "\n当前信息：\n";
    food->displayInfo();

    // 先把修改内容放在临时变量中
    std::string newName = food->getName();
    double newWarningStock = food->getWarningStock();

    while (true) {
        std::cout << "新名称（直接回车保持原名称）：";

        std::string input;

        if (!std::getline(std::cin, input)) {
            return;
        }

        if (input == "/cancel") {
            std::cout << "已取消修改。\n";
            return;
        }

        if (input.empty()) {
            break;
        }

        if (input.find_first_not_of(" \t\r\n")
            == std::string::npos) {
            std::cout << "名称不能全部是空白字符。\n";
            continue;
        }

        if (input.find_first_of("|\r\n") != std::string::npos) {
            std::cout << "名称不能包含竖线或换行符。\n";
            continue;
        }

        newName = input;
        break;
    }

    while (true) {
        std::cout << "新预警值（单位：" << food->getUnit()
                  << "，直接回车保持原值）：";

        std::string input;

        if (!std::getline(std::cin, input)) {
            return;
        }

        if (input == "/cancel") {
            std::cout << "已取消修改。\n";
            return;
        }

        if (input.empty()) {
            break;
        }

        double value = 0;
        std::istringstream stream(input);

        if (!(stream >> value)) {
            std::cout << "请输入有效数字。\n";
            continue;
        }

        stream >> std::ws;

        if (!stream.eof() ||
            !std::isfinite(value) || value < 0) {
            std::cout << "预警值必须是大于或等于0的有限数值。\n";
            continue;
        }

        newWarningStock = value;
        break;
    }

    // 未实际修改时直接返回
    if (newName == food->getName() &&
        newWarningStock == food->getWarningStock()) {
        std::cout << "信息没有变化。\n";
        return;
    }

        std::cout << "\n请核对修改内容：\n";
            std::cout << "名称：" << food->getName()
              << " → " << newName << "\n";
    std::cout << "预警值：" << food->getWarningStock()
              << " → " << newWarningStock
              << food->getUnit() << "\n";

    std::cout << "输入Y确认修改，其他输入取消：";
    std::string answer;

    if (!std::getline(std::cin, answer)) {
        return;
    }

    if (answer != "Y" && answer != "y") {
        std::cout << "已取消修改，原信息保持不变。\n";
        return;
    }

    food->setName(newName);
    food->setWarningStock(newWarningStock);

    std::cout << "修改成功！\n";

    if (!inventory.saveToFile(dataFile)) {
        std::cout << "内存信息已修改，但文件保存失败。\n";
        std::cout << "请不要关闭程序，排查后选择菜单6重新保存。\n";
    }
}