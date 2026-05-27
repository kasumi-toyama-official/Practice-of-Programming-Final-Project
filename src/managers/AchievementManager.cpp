#include "AchievementManager.h"
#include "../utils/JsonUtils.h"

#include <QJsonArray>
#include <QJsonObject>

bool AchievementManager::loadDefinitions(const QString& jsonPath)
{
    auto optArr = JsonUtils::readJsonArray(jsonPath);
    if (!optArr.has_value()) {
        return false;
    }

    QJsonArray arr = optArr.value();
    achievements.clear();
    achievements.reserve(arr.size());

    for (const auto& val : arr) {
        if (!val.isObject()) {
            continue;
        }
        Achievement ach = Achievement::fromJson(val.toObject());
        if (!ach.id.isEmpty()) {
            achievements.append(ach);
        }
    }

    return true;
}

bool AchievementManager::loadProgress(const QString& jsonPath)
{
    progressFilePath = jsonPath;

    // 先把所有成就重置为未解锁
    for (Achievement& ach : achievements) {
        ach.unlocked = false;
    }

    auto optObj = JsonUtils::readJsonObject(jsonPath);
    if (optObj.has_value()) {
        // 对象格式: {"ch1_clear": true, "perfect_clear": true}
        QJsonObject obj = optObj.value();
        for (const QString& key : obj.keys()) {
            if (obj[key].toBool(false)) {
                int idx = findIndex(key);
                if (idx >= 0) {
                    achievements[idx].unlocked = true;
                }
            }
        }
        return true;
    }

    auto optArr = JsonUtils::readJsonArray(jsonPath);
    if (optArr.has_value()) {
        // 数组格式: ["ch1_clear", "perfect_clear"]
        QJsonArray arr = optArr.value();
        for (const auto& val : arr) {
            QString id = val.toString();
            if (!id.isEmpty()) {
                int idx = findIndex(id);
                if (idx >= 0) {
                    achievements[idx].unlocked = true;
                }
            }
        }
        return true;
    }

    // 文件不存在或解析失败：全部视为未解锁，返回 true
    return true;
}

bool AchievementManager::unlockAchievement(const QString& achievementId)
{
    int idx = findIndex(achievementId);
    if (idx < 0) {
        return false; // 成就不存在
    }

    if (achievements[idx].unlocked) {
        return false; // 已经解锁了
    }

    achievements[idx].unlocked = true;
    saveProgress();
    return true;
}

bool AchievementManager::isUnlocked(const QString& achievementId) const
{
    int idx = findIndex(achievementId);
    if (idx < 0) {
        return false;
    }
    return achievements[idx].unlocked;
}

QVector<Achievement> AchievementManager::getAllAchievements() const
{
    return achievements;
}

QVector<Achievement> AchievementManager::getAchievementsByCategory(AchievementCategory cat) const
{
    QVector<Achievement> result;
    for (const Achievement& ach : achievements) {
        if (ach.category == cat) {
            result.append(ach);
        }
    }
    return result;
}

bool AchievementManager::saveProgress()
{
    if (progressFilePath.isEmpty()) {
        return false;
    }

    // 收集所有已解锁的成就ID
    QJsonArray arr;
    for (const Achievement& ach : achievements) {
        if (ach.unlocked) {
            arr.append(ach.id);
        }
    }

    return JsonUtils::writeJsonArray(progressFilePath, arr);
}

void AchievementManager::checkChapterClear(int chapterId, bool trophyEarned)
{
    // 解锁对应章节的通关成就
    QString chapterAchId = QString("ch%1_clear").arg(chapterId);
    unlockAchievement(chapterAchId);

    // 若满足奖杯阈值，解锁完美通关成就
    if (trophyEarned) {
        unlockAchievement("perfect_clear");
    }
}

int AchievementManager::findIndex(const QString& achievementId) const
{
    for (int i = 0; i < achievements.size(); ++i) {
        if (achievements[i].id == achievementId) {
            return i;
        }
    }
    return -1;
}
