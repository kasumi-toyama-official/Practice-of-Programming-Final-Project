#ifndef QUESTION_H
#define QUESTION_H

#include <QString>
#include <QVector>
#include <QJsonObject>

 // 0=简单, 1=中等, 2=困难

enum class Difficulty { Easy = 0, Medium = 1, Hard = 2 };

 // 0=选择题, 1=编程题, 2=代码补全题

enum class QuestionType { Choice = 0, Coding = 1, CodeCompletion = 2 };

/**
 * @brief 代码补全题的测试用例
 *
 * 用于未来接入真实编译判题时，自动验证用户补全的代码是否正确。
 */
struct QuestionTestCase {
    QString input;   // 程序输入
    QString output;  // 期望输出

    QJsonObject toJson() const;
    static QuestionTestCase fromJson(const QJsonObject& obj);
};

/**
 * @brief 题目数据结构
 *
 * 支持选择题、编程题和代码补全题。
 * 代码补全题使用 codeTemplate + referenceSolution + testCases 描述。
 */
struct Question {
    int id = 0;                          // 题目唯一编号
    int chapterId = 0;                   // 所属章节
    Difficulty difficulty = Difficulty::Easy;   // 难度
    QuestionType type = QuestionType::Choice;   // 题型
    QString questionText;                // 题干/题目描述
    QVector<QString> options;            // 选择题选项（A/B/C/D）
    int correctOptionIndex = 0;          // 选择题正确答案索引（0-based）
    QString explanation;                 // 答案解析（用于错题本复习）

    // 代码题相关字段
    QString codeTemplate;                // 完整代码模板（代码补全题用 {{BLANK}} 标记挖空）
    QString referenceSolution;           // 参考答案/标准代码片段
    int blankCount = 1;                  // 挖空数量（代码补全题）
    QVector<QuestionTestCase> testCases; // 编译判题测试用例

    QJsonObject toJson() const;
    static Question fromJson(const QJsonObject& obj);
};

#endif // QUESTION_H
