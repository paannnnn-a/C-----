#ifndef FEEDINGRECORD_H
#define FEEDINGRECORD_H

#include <string>

// 喂养记录：保存一次实际完成的喂养信息
class FeedingRecord {
private:
    std::string recordId;     // 记录编号
    std::string planId;       // 对应计划编号
    std::string animalId;     // 动物编号
    std::string foodId;       // 食物编号
    double amount;           // 实际喂养数量
    std::string unit;         // 当时的计量单位
    std::string keeperName;   // 操作人员
    std::string completedAt;  // 实际完成时间

public:
    FeedingRecord(const std::string& recordId,
                  const std::string& planId,
                  const std::string& animalId,
                  const std::string& foodId,
                  double amount,
                  const std::string& unit,
                  const std::string& keeperName,
                  const std::string& completedAt);

    std::string getRecordId() const;
    std::string getPlanId() const;
    std::string getAnimalId() const;
    std::string getFoodId() const;
    double getAmount() const;
    std::string getUnit() const;
    std::string getKeeperName() const;
    std::string getCompletedAt() const;

    // 显示完整记录
    void display() const;
};

#endif