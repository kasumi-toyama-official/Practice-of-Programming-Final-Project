#include "Question.h"

#include <QJsonArray>

QJsonObject QuestionTestCase::toJson() const
{
    QJsonObject obj;
    obj["input"] = input;
    obj["output"] = output;
    return obj;
}

QuestionTestCase QuestionTestCase::fromJson(const QJsonObject& obj)
{
    QuestionTestCase tc;
    tc.input = obj["input"].toString();
    tc.output = obj["output"].toString();
    return tc;
}

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

    obj["codeTemplate"] = codeTemplate;
    obj["referenceSolution"] = referenceSolution;
    obj["blankCount"] = blankCount;

    QJsonArray testCasesArr;
    for (const QuestionTestCase& tc : testCases) {
        testCasesArr.append(tc.toJson());
    }
    obj["testCases"] = testCasesArr;

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

    q.codeTemplate = obj["codeTemplate"].toString();
    q.referenceSolution = obj["referenceSolution"].toString();
    q.blankCount = obj["blankCount"].toInt(1);

    QJsonArray testCasesArr = obj["testCases"].toArray();
    for (const auto& val : testCasesArr) {
        if (val.isObject()) {
            q.testCases.append(QuestionTestCase::fromJson(val.toObject()));
        }
    }

    return q;
}
