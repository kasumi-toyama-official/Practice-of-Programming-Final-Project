#ifndef RANKINGMANAGER_H
#define RANKINGMANAGER_H

#include "models/RankingEntry.h"
#include <QVector>
#include <QString>
#include <optional>

/**
 * @brief 排行榜管理器
 *
 * 负责学习模式本地排行榜记录的加载、保存和查询。
 */
class RankingManager {
public:
    RankingManager();
    explicit RankingManager(const QString& jsonPath);

    void setFilePath(const QString& jsonPath);

    bool load();
    bool load(const QString& jsonPath);
    bool save() const;

    /**
     * @brief 添加一条战斗记录
     */
    void addRecord(const RankingEntry& entry);

    /**
     * @brief 获取指定章节的所有记录（按时间戳降序）
     */
    QVector<RankingEntry> getRecordsByChapter(int chapterId) const;

    /**
     * @brief 获取指定章节的最佳记录（按总伤害降序，伤害相同按回合数升序）
     */
    std::optional<RankingEntry> getBestRecord(int chapterId) const;

    /**
     * @brief 获取所有记录
     */
    QVector<RankingEntry> getAllRecords() const;

    /**
     * @brief 获取指定章节的记录数量
     */
    int getRecordCountByChapter(int chapterId) const;

private:
    QVector<RankingEntry> records;
    QString filePath;

    void sortByTimestampDesc();
};

#endif // RANKINGMANAGER_H
