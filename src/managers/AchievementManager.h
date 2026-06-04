#ifndef ACHIEVEMENTMANAGER_H
#define ACHIEVEMENTMANAGER_H

#include "models/Achievement.h"
#include <QVector>
#include <QMap>
#include <QString>

 //
 // 负责成就定义的加载、玩家解锁进度的读写、解锁判定和分类查询。

 //
 // 使用方式：

 // 1. 启动时调用 loadDefinitions() 加载静态成就定义

 // 2. 调用 loadProgress() 加载玩家已解锁的进度

 // 3. 游戏过程中通过 unlockAchievement() / checkChapterClear() 解锁成就

 // 4. 成就墙通过 getAllAchievements() / getAchievementsByCategory() 展示

class AchievementManager {
public:
    bool loadDefinitions(const QString& jsonPath);

    bool loadProgress(const QString& jsonPath);

    bool unlockAchievement(const QString& achievementId);

    bool isUnlocked(const QString& achievementId) const;

    QVector<Achievement> getAllAchievements() const;

    QVector<Achievement> getAchievementsByCategory(AchievementCategory cat) const;

    bool saveProgress();

     //
     // 内部逻辑：

     // - 解锁对应章节的通关成就（如 ch1_clear, ch3_clear）

     // - 若 trophyEarned 为 true，解锁 perfect_clear

    void checkChapterClear(int chapterId, bool trophyEarned);

private:
    QVector<Achievement> achievements;
    QString progressFilePath;

    int findIndex(const QString& achievementId) const;
};

#endif // ACHIEVEMENTMANAGER_H
