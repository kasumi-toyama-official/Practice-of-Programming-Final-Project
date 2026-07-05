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
    if (q.questionText.isEmpty()) {
        // Compatibility with CodeCompletion format (uses "title" + "description")
        QString title = obj["title"].toString();
        QString desc = obj["description"].toString();
        if (!title.isEmpty() && !desc.isEmpty())
            q.questionText = title + "\n" + desc;
        else
            q.questionText = title.isEmpty() ? desc : title;
    }
    q.correctOptionIndex = obj["correctOptionIndex"].toInt(0);
    q.explanation = obj["explanation"].toString();

    QJsonArray optionsArr = obj["options"].toArray();
    for (const auto& val : optionsArr) {
        q.options.append(val.toString());
    }

    q.codeTemplate = obj["codeTemplate"].toString();
    if (!q.codeTemplate.isEmpty() && static_cast<int>(q.type) == static_cast<int>(QuestionType::Choice) && !obj.contains("type")) {
        q.type = QuestionType::CodeCompletion;
    }
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
