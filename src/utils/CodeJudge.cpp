#include "CodeJudge.h"
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QTemporaryDir>
#include <QDebug>

static QString findGpp()
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString candidate = dir.filePath("tools/mingw/bin/g++.exe");
        if (QFile::exists(candidate)) return QFileInfo(candidate).absoluteFilePath();
        if (!dir.cdUp()) break;
    }
    return "g++";
}

static QString findMingwBin()
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 10; ++i) {
        QString candidate = dir.filePath("tools/mingw/bin/g++.exe");
        if (QFile::exists(candidate)) return QFileInfo(candidate).absolutePath();
        if (!dir.cdUp()) break;
    }
    return QString();
}

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

    QString mingwBin = findMingwBin();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!mingwBin.isEmpty()) {
        QString path = mingwBin + ";" + env.value("PATH");
        env.insert("PATH", path);
    }

    QProcess compiler;
    compiler.setProcessEnvironment(env);
    compiler.setWorkingDirectory(tmpDir.path());
    compiler.start(findGpp(), QStringList()
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
        runner.setProcessEnvironment(env);
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
        QString actual = QString::fromUtf8(runner.readAllStandardOutput()).remove(QChar('\r')).trimmed();
        QString expected = QString(tc.output).remove(QChar('\r')).trimmed();
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
