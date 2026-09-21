#ifndef FRUITFOOD_H
#define FRUITFOOD_H

#include "Food.h"

// 水果类食物
class FruitFood : public Food {
public:
    // 继承Food类的构造函数
    using Food::Food;

    // 重写父类的虚函数
    std::string getType() const override;
    std::string getStorageAdvice() const override;
};

#endif