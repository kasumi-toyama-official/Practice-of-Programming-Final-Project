#include "QuestionBank.h"
#include "../utils/JsonUtils.h"
#include "../utils/RandomUtils.h"

#include <QJsonArray>
#include <QJsonObject>

bool QuestionBank::loadChapter(int chapterId, const QString& jsonPath)
{
    auto optObj = JsonUtils::readJsonObject(jsonPath);
    if (!optObj.has_value()) {
        return false;
    }

    QJsonObject rootObj = optObj.value();
    QJsonArray questionsArr = rootObj["questions"].toArray();
    if (questionsArr.isEmpty()) {
        // 题库为空也视为加载成功（只是没有题目）
        chapterQuestions[chapterId] = QVector<Question>();
        return true;
    }

    QVector<Question> loadedQuestions;
    loadedQuestions.reserve(questionsArr.size());

    for (const auto& val : questionsArr) {
        if (!val.isObject()) {
            continue;
        }
        QJsonObject qObj = val.toObject();
        Question q = Question::fromJson(qObj);

        // 校验：题目中的 chapterId 应与传入的 chapterId 一致
        // 不一致时以文件内容为准，但给出容错
        if (q.chapterId == 0) {
            q.chapterId = chapterId;
        }

        loadedQuestions.append(q);
    }

    chapterQuestions[chapterId] = loadedQuestions;
    return true;
}

void QuestionBank::unloadChapter(int chapterId)
{
    chapterQuestions.remove(chapterId);
}

std::optional<Question> QuestionBank::drawQuestion(
    int chapterId,
    const GameConfig& config,
    const QSet<int>& usedIds
)
{
    int difficulty = config.rollDifficulty();
    return drawQuestion(chapterId, difficulty, usedIds);
}

std::optional<Question> QuestionBank::drawQuestion(
    int chapterId,
    int difficulty,
    const QSet<int>& usedIds
)
{
    if (!chapterQuestions.contains(chapterId)) {
        return std::nullopt;
    }

    const QVector<Question>& allQuestions = chapterQuestions[chapterId];

    // 筛选：指定难度 且 ID 不在 usedIds 中
    QVector<const Question*> candidates;
    candidates.reserve(allQuestions.size());

    Difficulty targetDiff = static_cast<Difficulty>(difficulty);
    for (const Question& q : allQuestions) {
        if (q.difficulty == targetDiff && !usedIds.contains(q.id)) {
            candidates.append(&q);
        }
    }

    if (candidates.isEmpty()) {
        return std::nullopt;
    }

    // 随机选择一道
    int index = RandomUtils::randomInt(0, candidates.size() - 1);
    return *candidates[index];
}

std::optional<Question> QuestionBank::getQuestionById(int chapterId, int questionId) const
{
    auto it = chapterQuestions.find(chapterId);
    if (it == chapterQuestions.end()) {
        return std::nullopt;
    }

    for (const Question& q : it.value()) {
        if (q.id == questionId) {
            return q;
        }
    }

    return std::nullopt;
}

int QuestionBank::getChapterQuestionCount(int chapterId) const
{
    auto it = chapterQuestions.find(chapterId);
    if (it == chapterQuestions.end()) {
        return 0;
    }
    return it.value().size();
}

int QuestionBank::getDifficultyQuestionCount(int chapterId, int difficulty) const
{
    auto it = chapterQuestions.find(chapterId);
    if (it == chapterQuestions.end()) {
        return 0;
    }

    Difficulty targetDiff = static_cast<Difficulty>(difficulty);
    int count = 0;
    for (const Question& q : it.value()) {
        if (q.difficulty == targetDiff) {
            ++count;
        }
    }
    return count;
}

bool QuestionBank::isChapterLoaded(int chapterId) const
{
    return chapterQuestions.contains(chapterId);
}
