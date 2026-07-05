#ifndef CODEJUDGE_H
#define CODEJUDGE_H

#include <QString>
#include <QVector>
#include "models/CodeCompletionQuestion.h"

struct CodeJudgeResult {
    bool passed = false;
    QString errorMessage;
    int passedCases = 0;
    int totalCases = 0;
};

class CodeJudge
{
public:
    static CodeJudgeResult compileAndRun(const CodeCompletionQuestion& question,
                                         const QString& userCode,
                                         int timeoutMs = 5000);
};

#endif // CODEJUDGE_H
