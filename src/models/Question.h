#ifndef QUESTION_H
#define QUESTION_H

#include <QString>
#include <QVector>
#include <QJsonObject>

/**
 * @brief 题目难度枚举
 * 0=简单, 1=中等, 2=困难
 */
enum class Difficulty { Easy = 0, Medium = 1, Hard = 2 };

/**
 * @brief 题目类型枚举
 * 当前只有选择题，保留结构以便后续扩展
 */
enum class QuestionType { Choice = 0 };

/**
 * @brief 题目数据结构
 *
 * 表示一道选择题的完整信息，包含题干、选项、正确答案和解析。
 * 支持 JSON 序列化，用于题库文件的加载和存档中记录已出现的题目。
 */
struct Question {
    int id = 0;                          // 题目唯一编号
    int chapterId = 0;                   // 所属章节
    Difficulty difficulty = Difficulty::Easy;   // 难度
    QuestionType type = QuestionType::Choice;   // 题型（目前固定选择题）
    QString questionText;                // 题干
    QVector<QString> options;            // 选项（A/B/C/D）
    int correctOptionIndex = 0;          // 正确答案索引（0-based）
    QString explanation;                 // 答案解析（用于错题本复习）

    /**
     * @brief 将题目序列化为 JSON 对象
     * @return QJsonObject
     */
    QJsonObject toJson() const;

    /**
     * @brief 从 JSON 对象解析题目
     * @param obj JSON 对象
     * @return 解析后的 Question
     */
    static Question fromJson(const QJsonObject& obj);
};

#endif // QUESTION_H
