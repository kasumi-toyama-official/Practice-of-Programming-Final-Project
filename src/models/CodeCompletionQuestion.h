#ifndef CODECOMPLETIONQUESTION_H
#define CODECOMPLETIONQUESTION_H

#include "models/Question.h"
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>

struct CodeCompletionTestCase {
    QString input;
    QString output;

    QJsonObject toJson() const;
    static CodeCompletionTestCase fromJson(const QJsonObject& obj);
};

struct CodeCompletionQuestion {
    int id = 0;
    int chapterId = 0;
    Difficulty difficulty = Difficulty::Easy;

    QString title;
    QString description;
    QString codeTemplate;
    QString blankPlaceholder;
    QString referenceSolution;
    int blankCount = 1;

    QVector<CodeCompletionTestCase> testCases;

    QString explanation;

    QString buildFullCode(const QString& userBlank) const;

    QJsonObject toJson() const;
    static CodeCompletionQuestion fromJson(const QJsonObject& obj);
};

#endif // CODECOMPLETIONQUESTION_H
