#include "CodeJudge.h"
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QDebug>

CodeJudgeResult CodeJudge::compileAndRun(const CodeCompletionQuestion& question,
                                          const QString& userCode,
                                          int timeoutMs)
{
    CodeJudgeResult result;
    result.totalCases = question.testCases.size();

    QString fullCode = question.buildFullCode(userCode);

    QTemporaryDir tmpDir;
    if (!tmpDir.isValid()) {
        result.errorMessage = "无法创建临时目录";
        return result;
    }

    QString cppPath = tmpDir.filePath("code.cpp");
    QFile cppFile(cppPath);
    if (!cppFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        result.errorMessage = "无法写入临时文件";
        return result;
    }
    cppFile.write(fullCode.toUtf8());
    cppFile.close();

    QString exePath = tmpDir.filePath("code.exe");

    QProcess compiler;
    compiler.setWorkingDirectory(tmpDir.path());
    compiler.start("g++", QStringList()
        << "-std=c++17" << "-O2"
        << "-o" << exePath
        << cppPath);
    if (!compiler.waitForFinished(15000)) {
        compiler.kill();
        result.errorMessage = "编译超时";
        return result;
    }
    if (compiler.exitCode() != 0) {
        result.errorMessage = "Compile Error";
        return result;
    }

    for (const CodeCompletionTestCase& tc : question.testCases) {
        QProcess runner;
        runner.setWorkingDirectory(tmpDir.path());
        runner.start(exePath);
        if (!runner.waitForStarted(3000)) {
            result.errorMessage = "程序启动失败";
            return result;
        }
        runner.write(tc.input.toUtf8());
        runner.closeWriteChannel();
        if (!runner.waitForFinished(timeoutMs)) {
            runner.kill();
            result.errorMessage = QString("第%1个测试用例运行超时").arg(result.passedCases + 1);
            return result;
        }
        QString actual = QString::fromUtf8(runner.readAllStandardOutput()).trimmed();
        QString expected = tc.output.trimmed();
        if (actual != expected) {
            result.errorMessage = QString("第%1个测试用例失败\n期望: %2\n实际: %3")
                .arg(result.passedCases + 1).arg(expected).arg(actual);
            return result;
        }
        result.passedCases++;
    }

    result.passed = true;
    return result;
}
