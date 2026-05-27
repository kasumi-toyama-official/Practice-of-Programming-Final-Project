#ifndef CHAPTERARCHIVE_H
#define CHAPTERARCHIVE_H

#include "PlayerState.h"
#include "EnemyState.h"
#include "GameConfig.h"
#include <QJsonObject>
#include <QVector>

/**
 * @brief 单章存档数据结构
 *
 * 存储某一章节的完整战斗进度，包括玩家/敌人状态、题目配置、
 * 已出现的题目ID列表和普通回合计数。
 */
struct ChapterArchive {
    int chapterId = 0;                  // 章节编号
    PlayerState player;                 // 玩家当前属性
    EnemyState enemy;                   // 敌人当前属性
    GameConfig config;                  // 本局使用的题目配置
    QVector<int> usedQuestionIds;       // 本局已出现过的题目ID列表
    int normalRoundCount = 0;           // 已进行的普通回合数
    bool hasActiveArchive = false;      // 是否有有效存档

    /**
     * @brief 检查当前配置是否满足奖杯阈值条件
     * @return true 如果 config.meetsThreshold() 返回 true
     */
    bool meetsTrophyCondition() const;

    /**
     * @brief 生成一个空存档（表示无存档状态）
     * @param chapterId 章节编号
     * @return 标记为无效的 ChapterArchive
     */
    static ChapterArchive createEmpty(int chapterId);

    /**
     * @brief 序列化为 JSON 对象
     */
    QJsonObject toJson() const;

    /**
     * @brief 从 JSON 对象解析
     */
    static ChapterArchive fromJson(const QJsonObject& obj);
};

#endif // CHAPTERARCHIVE_H
