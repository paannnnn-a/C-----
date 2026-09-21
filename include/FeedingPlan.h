#ifndef FEEDINGPLAN_H
#define FEEDINGPLAN_H

#include <string>

// 喂养计划：记录某只动物在指定时间需要哪种食物
// 第一版中，一条计划对应一种食物
class FeedingPlan {
private:
    std::string planId;    // 计划编号
    std::string animalId;  // 动物编号，与动物管理模块关联
    std::string foodId;    // 食物编号，与库存管理模块关联
    std::string date;      // 日期，例如2026-10-01
    std::string time;      // 时间，例如08:00
    double amount;        // 计划数量，单位与对应食物一致
    bool completed;       // 是否已经完成

public:
    // 新建计划时，默认状态为未完成
    FeedingPlan(const std::string& planId,
                const std::string& animalId,
                const std::string& foodId,
                const std::string& date,
                const std::string& time,
                double amount);

    std::string getPlanId() const;
    std::string getAnimalId() const;
    std::string getFoodId() const;
    std::string getDate() const;
    std::string getTime() const;
    double getAmount() const;
    bool isCompleted() const;

    // 标记完成；如果已经完成，返回false
    bool markCompleted();

    // 显示计划信息
    void display() const;
};

#endif