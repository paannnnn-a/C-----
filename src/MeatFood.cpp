#include "../include/MeatFood.h"

// 返回食物类型
std::string MeatFood::getType() const {
    return "肉类";
}

// 返回保存要求
std::string MeatFood::getStorageAdvice() const {
    return "需要冷藏或冷冻保存";
}