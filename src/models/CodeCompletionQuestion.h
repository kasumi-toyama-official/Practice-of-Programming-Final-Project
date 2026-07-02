#ifndef CODECOMPLETIONQUESTION_H
#define CODECOMPLETIONQUESTION_H

#include "models/Question.h"
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>

/**
 * @brief 代码补全题的判题模式
 *
 * 设计为可扩展枚举，预留未来多种判题方式：
 * - ReferenceMatch：与标准答案进行文本/规范化比较
 * - CompileRun：编译运行，用测试用例验证输出
 * - KeywordCheck：检查代码片段是否包含要求的关键字或是否包含禁用关键字
 */
enum class CodeCompletionJudgeMode {
    ReferenceMatch = 0,
    CompileRun     = 1,
    KeywordCheck   = 2
};

/**
 * @brief 代码补全题测试用例
 *
 * CompileRun 模式下使用。input 作为程序标准输入，output 为期望输出。
 */
struct CodeCompletionTestCase {
    QString input;
    QString output;

    QJsonObject toJson() const;
    static CodeCompletionTestCase fromJson(const QJsonObject& obj);
};

/**
 * @brief 代码补全题专用数据结构
 *
 * 与通用 Question 分离，避免选择题/编程题/代码补全题字段混在一起。
 * 所有代码补全题相关逻辑建议围绕本结构展开。
 */
struct CodeCompletionQuestion {
    int id = 0;                              // 题目唯一编号
    int chapterId = 0;                       // 所属章节
    Difficulty difficulty = Difficulty::Easy;// 难度

    QString title;                           // 题目标题（如"编程填空：放大器模板类"）
    QString description;                     // 题干描述，可包含输入输出说明
    QString codeTemplate;                    // 完整代码模板，用 {{BLANK}} 标记挖空
    QString blankPlaceholder;                // 挖空处给用户的提示文本
    QString referenceSolution;               // 参考答案/标准代码片段
    int blankCount = 1;                      // 挖空数量（当前主要支持 1 空）

    QVector<CodeCompletionTestCase> testCases; // CompileRun 模式用的测试用例
    CodeCompletionJudgeMode judgeMode = CodeCompletionJudgeMode::ReferenceMatch;

    QStringList requiredKeywords;            // KeywordCheck 模式下必须出现的关键字
    QStringList forbiddenKeywords;           // KeywordCheck 模式下禁止出现的关键字

    QString explanation;                     // 答案解析

    /**
     * @brief 把用户填写的挖空内容拼回完整代码
     *
     * 将 codeTemplate 中的 {{BLANK}} 替换为 userBlank，供编译运行或展示。
     * 如果模板中没有 {{BLANK}}，则直接追加用户答案到模板末尾（兜底行为）。
     */
    QString buildFullCode(const QString& userBlank) const;

    QJsonObject toJson() const;
    static CodeCompletionQuestion fromJson(const QJsonObject& obj);
};

#endif // CODECOMPLETIONQUESTION_H
