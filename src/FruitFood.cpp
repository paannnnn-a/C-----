#include "../include/FruitFood.h"

// 返回食物类型
std::string FruitFood::getType() const {
    return "水果类";
}

// 返回保存要求
std::string FruitFood::getStorageAdvice() const {
    return "冷藏保存，并注意保鲜";
}