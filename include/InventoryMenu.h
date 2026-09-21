#ifndef INVENTORYMENU_H
#define INVENTORYMENU_H

#include <string>
#include "InventoryManager.h"

// 库存菜单：负责用户交互
class InventoryMenu {
private:
    // 使用已有的库存管理器，不另外复制库存
    InventoryManager& inventory;

    // 库存数据文件路径
    std::string dataFile;

    // 显示菜单选项
    void showMenu() const;

    // 查询指定食物
    void queryFood();

        // 录入新食物，选择类型并添加到库存
    void addFood();

    // 修改已有食物的名称和库存预警值
    void editFood();

    // 处理入库或出库
    // true表示入库，false表示出库
    void changeStock(bool isStockIn);

public:
    // 接收库存管理器和数据文件路径
    InventoryMenu(InventoryManager& manager,
                  const std::string& filename);

    // 运行菜单循环
    // 返回 '0'：上一页，'M'：主菜单，'Q'：请求退出
    char run();
};

#endif