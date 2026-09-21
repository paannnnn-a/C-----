#ifndef FEEDINGMENU_H
#define FEEDINGMENU_H

#include "FeedingManager.h"

// 喂养菜单：负责显示选项和接收用户输入
class FeedingMenu {
private:
    // 使用已有的饲养管理器
    FeedingManager& feeding;

    // 显示菜单
    void showMenu() const;

    // 录入新的喂养计划
    void addPlan();

    // 按日期查询计划
    void queryByDate();

    // 按动物编号查询计划
    void queryByAnimal();

    // 确认后执行喂养计划
    void completePlan();

public:
    explicit FeedingMenu(FeedingManager& manager);

    // 返回 '0'：上一页，'M'：主菜单，'Q'：退出请求
    char run();
};

#endif