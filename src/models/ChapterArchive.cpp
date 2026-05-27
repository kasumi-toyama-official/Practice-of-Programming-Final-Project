#include "ChapterArchive.h"

#include <QJsonArray>

bool ChapterArchive::meetsTrophyCondition() const
{
    return config.meetsThreshold();
}

ChapterArchive ChapterArchive::createEmpty(int chapterId)
{
    ChapterArchive archive;
    archive.chapterId = chapterId;
    archive.hasActiveArchive = false;
    return archive;
}

QJsonObject ChapterArchive::toJson() const
{
    QJsonObject obj;
    obj["chapterId"] = chapterId;
    obj["player"] = player.toJson();
    obj["enemy"] = enemy.toJson();
    obj["config"] = config.toJson();

    QJsonArray usedIdsArr;
    for (int id : usedQuestionIds) {
        usedIdsArr.append(id);
    }
    obj["usedQuestionIds"] = usedIdsArr;

    obj["normalRoundCount"] = normalRoundCount;
    obj["hasActiveArchive"] = hasActiveArchive;

    return obj;
}

ChapterArchive ChapterArchive::fromJson(const QJsonObject& obj)
{
    ChapterArchive archive;
    archive.chapterId = obj["chapterId"].toInt(0);
    archive.player = PlayerState::fromJson(obj["player"].toObject());
    archive.enemy = EnemyState::fromJson(obj["enemy"].toObject());
    archive.config = GameConfig::fromJson(obj["config"].toObject());

    QJsonArray usedIdsArr = obj["usedQuestionIds"].toArray();
    for (const auto& val : usedIdsArr) {
        archive.usedQuestionIds.append(val.toInt());
    }

    archive.normalRoundCount = obj["normalRoundCount"].toInt(0);
    archive.hasActiveArchive = obj["hasActiveArchive"].toBool(false);

    return archive;
}
