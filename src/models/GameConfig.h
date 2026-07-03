#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QJsonObject>
#include <QString>

 //
 // 存储难度权重和题型权重，支持按权重随机选择难度，

 // 以及检查是否满足奖杯阈值条件。

struct GameConfig {
    int easyWeight = 1;      // 简单题权重
    int mediumWeight = 1;    // 中等题权重
    int hardWeight = 1;      // 困难题权重

    int choiceWeight = 1;    // 选择题权重（当前唯一题型）
    int codeCompletionWeight = 0;     // 代码补全题权重（本次不使用）
    int codeWeight = 0;      // 编程题权重（本次不使用）

    QString presetName;      // 配置名称（如"均衡配置"）

    int rollDifficulty() const;

    bool meetsThreshold() const;

    QJsonObject toJson() const;

    static GameConfig fromJson(const QJsonObject& obj);

    static GameConfig getDefault();
};

#endif // GAMECONFIG_H
