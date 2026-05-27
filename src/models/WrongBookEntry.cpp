#include "WrongBookEntry.h"

QJsonObject WrongBookEntry::toJson() const
{
    QJsonObject obj;
    obj["chapterId"] = chapterId;
    obj["questionId"] = questionId;
    obj["timestamp"] = timestamp;
    obj["wrongCount"] = wrongCount;
    return obj;
}

WrongBookEntry WrongBookEntry::fromJson(const QJsonObject& obj)
{
    WrongBookEntry entry;
    entry.chapterId = obj["chapterId"].toInt(0);
    entry.questionId = obj["questionId"].toInt(0);
    entry.timestamp = static_cast<qint64>(obj["timestamp"].toDouble(0));
    entry.wrongCount = obj["wrongCount"].toInt(1);
    return entry;
}
