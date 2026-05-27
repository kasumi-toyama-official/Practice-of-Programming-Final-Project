#ifndef ACHIEVEMENTMANAGER_H
#define ACHIEVEMENTMANAGER_H

#include "models/Achievement.h"
#include <QVector>
#include <QMap>
#include <QString>

/**
 * @brief 成就管理器
 *
 * 负责成就定义的加载、玩家解锁进度的读写、解锁判定和分类查询。
 *
 * 使用方式：
 * 1. 启动时调用 loadDefinitions() 加载静态成就定义
 * 2. 调用 loadProgress() 加载玩家已解锁的进度
 * 3. 游戏过程中通过 unlockAchievement() / checkChapterClear() 解锁成就
 * 4. 成就墙通过 getAllAchievements() / getAchievementsByCategory() 展示
 */
class AchievementManager {
public:
    /**
     * @brief 从 JSON 文件加载成就定义列表（静态配置）
     * @param jsonPath 成就定义文件路径，如 "data/achievements.json"
     * @return 加载成功返回 true
     */
    bool loadDefinitions(const QString& jsonPath);

    /**
     * @brief 从 JSON 文件加载玩家已解锁的成就进度
     * @param jsonPath 进度文件路径
     * @return 加载成功返回 true（文件不存在视为成功，全部未解锁）
     */
    bool loadProgress(const QString& jsonPath);

    /**
     * @brief 检查并解锁指定ID的成就
     * @param achievementId 成就唯一标识
     * @return 如果之前未解锁，解锁成功并保存进度返回 true；已解锁返回 false
     */
    bool unlockAchievement(const QString& achievementId);

    /**
     * @brief 查询某成就是否已解锁
     * @param achievementId 成就唯一标识
     * @return true 如果已解锁或成就不存在（安全回退）
     */
    bool isUnlocked(const QString& achievementId) const;

    /**
     * @brief 获取所有成就（用于成就墙展示）
     * @return 成就列表的副本
     */
    QVector<Achievement> getAllAchievements() const;

    /**
     * @brief 按分类获取成就
     * @param cat 成就分类
     * @return 该分类下的成就列表
     */
    QVector<Achievement> getAchievementsByCategory(AchievementCategory cat) const;

    /**
     * @brief 保存解锁进度到 JSON 文件
     * @return 写入成功返回 true
     */
    bool saveProgress();

    /**
     * @brief 通关某章后调用，检查并触发相关成就
     * @param chapterId 章节编号
     * @param trophyEarned 是否满足奖杯阈值条件
     *
     * 内部逻辑：
     * - 解锁对应章节的通关成就（如 ch1_clear, ch3_clear）
     * - 若 trophyEarned 为 true，解锁 perfect_clear
     */
    void checkChapterClear(int chapterId, bool trophyEarned);

private:
    QVector<Achievement> achievements;
    QString progressFilePath;

    /**
     * @brief 查找指定ID的成就索引
     * @param achievementId 成就ID
     * @return 索引值，找不到返回 -1
     */
    int findIndex(const QString& achievementId) const;
};

#endif // ACHIEVEMENTMANAGER_H
