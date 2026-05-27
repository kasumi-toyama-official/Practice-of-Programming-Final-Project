#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QJsonObject>
#include <QString>

/**
 * @brief 题目配置数据结构
 *
 * 存储难度权重和题型权重，支持按权重随机选择难度，
 * 以及检查是否满足奖杯阈值条件。
 */
struct GameConfig {
    int easyWeight = 1;      // 简单题权重
    int mediumWeight = 1;    // 中等题权重
    int hardWeight = 1;      // 困难题权重

    int choiceWeight = 1;    // 选择题权重（当前唯一题型）
    int blankWeight = 0;     // 填空题权重（本次不使用）
    int codeWeight = 0;      // 编程题权重（本次不使用）

    QString presetName;      // 配置名称（如"均衡配置"）

    /**
     * @brief 按权重随机选择难度
     * @return 0=Easy, 1=Medium, 2=Hard
     */
    int rollDifficulty() const;

    /**
     * @brief 检查是否满足奖杯阈值条件
     * @return true 如果简单题占比 <= 60% 且选择题占比 >= 80%
     */
    bool meetsThreshold() const;

    /**
     * @brief 序列化为 JSON 对象
     */
    QJsonObject toJson() const;

    /**
     * @brief 从 JSON 对象解析
     */
    static GameConfig fromJson(const QJsonObject& obj);

    /**
     * @brief 返回默认配置（1:1:1）
     */
    static GameConfig getDefault();
};

#endif // GAMECONFIG_H
