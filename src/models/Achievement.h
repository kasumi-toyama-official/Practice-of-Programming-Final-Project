#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <QJsonObject>
#include <QString>

enum class AchievementCategory { Learning, Combat };

 //
 // 表示一个成就的定义信息，包含名称、描述、分类、图标和解锁状态。

 // 支持 JSON 序列化，用于成就定义文件和玩家进度存档。

struct Achievement {
    QString id;                         // 唯一标识（如"ch1_clear"）
    QString name;                       // 显示名称（如"初出茅庐"）
    QString description;                // 描述（如"通关第1章"）
    AchievementCategory category = AchievementCategory::Learning; // 分类
    QString iconPath;                   // 图标路径（如":/icons/ach_ch1.png"）
    bool unlocked = false;              // 是否已解锁

    QJsonObject toJson() const;

    static Achievement fromJson(const QJsonObject& obj);
};

#endif // ACHIEVEMENT_H
