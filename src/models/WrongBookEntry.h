#ifndef WRONGBOOKENTRY_H
#define WRONGBOOKENTRY_H

#include <QJsonObject>

 //
 // 记录玩家答错的题目信息，支持 JSON 序列化用于错题本持久化。

 // 同一题多次答错时，wrongCount 累加。

struct WrongBookEntry {
    int chapterId = 0;      // 所属章节
    int questionId = 0;     // 题目ID
    qint64 timestamp = 0;   // 做错的时间戳（秒级 Unix 时间戳）
    int wrongCount = 1;     // 错误次数（重复做错同一题则累加）

    QJsonObject toJson() const;

    static WrongBookEntry fromJson(const QJsonObject& obj);
};

#endif // WRONGBOOKENTRY_H
