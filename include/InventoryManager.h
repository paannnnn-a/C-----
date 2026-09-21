#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "Food.h"

// 库存管理类：统一管理不同类型的食物
class InventoryManager {
private:
    // 智能指针负责管理食物对象的生命周期
    std::vector<std::unique_ptr<Food>> foods;

public:
    // 添加食物：空指针或编号重复时返回false
    bool addFood(std::unique_ptr<Food> food);

    // 根据编号查找食物，找不到时返回nullptr
    Food* findFood(const std::string& foodId);

    // 入库：成功返回true，失败返回false
    bool stockIn(const std::string& foodId, double amount);

    // 出库：成功返回true，失败返回false
    bool stockOut(const std::string& foodId, double amount);

    // 显示全部食物
    void showAllFoods() const;

    // 显示库存低于或等于预警值的食物
    void showLowStockFoods() const;

        // 保存库存到文件，成功返回true
    bool saveToFile(const std::string& filename) const;

    // 从文件加载库存，成功返回true
    bool loadFromFile(const std::string& filename);
};

#endif