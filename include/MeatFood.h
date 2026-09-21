#ifndef MEATFOOD_H
#define MEATFOOD_H

#include "Food.h"

// 肉类食物
class MeatFood : public Food {
public:
    // 继承Food类的构造函数
    using Food::Food;

    // 重写父类的虚函数
    std::string getType() const override;
    std::string getStorageAdvice() const override;
};

#endif