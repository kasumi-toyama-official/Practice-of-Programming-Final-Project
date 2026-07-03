#include "RankingEntry.h"

QJsonObject RankingEntry::toJson() const
{
    QJsonObject obj;
    obj["chapterId"] = chapterId;
    obj["totalDamage"] = totalDamage;
    obj["rounds"] = rounds;
    obj["correctCount"] = correctCount;
    obj["timestamp"] = timestamp;
    obj["trophyEarned"] = trophyEarned;
    obj["victory"] = victory;
    return obj;
}

RankingEntry RankingEntry::fromJson(const QJsonObject& obj)
{
    RankingEntry entry;
    entry.chapterId = obj["chapterId"].toInt(0);
    entry.totalDamage = obj["totalDamage"].toInt(0);
    entry.rounds = obj["rounds"].toInt(0);
    entry.correctCount = obj["correctCount"].toInt(0);
    entry.timestamp = static_cast<qint64>(obj["timestamp"].toDouble(0));
    entry.trophyEarned = obj["trophyEarned"].toBool(false);
    entry.victory = obj["victory"].toBool(false);
    return entry;
}
