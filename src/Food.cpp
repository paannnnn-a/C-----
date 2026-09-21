#include "../include/Food.h"
#include <iostream>

// 构造函数
Food::Food(const std::string& id,
           const std::string& name,
           double stock,
           const std::string& unit,
           double warningStock)
    : id(id),
      name(name),
      stock(stock),
      unit(unit),
      warningStock(warningStock) {
}

// 食物入库
void Food::addStock(double amount) {
    if (amount <= 0) {
        std::cout << "入库数量必须大于0！" << std::endl;
        return;
    }

    stock += amount;
    std::cout << "入库成功！" << std::endl;
}

// 食物出库
bool Food::reduceStock(double amount) {
    if (amount <= 0) {
        std::cout << "出库数量必须大于0！" << std::endl;
        return false;
    }

    if (amount > stock) {
        std::cout << "库存不足，无法出库！" << std::endl;
        return false;
    }

    stock -= amount;
    std::cout << "出库成功！" << std::endl;
    return true;
}

// 判断库存是否低于或等于预警值
bool Food::isLowStock() const {
    return stock <= warningStock;
}

// 显示食物基本信息
void Food::displayInfo() const {
    std::cout << "食物编号：" << id << std::endl;
    std::cout << "食物名称：" << name << std::endl;
    std::cout << "食物类型：" << getType() << std::endl;
    std::cout << "当前库存：" << stock << unit << std::endl;
    std::cout << "预警值：" << warningStock << unit << std::endl;
    std::cout << "保存要求：" << getStorageAdvice() << std::endl;

    if (isLowStock()) {
        std::cout << "库存状态：库存不足，请及时补充！" << std::endl;
    } else {
        std::cout << "库存状态：正常" << std::endl;
    }
}

// Getter
std::string Food::getId() const {
    return id;
}

std::string Food::getName() const {
    return name;
}

double Food::getStock() const {
    return stock;
}

std::string Food::getUnit() const {
    return unit;
}

double Food::getWarningStock() const {
    return warningStock;
}

// Setter
void Food::setName(const std::string& newName) {
    if (!newName.empty()) {
        name = newName;
    }
}

void Food::setWarningStock(double newWarningStock) {
    if (newWarningStock >= 0) {
        warningStock = newWarningStock;
    }
}