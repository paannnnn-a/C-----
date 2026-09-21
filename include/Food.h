#ifndef FOOD_H
#define FOOD_H

#include <string>

// 食物抽象基类
class Food {
private:
    std::string id;       // 食物编号
    std::string name;     // 食物名称
    double stock;         // 当前库存
    std::string unit;     // 计量单位
    double warningStock;  // 库存预警值

public:
    // 构造函数
    Food(const std::string& id,
         const std::string& name,
         double stock,
         const std::string& unit,
         double warningStock);

    // 虚析构函数
    virtual ~Food() = default;

    // 库存操作
    void addStock(double amount);
    bool reduceStock(double amount);
    bool isLowStock() const;

    // 显示食物信息
    virtual void displayInfo() const;

    // 纯虚函数，由子类实现
    virtual std::string getType() const = 0;
    virtual std::string getStorageAdvice() const = 0;

    // Getter
    std::string getId() const;
    std::string getName() const;
    double getStock() const;
    std::string getUnit() const;
    double getWarningStock() const;

    // Setter
    void setName(const std::string& newName);
    void setWarningStock(double newWarningStock);
};

#endif