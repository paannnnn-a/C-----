#ifndef PLANTFOOD_H
#define PLANTFOOD_H

#include "Food.h"

// 植物类食物
class PlantFood : public Food {
public:
    // 直接继承Food类的构造函数
    using Food::Food;

    // 重写父类的虚函数
    std::string getType() const override;
    std::string getStorageAdvice() const override;
};

#endif