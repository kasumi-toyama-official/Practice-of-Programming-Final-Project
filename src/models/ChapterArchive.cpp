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

    // 按难度分组存储已用题目ID
    QJsonObject diffObj;
    for (auto it = usedQuestionIds.begin(); it != usedQuestionIds.end(); ++it) {
        QJsonArray arr;
        for (int id : it.value())
            arr.append(id);
        diffObj[QString::number(it.key())] = arr;
    }
    obj["usedQuestionIds"] = diffObj;

    obj["normalRoundCount"] = normalRoundCount;
    obj["hasActiveArchive"] = hasActiveArchive;
    obj["configData"] = configData.toJson();
    return obj;
}

ChapterArchive ChapterArchive::fromJson(const QJsonObject& obj)
{
    ChapterArchive a;
    a.chapterId = obj["chapterId"].toInt(0);
    a.playerData = obj["playerData"].toObject();
    a.enemyData = obj["enemyData"].toObject();

    // 兼容新旧两种存档格式
    QJsonValue usedVal = obj["usedQuestionIds"];
    if (usedVal.isObject()) {
        // 新格式：{"0": [...], "1": [...], "2": [...]}
        QJsonObject diffObj = usedVal.toObject();
        for (auto it = diffObj.begin(); it != diffObj.end(); ++it) {
            int diff = it.key().toInt();
            QJsonArray arr = it.value().toArray();
            for (const auto& v : arr)
                a.usedQuestionIds[diff].insert(v.toInt());
        }
    } else if (usedVal.isArray()) {
        // 旧格式：[1, 2, 3, ...] — 全放入各难度（保守策略）
        QJsonArray ids = usedVal.toArray();
        for (const auto& v : ids) {
            int id = v.toInt();
            a.usedQuestionIds[0].insert(id);
            a.usedQuestionIds[1].insert(id);
            a.usedQuestionIds[2].insert(id);
        }
    }

    a.normalRoundCount = obj["normalRoundCount"].toInt(0);
    a.hasActiveArchive = obj["hasActiveArchive"].toBool(false);

    if (obj.contains("configData"))
        a.configData = GameConfig::fromJson(obj["configData"].toObject());
    else
        a.configData = GameConfig::getDefault();  // 旧版存档兼容

    return a;
}
