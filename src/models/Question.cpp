#include "Question.h"

#include <QJsonArray>

QJsonObject Question::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["chapterId"] = chapterId;
    obj["difficulty"] = static_cast<int>(difficulty);
    obj["type"] = static_cast<int>(type);
    obj["questionText"] = questionText;
    obj["correctOptionIndex"] = correctOptionIndex;
    obj["explanation"] = explanation;

    QJsonArray optionsArr;
    for (const QString& opt : options) {
        optionsArr.append(opt);
    }
    obj["options"] = optionsArr;

    QJsonArray blankArr;
    for (const QString& ans : blankAnswers) {
        blankArr.append(ans);
    }
    obj["blankAnswers"] = blankArr;
    obj["codeTemplate"] = codeTemplate;

    return obj;
}

Question Question::fromJson(const QJsonObject& obj)
{
    Question q;

    q.id = obj["id"].toInt(0);
    q.chapterId = obj["chapterId"].toInt(0);
    q.difficulty = static_cast<Difficulty>(obj["difficulty"].toInt(0));
    q.type = static_cast<QuestionType>(obj["type"].toInt(0));
    q.questionText = obj["questionText"].toString();
    q.correctOptionIndex = obj["correctOptionIndex"].toInt(0);
    q.explanation = obj["explanation"].toString();

    QJsonArray optionsArr = obj["options"].toArray();
    for (const auto& val : optionsArr) {
        q.options.append(val.toString());
    }

    QJsonArray blankArr = obj["blankAnswers"].toArray();
    for (const auto& val : blankArr) {
        q.blankAnswers.append(val.toString());
    }
    q.codeTemplate = obj["codeTemplate"].toString();

    return q;
}
