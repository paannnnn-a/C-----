#include "../include/FeedingRecord.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

// 构造函数：保存一次实际完成的喂养信息
FeedingRecord::FeedingRecord(
    const std::string& recordId,
    const std::string& planId,
    const std::string& animalId,
    const std::string& foodId,
    double amount,
    const std::string& unit,
    const std::string& keeperName,
    const std::string& completedAt)
    : recordId(recordId),
      planId(planId),
      animalId(animalId),
      foodId(foodId),
      amount(amount),
      unit(unit),
      keeperName(keeperName),
      completedAt(completedAt) {

    if (!std::isfinite(amount) || amount <= 0) {
        throw std::invalid_argument(
            "实际喂养数量必须是大于0的有限数值"
        );
    }
}

std::string FeedingRecord::getRecordId() const {
    return recordId;
}

std::string FeedingRecord::getPlanId() const {
    return planId;
}

std::string FeedingRecord::getAnimalId() const {
    return animalId;
}

std::string FeedingRecord::getFoodId() const {
    return foodId;
}

double FeedingRecord::getAmount() const {
    return amount;
}

std::string FeedingRecord::getUnit() const {
    return unit;
}

std::string FeedingRecord::getKeeperName() const {
    return keeperName;
}

std::string FeedingRecord::getCompletedAt() const {
    return completedAt;
}

// 显示完整喂养记录
void FeedingRecord::display() const {
    std::cout << "记录编号：" << recordId << "\n";
    std::cout << "计划编号：" << planId << "\n";
    std::cout << "动物编号：" << animalId << "\n";
    std::cout << "食物编号：" << foodId << "\n";
    std::cout << "实际喂养量：" << amount << unit << "\n";
    std::cout << "操作人员：" << keeperName << "\n";
    std::cout << "完成时间：" << completedAt << "\n";
}