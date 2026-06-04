#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include "models/Question.h"
#include "models/GameConfig.h"
#include <QMap>
#include <QVector>
#include <QSet>
#include <QString>
#include <optional>

 //
 // 负责从 JSON 文件加载各章节的题库，并提供按难度权重随机抽题的功能。

 // 抽题时会自动排除已使用过的题目，确保一局内不重复出题。

class QuestionBank {
public:
    bool loadChapter(int chapterId, const QString& jsonPath);

    void unloadChapter(int chapterId);

     //
     // 内部会先调用 config.rollDifficulty() 确定难度，再抽取对应难度的题目。

     //
    std::optional<Question> drawQuestion(
        int chapterId,
        const GameConfig& config,
        const QSet<int>& usedIds
    );

     //
     // 由调用方（如 BattleSystem）先通过 config.rollDifficulty() 确定难度，

     // 再将难度值传入此函数精确抽题，避免重复随机。

     //
    std::optional<Question> drawQuestion(
        int chapterId,
        int difficulty,
        const QSet<int>& usedIds
    );

    std::optional<Question> getQuestionById(int chapterId, int questionId) const;

    int getChapterQuestionCount(int chapterId) const;

    int getDifficultyQuestionCount(int chapterId, int difficulty) const;

    bool isChapterLoaded(int chapterId) const;

private:
    // chapterId -> 该章节的题目列表
    QMap<int, QVector<Question>> chapterQuestions;
};

#endif // QUESTIONBANK_H
