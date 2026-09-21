#include "../include/FeedingPlan.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

// 构造函数：创建一条未完成的喂养计划
FeedingPlan::FeedingPlan(
    const std::string& planId,
    const std::string& animalId,
    const std::string& foodId,
    const std::string& date,
    const std::string& time,
    double amount)
    : planId(planId),
      animalId(animalId),
      foodId(foodId),
      date(date),
      time(time),
      amount(amount),
      completed(false) {

    // 防止创建数量不合法的计划
    if (!std::isfinite(amount) || amount <= 0) {
        throw std::invalid_argument(
            "喂养数量必须是大于0的有限数值"
        );
    }
}

std::string FeedingPlan::getPlanId() const {
    return planId;
}

std::string FeedingPlan::getAnimalId() const {
    return animalId;
}

std::string FeedingPlan::getFoodId() const {
    return foodId;
}

std::string FeedingPlan::getDate() const {
    return date;
}

std::string FeedingPlan::getTime() const {
    return time;
}

double FeedingPlan::getAmount() const {
    return amount;
}

bool FeedingPlan::isCompleted() const {
    return completed;
}

// 标记完成，防止重复完成同一条计划
bool FeedingPlan::markCompleted() {
    if (completed) {
        return false;
    }

    completed = true;
    return true;
}

// 显示计划信息
void FeedingPlan::display() const {
    std::cout << "计划编号：" << planId << "\n";
    std::cout << "动物编号：" << animalId << "\n";
    std::cout << "食物编号：" << foodId << "\n";
    std::cout << "喂养日期：" << date << "\n";
    std::cout << "喂养时间：" << time << "\n";
    std::cout << "计划数量：" << amount
              << "（单位与对应食物一致）\n";
    std::cout << "任务状态："
              << (completed ? "已完成" : "未完成")
              << "\n";
}