#ifndef CHAPTERARCHIVE_H
#define CHAPTERARCHIVE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

struct Stats;

struct ChapterArchive {
    int chapterId = 0;
    QJsonObject playerData;          // Stats 的 JSON 快照
    QJsonObject enemyData;
    QSet<int> usedQuestionIds;
    int normalRoundCount = 0;
    bool hasActiveArchive = false;

    static ChapterArchive createEmpty(int chapterId);
    QJsonObject toJson() const;
    static ChapterArchive fromJson(const QJsonObject& obj);
};

#endif
