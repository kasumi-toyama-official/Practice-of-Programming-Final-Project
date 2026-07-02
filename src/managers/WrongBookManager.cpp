#include "managers/WrongBookManager.h"
#include "utils/JsonUtils.h"
#include <QJsonArray>
#include <QDateTime>
#include <algorithm>

WrongBookManager::WrongBookManager()
{
}

WrongBookManager::WrongBookManager(const QString& jsonPath)
    : filePath(jsonPath)
{
}

void WrongBookManager::setFilePath(const QString& jsonPath)
{
    filePath = jsonPath;
}

bool WrongBookManager::load()
{
    if (filePath.isEmpty()) {
        return false;
    }
    return load(filePath);
}

bool WrongBookManager::load(const QString& jsonPath)
{
    filePath = jsonPath;
    entries.clear();

    auto arrOpt = JsonUtils::readJsonArray(filePath);
    if (!arrOpt.has_value()) {
        // 文件不存在或解析失败：视为空错题本
        return true;
    }

    const QJsonArray arr = arrOpt.value();
    for (const QJsonValue& val : arr) {
        if (!val.isObject()) {
            continue;
        }
        entries.append(WrongBookEntry::fromJson(val.toObject()));
    }

    sortByTimestampDesc();
    return true;
}

bool WrongBookManager::save() const
{
    if (filePath.isEmpty()) {
        return false;
    }

    QJsonArray arr;
    for (const WrongBookEntry& entry : entries) {
        arr.append(entry.toJson());
    }

    return JsonUtils::writeJsonArray(filePath, arr);
}

void WrongBookManager::addWrongQuestion(int chapterId, int questionId)
{
    int idx = findEntryIndex(chapterId, questionId);
    if (idx >= 0) {
        entries[idx].wrongCount++;
        entries[idx].timestamp = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    } else {
        WrongBookEntry entry;
        entry.chapterId = chapterId;
        entry.questionId = questionId;
        entry.timestamp = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
        entry.wrongCount = 1;
        entries.append(entry);
    }

    sortByTimestampDesc();
}

bool WrongBookManager::removeEntry(int chapterId, int questionId)
{
    int idx = findEntryIndex(chapterId, questionId);
    if (idx < 0) {
        return false;
    }
    entries.removeAt(idx);
    return true;
}

bool WrongBookManager::hasEntry(int chapterId, int questionId) const
{
    return findEntryIndex(chapterId, questionId) >= 0;
}

QVector<WrongBookEntry> WrongBookManager::getAllEntries() const
{
    return entries;
}

QVector<WrongBookEntry> WrongBookManager::getEntriesByChapter(int chapterId) const
{
    QVector<WrongBookEntry> result;
    for (const WrongBookEntry& entry : entries) {
        if (entry.chapterId == chapterId) {
            result.append(entry);
        }
    }
    return result;
}

int WrongBookManager::getEntryCount() const
{
    return entries.size();
}

int WrongBookManager::getEntryCountByChapter(int chapterId) const
{
    int count = 0;
    for (const WrongBookEntry& entry : entries) {
        if (entry.chapterId == chapterId) {
            count++;
        }
    }
    return count;
}

std::optional<WrongBookEntry> WrongBookManager::getEntry(int chapterId, int questionId) const
{
    int idx = findEntryIndex(chapterId, questionId);
    if (idx < 0) {
        return std::nullopt;
    }
    return entries[idx];
}

int WrongBookManager::findEntryIndex(int chapterId, int questionId) const
{
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].chapterId == chapterId && entries[i].questionId == questionId) {
            return i;
        }
    }
    return -1;
}

void WrongBookManager::sortByTimestampDesc()
{
    std::sort(entries.begin(), entries.end(),
              [](const WrongBookEntry& a, const WrongBookEntry& b) {
                  return a.timestamp > b.timestamp;
              });
}
