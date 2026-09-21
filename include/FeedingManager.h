#ifndef FEEDINGMANAGER_H
#define FEEDINGMANAGER_H

#include <string>
#include <vector>

#include "FeedingPlan.h"
#include "InventoryManager.h"
#include "FeedingRecord.h"

// 饲养管理类：管理喂养计划，并与食物库存联动
class FeedingManager {
private:
    // 保存多条喂养计划
    std::vector<FeedingPlan> plans;
        // 保存已完成喂养的历史记录
    std::vector<FeedingRecord> records;

    // 使用已有库存管理器，确保两个模块操作同一份库存
    InventoryManager& inventory;

    // 内部查找计划，找不到时返回nullptr
    FeedingPlan* findPlan(const std::string& planId);

public:
    explicit FeedingManager(InventoryManager& manager);

    // 添加计划：检查重复编号、食物是否存在等
    bool addPlan(const FeedingPlan& plan);

    // 显示全部计划
    void showAllPlans() const;

    // 按日期查询计划，日期格式为YYYY-MM-DD
    void showPlansByDate(const std::string& date) const;

    // 按动物编号查询计划
    void showPlansByAnimal(const std::string& animalId) const;

    // 完成计划：检查状态和库存，扣库存后标记完成
        // 完成计划，并记录操作人员
    bool completePlan(const std::string& planId,
                      const std::string& keeperName);

        // 保存全部计划，包括完成状态
    bool saveToFile(const std::string& filename) const;

    // 读取全部计划，包括完成状态
    bool loadFromFile(const std::string& filename);

        // 显示全部喂养记录
    void showAllRecords() const;

    // 按动物编号查询喂养记录
    void showRecordsByAnimal(
        const std::string& animalId) const;

            // 保存历史喂养记录
    bool saveRecordsToFile(
        const std::string& filename) const;

    // 读取历史喂养记录
    bool loadRecordsFromFile(
        const std::string& filename);
            // 显示今日喂养任务及完成情况
    void showTodayPlans() const;
        // 按食物编号和单位汇总历史喂养消耗
    void showFoodConsumption() const;
        // 按实际完成日期统计喂养消耗
    void showFoodConsumptionByDate(
        const std::string& date) const;
};

#endif