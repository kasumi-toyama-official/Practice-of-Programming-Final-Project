#include "managers/RankingManager.h"
#include "utils/JsonUtils.h"
#include <QJsonArray>
#include <algorithm>

RankingManager::RankingManager()
{
}

RankingManager::RankingManager(const QString& jsonPath)
    : filePath(jsonPath)
{
}

void RankingManager::setFilePath(const QString& jsonPath)
{
    filePath = jsonPath;
}

bool RankingManager::load()
{
    if (filePath.isEmpty()) return false;
    return load(filePath);
}

bool RankingManager::load(const QString& jsonPath)
{
    filePath = jsonPath;
    records.clear();

    auto arrOpt = JsonUtils::readJsonArray(filePath);
    if (!arrOpt.has_value()) {
        return true; // 文件不存在视为空排行榜
    }

    const QJsonArray arr = arrOpt.value();
    for (const QJsonValue& val : arr) {
        if (!val.isObject()) continue;
        records.append(RankingEntry::fromJson(val.toObject()));
    }

    sortByTimestampDesc();
    return true;
}

bool RankingManager::save() const
{
    if (filePath.isEmpty()) return false;

    QJsonArray arr;
    for (const RankingEntry& entry : records) {
        arr.append(entry.toJson());
    }

    return JsonUtils::writeJsonArray(filePath, arr);
}

void RankingManager::addRecord(const RankingEntry& entry)
{
    records.append(entry);
    sortByTimestampDesc();
}

QVector<RankingEntry> RankingManager::getRecordsByChapter(int chapterId) const
{
    QVector<RankingEntry> result;
    for (const RankingEntry& entry : records) {
        if (entry.chapterId == chapterId) {
            result.append(entry);
        }
    }
    return result;
}

std::optional<RankingEntry> RankingManager::getBestRecord(int chapterId) const
{
    std::optional<RankingEntry> best;
    for (const RankingEntry& entry : records) {
        if (entry.chapterId != chapterId) continue;
        if (!best.has_value()) {
            best = entry;
        } else {
            const RankingEntry& b = best.value();
            if (entry.totalDamage > b.totalDamage ||
                (entry.totalDamage == b.totalDamage && entry.rounds < b.rounds)) {
                best = entry;
            }
        }
    }
    return best;
}

QVector<RankingEntry> RankingManager::getAllRecords() const
{
    return records;
}

int RankingManager::getRecordCountByChapter(int chapterId) const
{
    int count = 0;
    for (const RankingEntry& entry : records) {
        if (entry.chapterId == chapterId) count++;
    }
    return count;
}

void RankingManager::sortByTimestampDesc()
{
    std::sort(records.begin(), records.end(),
              [](const RankingEntry& a, const RankingEntry& b) {
                  return a.timestamp > b.timestamp;
              });
}
