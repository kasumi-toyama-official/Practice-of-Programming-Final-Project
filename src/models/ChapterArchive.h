#ifndef CHAPTERARCHIVE_H
#define CHAPTERARCHIVE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QMap>
#include "GameConfig.h"

struct Stats;

struct ChapterArchive {
    int chapterId = 0;
    QJsonObject playerData;          // Stats 的 JSON 快照
    QJsonObject enemyData;
    QMap<int, QSet<int>> usedQuestionIds;  // difficulty(0/1/2) → 已用题目ID集合
    int normalRoundCount = 0;
    bool hasActiveArchive = false;
    GameConfig configData;           // 本局使用的难度配置

    static ChapterArchive createEmpty(int chapterId);
    QJsonObject toJson() const;
    static ChapterArchive fromJson(const QJsonObject& obj);
};

#endif
