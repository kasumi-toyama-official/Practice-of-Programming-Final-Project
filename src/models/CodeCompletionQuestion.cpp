#include "CodeCompletionQuestion.h"

#include <QJsonArray>

QJsonObject CodeCompletionTestCase::toJson() const
{
    QJsonObject obj;
    obj["input"] = input;
    obj["output"] = output;
    return obj;
}

CodeCompletionTestCase CodeCompletionTestCase::fromJson(const QJsonObject& obj)
{
    CodeCompletionTestCase tc;
    tc.input = obj["input"].toString();
    tc.output = obj["output"].toString();
    return tc;
}

QString CodeCompletionQuestion::buildFullCode(const QString& userBlank) const
{
    QString result = codeTemplate;
    if (result.contains("{{BLANK}}")) {
        result.replace("{{BLANK}}", userBlank);
        return result;
    }

    // 兜底：模板里没有占位符时，在末尾追加用户代码
    return result + "\n" + userBlank;
}

QJsonObject CodeCompletionQuestion::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["chapterId"] = chapterId;
    obj["difficulty"] = static_cast<int>(difficulty);
    obj["title"] = title;
    obj["description"] = description;
    obj["codeTemplate"] = codeTemplate;
    obj["blankPlaceholder"] = blankPlaceholder;
    obj["referenceSolution"] = referenceSolution;
    obj["blankCount"] = blankCount;

    QJsonArray testCasesArr;
    for (const CodeCompletionTestCase& tc : testCases) {
        testCasesArr.append(tc.toJson());
    }
    obj["testCases"] = testCasesArr;

    obj["explanation"] = explanation;
    return obj;
}

CodeCompletionQuestion CodeCompletionQuestion::fromJson(const QJsonObject& obj)
{
    CodeCompletionQuestion q;
    q.id = obj["id"].toInt(0);
    q.chapterId = obj["chapterId"].toInt(0);
    q.difficulty = static_cast<Difficulty>(obj["difficulty"].toInt(0));
    q.title = obj["title"].toString();
    q.description = obj["description"].toString();
    q.codeTemplate = obj["codeTemplate"].toString();
    q.blankPlaceholder = obj["blankPlaceholder"].toString();
    q.referenceSolution = obj["referenceSolution"].toString();
    q.blankCount = obj["blankCount"].toInt(1);

    QJsonArray testCasesArr = obj["testCases"].toArray();
    for (const auto& val : testCasesArr) {
        if (val.isObject()) {
            q.testCases.append(CodeCompletionTestCase::fromJson(val.toObject()));
        }
    }

    q.explanation = obj["explanation"].toString();
    return q;
}
