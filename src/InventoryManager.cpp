#include "../include/InventoryManager.h"

#include "../include/PlantFood.h"
#include "../include/MeatFood.h"
#include "../include/FruitFood.h"

#include <sstream>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <utility>

#include <fstream>
#include <iomanip>
#include <limits>
// 添加食物：检查对象是否有效，以及编号是否重复
bool InventoryManager::addFood(std::unique_ptr<Food> food) {
    if (!food) {
        std::cout << "添加失败：食物对象为空。\n";
        return false;
    }

    if (food->getId().empty()) {
        std::cout << "添加失败：食物编号不能为空。\n";
        return false;
    }

    if (findFood(food->getId()) != nullptr) {
        std::cout << "添加失败：食物编号已存在。\n";
        return false;
    }

    // 将食物对象的所有权转交给库存管理器
    foods.push_back(std::move(food));

    std::cout << "食物添加成功！\n";
    return true;
}

// 根据编号查找食物
Food* InventoryManager::findFood(const std::string& foodId) {
    for (const auto& food : foods) {
        if (food->getId() == foodId) {
            return food.get();
        }
    }

    return nullptr;
}

// 食物入库
bool InventoryManager::stockIn(const std::string& foodId,
                               double amount) {
    // 排除非正数、无穷大和非数字值
    if (!std::isfinite(amount) || amount <= 0) {
        std::cout << "入库失败：数量必须是大于0的有限数值。\n";
        return false;
    }

    Food* food = findFood(foodId);

    if (food == nullptr) {
        std::cout << "入库失败：找不到该食物编号。\n";
        return false;
    }

    if (!std::isfinite(food->getStock() + amount)) {
        std::cout << "入库失败：库存数值过大。\n";
        return false;
    }

    food->addStock(amount);
    return true;
}

// 食物出库
bool InventoryManager::stockOut(const std::string& foodId,
                                double amount) {
    if (!std::isfinite(amount) || amount <= 0) {
        std::cout << "出库失败：数量必须是大于0的有限数值。\n";
        return false;
    }

    Food* food = findFood(foodId);

    if (food == nullptr) {
        std::cout << "出库失败：找不到该食物编号。\n";
        return false;
    }

    // Food::reduceStock内部会检查库存是否足够
    return food->reduceStock(amount);
}

// 显示全部食物
void InventoryManager::showAllFoods() const {
    if (foods.empty()) {
        std::cout << "当前没有食物信息。\n";
        return;
    }

    for (const auto& food : foods) {
        food->displayInfo();
        std::cout << "----------------------------------\n";
    }
}

// 显示库存预警
void InventoryManager::showLowStockFoods() const {
    bool found = false;

    for (const auto& food : foods) {
        if (food->isLowStock()) {
            food->displayInfo();
            std::cout << "----------------------------------\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "当前没有需要补充库存的食物。\n";
    }}
    // 将全部食物保存到文件
bool InventoryManager::saveToFile(
    const std::string& filename) const {

    // 先检查文本字段，避免破坏文件的分隔格式
    for (const auto& food : foods) {
        if (food->getId().find_first_of("|\r\n") != std::string::npos ||
            food->getName().find_first_of("|\r\n") != std::string::npos ||
            food->getUnit().find_first_of("|\r\n") != std::string::npos) {

            std::cout << "保存失败：编号、名称或单位"
                         "不能包含竖线或换行符。\n";
            return false;
        }
    }

    // 打开文件。注意：会覆盖该文件原有内容
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "保存失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    // 保留double类型数值读写所需的精度
    file << std::setprecision(
        std::numeric_limits<double>::max_digits10
    );

    for (const auto& food : foods) {
        file << food->getId() << "|"
             << food->getName() << "|"
             << food->getType() << "|"
             << food->getStock() << "|"
             << food->getUnit() << "|"
             << food->getWarningStock() << "\n";
    }

    // 关闭时也检查写入是否成功
    file.close();

    if (!file) {
        std::cout << "保存失败：文件写入或关闭发生错误。\n";
        return false;
    }

    std::cout << "库存已保存到：" << filename << "\n";
    return true;

}
// 从文件加载库存
// 只有全部数据检查通过后，才替换当前库存
bool InventoryManager::loadFromFile(
    const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "读取失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    // 临时保存读到的食物，避免读取失败破坏原库存
    std::vector<std::unique_ptr<Food>> loadedFoods;

    // 用于检查食物编号是否重复
    std::unordered_set<std::string> usedIds;

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;

        // 兼容文件开头的UTF-8 BOM
        if (lineNumber == 1 &&
            line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

        // 兼容不同系统的换行格式
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // 跳过空行
        if (line.empty()) {
            continue;
        }

        // 按竖线拆分，保留空字段
        std::vector<std::string> fields;
        std::size_t start = 0;

        while (true) {
            std::size_t pos = line.find('|', start);

            if (pos == std::string::npos) {
                fields.push_back(line.substr(start));
                break;
            }

            fields.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }

        if (fields.size() != 6) {
            std::cout << "读取失败：第" << lineNumber
                      << "行必须包含6个字段。\n";
            return false;
        }

        const std::string& id = fields[0];
        const std::string& name = fields[1];
        const std::string& type = fields[2];
        const std::string& unit = fields[4];

        if (id.empty() || name.empty() || unit.empty()) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的编号、名称或单位为空。\n";
            return false;
        }

        if (!usedIds.insert(id).second) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的食物编号重复。\n";
            return false;
        }

        double stock = 0;
        double warningStock = 0;

        // 检查库存：必须是完整、有限且非负的数值
        std::istringstream stockStream(fields[3]);
        if (!(stockStream >> stock)) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的库存不是有效数字。\n";
            return false;
        }

        stockStream >> std::ws;

        if (!stockStream.eof() ||
            !std::isfinite(stock) || stock < 0) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的库存数值不合法。\n";
            return false;
        }

        // 检查预警值
        std::istringstream warningStream(fields[5]);
        if (!(warningStream >> warningStock)) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的预警值不是有效数字。\n";
            return false;
        }

        warningStream >> std::ws;

        if (!warningStream.eof() ||
            !std::isfinite(warningStock) || warningStock < 0) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的预警值不合法。\n";
            return false;
        }

        // 根据保存的类型，重新创建对应的子类对象
        if (type == "植物类") {
            loadedFoods.push_back(
                std::make_unique<PlantFood>(
                    id, name, stock, unit, warningStock
                )
            );
        } else if (type == "肉类") {
            loadedFoods.push_back(
                std::make_unique<MeatFood>(
                    id, name, stock, unit, warningStock
                )
            );
        } else if (type == "水果类") {
            loadedFoods.push_back(
                std::make_unique<FruitFood>(
                    id, name, stock, unit, warningStock
                )
            );
        } else {
            std::cout << "读取失败：第" << lineNumber
                      << "行包含未知食物类型。\n";
            return false;
        }
    }

    if (file.bad() || (file.fail() && !file.eof())) {
        std::cout << "读取失败：文件读取过程中发生错误。\n";
        return false;
    }

    // 全部读取成功后，统一替换原库存
    foods = std::move(loadedFoods);

    std::cout << "库存读取成功，共加载 "
              << foods.size() << "种食物。\n";
    return true;
}