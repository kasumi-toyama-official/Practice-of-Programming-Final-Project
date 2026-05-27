#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include "models/Question.h"
#include "models/GameConfig.h"
#include <QMap>
#include <QVector>
#include <QSet>
#include <QString>
#include <optional>

/**
 * @brief 题库管理器
 *
 * 负责从 JSON 文件加载各章节的题库，并提供按难度权重随机抽题的功能。
 * 抽题时会自动排除已使用过的题目，确保一局内不重复出题。
 */
class QuestionBank {
public:
    /**
     * @brief 从 JSON 文件加载指定章节的题库
     * @param chapterId 章节编号（1-based）
     * @param jsonPath 题库文件路径，如 "data/questions/chapter1.json"
     * @return 加载成功返回 true
     */
    bool loadChapter(int chapterId, const QString& jsonPath);

    /**
     * @brief 卸载指定章节的题库，释放内存
     * @param chapterId 章节编号
     */
    void unloadChapter(int chapterId);

    /**
     * @brief 按配置权重随机抽取一道未使用过的题目（兼容接口）
     *
     * 内部会先调用 config.rollDifficulty() 确定难度，再抽取对应难度的题目。
     *
     * @param chapterId 章节编号
     * @param config 题目配置（包含难度权重）
     * @param usedIds 本局已使用过的题目 ID 集合
     * @return 抽到的题目。若该章节该难度的所有题都已用完，返回 std::nullopt
     */
    std::optional<Question> drawQuestion(
        int chapterId,
        const GameConfig& config,
        const QSet<int>& usedIds
    );

    /**
     * @brief 按指定难度随机抽取一道未使用过的题目（推荐接口）
     *
     * 由调用方（如 BattleSystem）先通过 config.rollDifficulty() 确定难度，
     * 再将难度值传入此函数精确抽题，避免重复随机。
     *
     * @param chapterId 章节编号
     * @param difficulty 难度值：0=简单, 1=中等, 2=困难
     * @param usedIds 本局已使用过的题目 ID 集合
     * @return 抽到的题目。若该章节该难度的所有题都已用完，返回 std::nullopt
     */
    std::optional<Question> drawQuestion(
        int chapterId,
        int difficulty,
        const QSet<int>& usedIds
    );

    /**
     * @brief 根据 ID 获取指定章节的某道题目（用于错题本重做等场景）
     * @param chapterId 章节编号
     * @param questionId 题目唯一编号
     * @return 对应的题目。若找不到则返回 std::nullopt
     */
    std::optional<Question> getQuestionById(int chapterId, int questionId) const;

    /**
     * @brief 获取某章节的题目总数
     * @param chapterId 章节编号
     * @return 题目数量。若章节未加载则返回 0
     */
    int getChapterQuestionCount(int chapterId) const;

    /**
     * @brief 获取某章节指定难度的题目数量
     * @param chapterId 章节编号
     * @param difficulty 难度值：0=简单, 1=中等, 2=困难
     * @return 该难度的题目数量。若章节未加载则返回 0
     */
    int getDifficultyQuestionCount(int chapterId, int difficulty) const;

    /**
     * @brief 检查某章节是否已加载
     * @param chapterId 章节编号
     * @return true 如果已加载
     */
    bool isChapterLoaded(int chapterId) const;

private:
    // chapterId -> 该章节的题目列表
    QMap<int, QVector<Question>> chapterQuestions;
};

#endif // QUESTIONBANK_H
