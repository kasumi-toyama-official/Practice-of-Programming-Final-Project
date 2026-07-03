#ifndef RANKINGENTRY_H
#define RANKINGENTRY_H

#include <QJsonObject>
#include <QString>

/**
 * @brief 排行榜记录数据结构
 *
 * 记录一场战斗的结算信息，用于学习模式本地排行榜展示。
 */
struct RankingEntry {
    int chapterId = 0;          // 章节编号
    int totalDamage = 0;        // 本局总伤害
    int rounds = 0;             // 回合数
    int correctCount = 0;       // 答对题数
    qint64 timestamp = 0;       // 记录时间戳（秒级 Unix 时间戳）
    bool trophyEarned = false;  // 是否获得奖杯
    bool victory = false;       // 是否胜利

    QJsonObject toJson() const;
    static RankingEntry fromJson(const QJsonObject& obj);
};

#endif // RANKINGENTRY_H
