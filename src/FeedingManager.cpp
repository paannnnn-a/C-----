#include "../include/FeedingManager.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <ctime>
#include <map>
// 使用已有的库存管理器
FeedingManager::FeedingManager(InventoryManager& manager)
    : inventory(manager) {
}

// 根据编号查找计划，仅供内部使用
FeedingPlan* FeedingManager::findPlan(
    const std::string& planId) {

    for (auto& plan : plans) {
        if (plan.getPlanId() == planId) {
            return &plan;
        }
    }

    return nullptr;
}

// 添加喂养计划
bool FeedingManager::addPlan(const FeedingPlan& plan) {
    // 编号不能为空，也不能包含空白或文件分隔符
    const auto validId = [](const std::string& id) {
        return !id.empty() &&
               id.find_first_of(" \t\r\n|") == std::string::npos;
    };

    if (!validId(plan.getPlanId()) ||
        !validId(plan.getAnimalId()) ||
        !validId(plan.getFoodId())) {
        std::cout << "添加失败：编号不能为空，"
                     "也不能包含空白或竖线。\n";
        return false;
    }

    if (findPlan(plan.getPlanId()) != nullptr) {
        std::cout << "添加失败：计划编号已存在。\n";
        return false;
    }

    if (plan.isCompleted()) {
        std::cout << "添加失败：新计划必须是未完成状态。\n";
        return false;
    }

    if (!std::isfinite(plan.getAmount()) ||
        plan.getAmount() <= 0) {
        std::cout << "添加失败：喂养数量不合法。\n";
        return false;
    }

    Food* food = inventory.findFood(plan.getFoodId());

    if (food == nullptr) {
        std::cout << "添加失败：对应食物不存在，"
                     "请先在库存中添加食物。\n";
        return false;
    }

    plans.push_back(plan);
    std::cout << "喂养计划添加成功！\n";

    // 建立计划不扣库存，库存不足时只提醒
    if (food->getStock() < plan.getAmount()) {
        std::cout << "提醒：当前库存不足以完成这条计划，"
                     "请在喂养前补充。\n";
    }

    return true;
}

// 显示全部计划
void FeedingManager::showAllPlans() const {
    if (plans.empty()) {
        std::cout << "当前没有喂养计划。\n";
        return;
    }

    for (const auto& plan : plans) {
        plan.display();
        std::cout << "----------------------------------\n";
    }
}

// 按日期查询
void FeedingManager::showPlansByDate(
    const std::string& date) const {

    bool found = false;

    for (const auto& plan : plans) {
        if (plan.getDate() == date) {
            plan.display();
            std::cout << "----------------------------------\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "该日期没有喂养计划。\n";
    }
}

// 按动物编号查询
void FeedingManager::showPlansByAnimal(
    const std::string& animalId) const {

    bool found = false;

    for (const auto& plan : plans) {
        if (plan.getAnimalId() == animalId) {
            plan.display();
            std::cout << "----------------------------------\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "该动物没有喂养计划。\n";
    }
}

// 完成计划：成功扣库存后才标记完成
// 完成喂养，并生成历史记录
bool FeedingManager::completePlan(
    const std::string& planId,
    const std::string& keeperName) {

    if (keeperName.empty() ||
        keeperName.find_first_not_of(" \t\r\n")
            == std::string::npos ||
        keeperName.find_first_of("|\r\n")
            != std::string::npos) {
        std::cout << "操作失败：操作人员不能为空，"
                     "也不能包含竖线或换行符。\n";
        return false;
    }

    FeedingPlan* plan = findPlan(planId);

    if (plan == nullptr) {
        std::cout << "操作失败：没有找到该计划。\n";
        return false;
    }

    if (plan->isCompleted()) {
        std::cout << "操作失败：该计划已经完成，"
                     "不能重复扣库存。\n";
        return false;
    }

    Food* food = inventory.findFood(plan->getFoodId());

    if (food == nullptr) {
        std::cout << "操作失败：计划对应的食物不存在。\n";
        return false;
    }

    if (food->getStock() < plan->getAmount()) {
        std::cout << "操作失败：库存不足，计划仍未完成。\n";
        return false;
    }

    // 一条计划只允许完成一次，记录编号由计划编号生成
    const std::string recordId = "FR-" + planId;

    for (const auto& record : records) {
        if (record.getRecordId() == recordId ||
            record.getPlanId() == planId) {
            std::cout << "操作失败：该计划已有喂养记录，"
                         "请检查计划状态。\n";
            return false;
        }
    }

    // 获取电脑当前的本地时间
    std::time_t now = std::time(nullptr);

    if (now == static_cast<std::time_t>(-1)) {
        std::cout << "操作失败：无法获取当前时间。\n";
        return false;
    }

    const std::tm* localTime = std::localtime(&now);

    if (localTime == nullptr) {
        std::cout << "操作失败：无法转换当前时间。\n";
        return false;
    }

    std::ostringstream timeStream;
    timeStream << std::put_time(
        localTime, "%Y-%m-%d %H:%M:%S"
    );

    // 先准备记录；若后面的出库失败，则移除这条记录
    records.emplace_back(
        recordId,
        plan->getPlanId(),
        plan->getAnimalId(),
        plan->getFoodId(),
        plan->getAmount(),
        food->getUnit(),
        keeperName,
        timeStream.str()
    );

    if (!inventory.stockOut(
            plan->getFoodId(), plan->getAmount())) {
        records.pop_back();

        std::cout << "喂养未完成，未保留本次记录。\n";
        return false;
    }

    plan->markCompleted();

    std::cout << "喂养计划 " << planId << " 已完成。\n";
    std::cout << "生成记录：" << recordId << "\n";
    std::cout << "操作人员：" << keeperName << "\n";
    std::cout << "完成时间：" << timeStream.str() << "\n";
    std::cout << food->getName() << "剩余库存："
              << food->getStock()
              << food->getUnit() << "\n";

    if (food->isLowStock()) {
        std::cout << "提醒：库存已达到预警范围，请及时补充。\n";
    }

    return true;
}
// 保存全部喂养计划，包括完成状态
bool FeedingManager::saveToFile(
    const std::string& filename) const {

    // 在打开文件前检查字段，避免破坏分隔格式
    for (const auto& plan : plans) {
        if (plan.getPlanId().find_first_of("|\r\n")
                != std::string::npos ||
            plan.getAnimalId().find_first_of("|\r\n")
                != std::string::npos ||
            plan.getFoodId().find_first_of("|\r\n")
                != std::string::npos ||
            plan.getDate().find_first_of("|\r\n")
                != std::string::npos ||
            plan.getTime().find_first_of("|\r\n")
                != std::string::npos) {

            std::cout << "保存失败：计划字段不能包含"
                         "竖线或换行符。\n";
            return false;
        }
    }

    // 覆盖文件，用当前全部计划更新保存内容
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "保存失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    // 保留喂养数量的数值精度
    file << std::setprecision(
        std::numeric_limits<double>::max_digits10
    );

    for (const auto& plan : plans) {
        file << plan.getPlanId() << "|"
             << plan.getAnimalId() << "|"
             << plan.getFoodId() << "|"
             << plan.getDate() << "|"
             << plan.getTime() << "|"
             << plan.getAmount() << "|"
             << (plan.isCompleted() ? 1 : 0) << "\n";
    }

    file.close();

    if (!file) {
        std::cout << "保存失败：计划文件写入"
                     "或关闭发生错误。\n";
        return false;
    }

    std::cout << "喂养计划已保存到："
              << filename << "\n";
    return true;
}
// 读取喂养计划，全部检查通过后才替换当前计划
bool FeedingManager::loadFromFile(
    const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "读取失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    std::vector<FeedingPlan> loadedPlans;
    std::unordered_set<std::string> usedIds;

    // 检查编号格式
    auto validId = [](const std::string& id) -> bool {
        return !id.empty() &&
               id.find_first_of(" \t\r\n|")
                   == std::string::npos;
    };

    // 检查指定范围是否全部为数字
    auto allDigits = [](const std::string& text,
                        std::size_t start,
                        std::size_t count) -> bool {
        for (std::size_t i = start; i < start + count; ++i) {
            if (text[i] < '0' || text[i] > '9') {
                return false;
            }
        }
        return true;
    };

    // 检查日期，包括闰年和每月天数
    auto validDate = [&](const std::string& date) -> bool {
        if (date.size() != 10 ||
            date[4] != '-' || date[7] != '-' ||
            !allDigits(date, 0, 4) ||
            !allDigits(date, 5, 2) ||
            !allDigits(date, 8, 2)) {
            return false;
        }

        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (year < 1 || month < 1 || month > 12) {
            return false;
        }

        int days[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        if (year % 400 == 0 ||
            (year % 4 == 0 && year % 100 != 0)) {
            days[1] = 29;
        }

        return day >= 1 && day <= days[month - 1];
    };

    // 检查24小时制时间
    auto validTime = [&](const std::string& time) -> bool {
        if (time.size() != 5 || time[2] != ':' ||
            !allDigits(time, 0, 2) ||
            !allDigits(time, 3, 2)) {
            return false;
        }

        int hour = std::stoi(time.substr(0, 2));
        int minute = std::stoi(time.substr(3, 2));

        return hour <= 23 && minute <= 59;
    };

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;

        // 兼容UTF-8 BOM
        if (lineNumber == 1 &&
            line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

        // 兼容不同系统的换行格式
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        // 拆分字段，并保留空字段
        std::vector<std::string> fields;
        std::size_t start = 0;

        while (true) {
            std::size_t pos = line.find('|', start);

            if (pos == std::string::npos) {
                fields.push_back(line.substr(start));
                break;
            }

            fields.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }

        if (fields.size() != 7) {
            std::cout << "读取失败：第" << lineNumber
                      << "行必须包含7个字段。\n";
            return false;
        }

        if (!validId(fields[0]) ||
            !validId(fields[1]) ||
            !validId(fields[2])) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的编号格式不合法。\n";
            return false;
        }

        if (!usedIds.insert(fields[0]).second) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的计划编号重复。\n";
            return false;
        }

        if (!validDate(fields[3]) || !validTime(fields[4])) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的日期或时间不合法。\n";
            return false;
        }

        double amount = 0;
        std::istringstream stream(fields[5]);

        if (!(stream >> amount)) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的喂养数量不是有效数字。\n";
            return false;
        }

        stream >> std::ws;

        if (!stream.eof() ||
            !std::isfinite(amount) || amount <= 0) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的喂养数量不合法。\n";
            return false;
        }

        if (fields[6] != "0" && fields[6] != "1") {
            std::cout << "读取失败：第" << lineNumber
                      << "行的完成状态必须是0或1。\n";
            return false;
        }

        if (inventory.findFood(fields[2]) == nullptr) {
            std::cout << "读取失败：第" << lineNumber
                      << "行对应的食物不存在。\n";
            return false;
        }

        FeedingPlan plan(
            fields[0], fields[1], fields[2],
            fields[3], fields[4], amount
        );

        // 只恢复状态，不调用扣库存的completePlan()
        if (fields[6] == "1") {
            plan.markCompleted();
        }

        loadedPlans.push_back(plan);
    }

    if (file.bad() || (file.fail() && !file.eof())) {
        std::cout << "读取失败：计划文件读取过程中发生错误。\n";
        return false;
    }

    // 全部成功后才替换，防止读取一半破坏原计划
    plans = std::move(loadedPlans);

    std::cout << "喂养计划读取成功，共加载 "
              << plans.size() << "条计划。\n";
    return true;
}
// 显示全部喂养记录
void FeedingManager::showAllRecords() const {
    if (records.empty()) {
        std::cout << "当前没有喂养记录。\n";
        return;
    }

    for (const auto& record : records) {
        record.display();
        std::cout << "----------------------------------\n";
    }
}

// 按动物编号查询记录
void FeedingManager::showRecordsByAnimal(
    const std::string& animalId) const {

    bool found = false;

    for (const auto& record : records) {
        if (record.getAnimalId() == animalId) {
            record.display();
            std::cout << "----------------------------------\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "该动物没有喂养记录。\n";
    }
}
// 保存全部历史喂养记录
bool FeedingManager::saveRecordsToFile(
    const std::string& filename) const {

    // 检查文本字段，避免破坏文件格式
    auto validField = [](const std::string& text) -> bool {
        return !text.empty() &&
               text.find_first_of("|\r\n")
                   == std::string::npos;
    };

    // 先检查全部记录，再打开文件
    for (const auto& record : records) {
        if (!validField(record.getRecordId()) ||
            !validField(record.getPlanId()) ||
            !validField(record.getAnimalId()) ||
            !validField(record.getFoodId()) ||
            !validField(record.getUnit()) ||
            !validField(record.getKeeperName()) ||
            !validField(record.getCompletedAt())) {

            std::cout << "保存失败：记录字段不能为空，"
                         "也不能包含竖线或换行符。\n";
            return false;
        }

        if (!std::isfinite(record.getAmount()) ||
            record.getAmount() <= 0) {
            std::cout << "保存失败：记录中的喂养数量不合法。\n";
            return false;
        }
    }

    // 用当前全部记录覆盖文件
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "保存失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    file << std::setprecision(
        std::numeric_limits<double>::max_digits10
    );

    for (const auto& record : records) {
        file << record.getRecordId() << "|"
             << record.getPlanId() << "|"
             << record.getAnimalId() << "|"
             << record.getFoodId() << "|"
             << record.getAmount() << "|"
             << record.getUnit() << "|"
             << record.getKeeperName() << "|"
             << record.getCompletedAt() << "\n";
    }

    file.close();

    if (!file) {
        std::cout << "保存失败：记录文件写入"
                     "或关闭发生错误。\n";
        return false;
    }

    std::cout << "喂养记录已保存到："
              << filename << "\n";
    return true;
}
// 读取历史记录，全部检查成功后才替换内存中的记录
bool FeedingManager::loadRecordsFromFile(
    const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "读取失败：无法打开文件 "
                  << filename << "\n";
        return false;
    }

    std::vector<FeedingRecord> loadedRecords;
    std::unordered_set<std::string> usedRecordIds;
    std::unordered_set<std::string> usedPlanIds;

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;

        // 兼容UTF-8 BOM
        if (lineNumber == 1 &&
            line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        // 拆分字段
        std::vector<std::string> fields;
        std::size_t start = 0;

        while (true) {
            std::size_t pos = line.find('|', start);

            if (pos == std::string::npos) {
                fields.push_back(line.substr(start));
                break;
            }

            fields.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }

        if (fields.size() != 8) {
            std::cout << "读取失败：第" << lineNumber
                      << "行必须包含8个字段。\n";
            return false;
        }

        // 所有字段都必须有内容
        for (const auto& field : fields) {
            if (field.find_first_not_of(" \t\r\n")
                    == std::string::npos ||
                field.find_first_of("\r\n")
                    != std::string::npos) {
                std::cout << "读取失败：第" << lineNumber
                          << "行包含空字段或非法换行。\n";
                return false;
            }
        }

        // 前四个字段是编号，不允许包含空白
        for (int i = 0; i < 4; ++i) {
            if (fields[i].find_first_of(" \t\r\n")
                    != std::string::npos) {
                std::cout << "读取失败：第" << lineNumber
                          << "行的编号包含空白。\n";
                return false;
            }
        }

        // 当前设计中，每条计划只能产生一条记录
        if (!usedRecordIds.insert(fields[0]).second ||
            !usedPlanIds.insert(fields[1]).second) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的记录编号或计划编号重复。\n";
            return false;
        }

        double amount = 0;
        std::istringstream amountStream(fields[4]);

        if (!(amountStream >> amount)) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的数量不是有效数字。\n";
            return false;
        }

        amountStream >> std::ws;

        if (!amountStream.eof() ||
            !std::isfinite(amount) || amount <= 0) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的数量不合法。\n";
            return false;
        }

        // 校验完成时间的固定格式
        const std::string& timestamp = fields[7];
        bool validTime = timestamp.size() == 19;

        if (validTime) {
            for (std::size_t i = 0; i < timestamp.size(); ++i) {
                if (i == 4 || i == 7) {
                    validTime = timestamp[i] == '-';
                } else if (i == 10) {
                    validTime = timestamp[i] == ' ';
                } else if (i == 13 || i == 16) {
                    validTime = timestamp[i] == ':';
                } else {
                    validTime =
                        timestamp[i] >= '0' && timestamp[i] <= '9';
                }

                if (!validTime) {
                    break;
                }
            }
        }

        if (validTime) {
            int year = std::stoi(timestamp.substr(0, 4));
            int month = std::stoi(timestamp.substr(5, 2));
            int day = std::stoi(timestamp.substr(8, 2));
            int hour = std::stoi(timestamp.substr(11, 2));
            int minute = std::stoi(timestamp.substr(14, 2));
            int second = std::stoi(timestamp.substr(17, 2));

            validTime = year >= 1 &&
                        month >= 1 && month <= 12 &&
                        hour <= 23 && minute <= 59 &&
                        second <= 59;

            if (validTime) {
                int days[] = {
                    31, 28, 31, 30, 31, 30,
                    31, 31, 30, 31, 30, 31
                };

                if (year % 400 == 0 ||
                    (year % 4 == 0 && year % 100 != 0)) {
                    days[1] = 29;
                }

                validTime = day >= 1 && day <= days[month - 1];
            }
        }

        if (!validTime) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的完成时间不合法。\n";
            return false;
        }

        // 检查记录与已加载的计划是否一致
        FeedingPlan* plan = findPlan(fields[1]);

        if (plan == nullptr ||
            !plan->isCompleted() ||
            plan->getAnimalId() != fields[2] ||
            plan->getFoodId() != fields[3] ||
            plan->getAmount() != amount) {
            std::cout << "读取失败：第" << lineNumber
                      << "行的记录与对应计划不一致。\n";
            return false;
        }

        loadedRecords.emplace_back(
            fields[0], fields[1], fields[2], fields[3],
            amount, fields[5], fields[6], fields[7]
        );
    }

    if (file.bad() || (file.fail() && !file.eof())) {
        std::cout << "读取失败：记录文件读取过程中发生错误。\n";
        return false;
    }

    // 只恢复历史记录，不执行喂养，也不扣库存
    records = std::move(loadedRecords);

    std::cout << "喂养记录读取成功，共加载 "
              << records.size() << "条记录。\n";
    return true;
}
// 按电脑当前本地日期显示今日任务
void FeedingManager::showTodayPlans() const {
    std::time_t now = std::time(nullptr);

    if (now == static_cast<std::time_t>(-1)) {
        std::cout << "无法获取当前时间。\n";
        return;
    }

    const std::tm* localTime = std::localtime(&now);

    if (localTime == nullptr) {
        std::cout << "无法转换当前日期。\n";
        return;
    }

    std::ostringstream dateStream;
    dateStream << std::put_time(localTime, "%Y-%m-%d");
    const std::string today = dateStream.str();

    int total = 0;
    int completed = 0;

    std::cout << "\n========== 今日喂养任务 ==========\n";
    std::cout << "日期：" << today << "\n";

    for (const auto& plan : plans) {
        if (plan.getDate() != today) {
            continue;
        }

        ++total;

        if (plan.isCompleted()) {
            ++completed;
        }

        plan.display();
        std::cout << "----------------------------------\n";
    }

    if (total == 0) {
        std::cout << "今天没有安排喂养任务。\n";
        return;
    }

    std::cout << "任务总数：" << total << "\n";
    std::cout << "已完成：" << completed << "\n";
    std::cout << "未完成：" << total - completed << "\n";
}
// 根据历史喂养记录统计实际消耗
void FeedingManager::showFoodConsumption() const {
    if (records.empty()) {
        std::cout << "当前没有喂养记录，暂无消耗统计。\n";
        return;
    }

    // 同一食物的不同单位分别统计，避免把kg和g直接相加
    using FoodKey = std::pair<std::string, std::string>;
    std::map<FoodKey, double> totals;

    for (const auto& record : records) {
        FoodKey key(
            record.getFoodId(),
            record.getUnit()
        );

        double newTotal = totals[key] + record.getAmount();

        if (!std::isfinite(newTotal)) {
            std::cout << "统计失败：累计数量超出数值范围。\n";
            return;
        }

        totals[key] = newTotal;
    }

    std::cout << "\n========== 历史喂养消耗统计 ==========\n";
    std::cout << "统计范围：当前已加载的全部喂养记录\n";
    std::cout << "记录条数：" << records.size() << "\n\n";

    for (const auto& item : totals) {
        const std::string& foodId = item.first.first;
        const std::string& unit = item.first.second;

        Food* food = inventory.findFood(foodId);

        std::cout << "食物编号：" << foodId << "\n";

        if (food != nullptr) {
            std::cout << "食物名称：" << food->getName() << "\n";
        } else {
            std::cout << "食物名称：当前库存中未找到\n";
        }

        std::cout << "累计喂养消耗："
                  << item.second << unit << "\n";
        std::cout << "----------------------------------\n";
    }
}
// 按实际完成日期统计食物消耗
void FeedingManager::showFoodConsumptionByDate(
    const std::string& date) const {

    // 检查日期格式
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        std::cout << "日期格式错误，请使用YYYY-MM-DD。\n";
        return;
    }

    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }

        if (date[i] < '0' || date[i] > '9') {
            std::cout << "日期格式错误，请使用YYYY-MM-DD。\n";
            return;
        }
    }

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    if (year < 1 || month < 1 || month > 12) {
        std::cout << "年份或月份不合法。\n";
        return;
    }

    int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (year % 400 == 0 ||
        (year % 4 == 0 && year % 100 != 0)) {
        days[1] = 29;
    }

    if (day < 1 || day > days[month - 1]) {
        std::cout << "该日期不存在。\n";
        return;
    }

    using FoodKey = std::pair<std::string, std::string>;
    std::map<FoodKey, double> totals;
    std::size_t recordCount = 0;

    for (const auto& record : records) {
        // 完成时间格式：YYYY-MM-DD HH:MM:SS
        if (record.getCompletedAt().substr(0, 10) != date) {
            continue;
        }

        FoodKey key(record.getFoodId(), record.getUnit());
        double newTotal = totals[key] + record.getAmount();

        if (!std::isfinite(newTotal)) {
            std::cout << "统计失败：累计数量超出数值范围。\n";
            return;
        }

        totals[key] = newTotal;
        ++recordCount;
    }

    std::cout << "\n========== 每日喂养消耗统计 ==========\n";
    std::cout << "实际完成日期：" << date << "\n";

    if (recordCount == 0) {
        std::cout << "该日期没有喂养记录。\n";
        return;
    }

    std::cout << "喂养记录数：" << recordCount << "\n";

    for (const auto& item : totals) {
        const std::string& foodId = item.first.first;
        const std::string& unit = item.first.second;

        Food* food = inventory.findFood(foodId);

        std::cout << "\n食物编号：" << foodId << "\n";

        if (food != nullptr) {
            std::cout << "食物名称：" << food->getName() << "\n";
        }

        std::cout << "当日喂养消耗："
                  << item.second << unit << "\n";
    }
}