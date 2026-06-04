#include "ChapterArchive.h"

ChapterArchive ChapterArchive::createEmpty(int chapterId)
{
    ChapterArchive a;
    a.chapterId = chapterId;
    a.hasActiveArchive = false;
    return a;
}

QJsonObject ChapterArchive::toJson() const
{
    QJsonObject obj;
    obj["chapterId"] = chapterId;
    obj["playerData"] = playerData;
    obj["enemyData"] = enemyData;

    QJsonArray ids;
    for (int id : usedQuestionIds)
        ids.append(id);
    obj["usedQuestionIds"] = ids;

    obj["normalRoundCount"] = normalRoundCount;
    obj["hasActiveArchive"] = hasActiveArchive;
    return obj;
}

ChapterArchive ChapterArchive::fromJson(const QJsonObject& obj)
{
    ChapterArchive a;
    a.chapterId = obj["chapterId"].toInt(0);
    a.playerData = obj["playerData"].toObject();
    a.enemyData = obj["enemyData"].toObject();

    QJsonArray ids = obj["usedQuestionIds"].toArray();
    for (const auto& v : ids)
        a.usedQuestionIds.insert(v.toInt());

    a.normalRoundCount = obj["normalRoundCount"].toInt(0);
    a.hasActiveArchive = obj["hasActiveArchive"].toBool(false);
    return a;
}
