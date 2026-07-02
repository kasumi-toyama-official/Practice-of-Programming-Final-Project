#ifndef WRONGBOOKMANAGER_H
#define WRONGBOOKMANAGER_H

#include "models/WrongBookEntry.h"
#include <QVector>
#include <QString>
#include <optional>

/**
 * @brief 错题本管理器
 *
 * 负责错题记录的加载、保存、增删改查。
 * 以 (chapterId, questionId) 作为唯一键，同一题多次答错时累加 wrongCount 并更新 timestamp。
 */
class WrongBookManager {
public:
    WrongBookManager();

    /**
     * @brief 构造时指定错题本 JSON 文件路径
     * @param jsonPath 文件路径，如 "data/progress/wrongbook.json"
     */
    explicit WrongBookManager(const QString& jsonPath);

    /**
     * @brief 设置错题本 JSON 文件路径
     */
    void setFilePath(const QString& jsonPath);

    /**
     * @brief 使用已设置的路径加载错题本
     * @return 加载成功返回 true（文件不存在视为空错题本，也返回 true）
     */
    bool load();

    /**
     * @brief 加载指定路径的错题本
     */
    bool load(const QString& jsonPath);

    /**
     * @brief 保存错题本到 JSON 文件
     */
    bool save() const;

    /**
     * @brief 记录一道错题
     * @param chapterId 章节编号
     * @param questionId 题目编号
     *
     * 若该题已存在，则 wrongCount +1 并刷新时间戳；
     * 若不存在，则新增一条记录。
     */
    void addWrongQuestion(int chapterId, int questionId);

    /**
     * @brief 删除指定错题
     * @return 删除成功返回 true
     */
    bool removeEntry(int chapterId, int questionId);

    /**
     * @brief 检查某题是否已在错题本中
     */
    bool hasEntry(int chapterId, int questionId) const;

    /**
     * @brief 获取所有错题记录（按时间戳降序，最近的在前）
     */
    QVector<WrongBookEntry> getAllEntries() const;

    /**
     * @brief 获取指定章节的所有错题
     */
    QVector<WrongBookEntry> getEntriesByChapter(int chapterId) const;

    /**
     * @brief 获取错题总数
     */
    int getEntryCount() const;

    /**
     * @brief 获取指定章节的错题数量
     */
    int getEntryCountByChapter(int chapterId) const;

    /**
     * @brief 获取指定题目的错题记录
     */
    std::optional<WrongBookEntry> getEntry(int chapterId, int questionId) const;

private:
    QVector<WrongBookEntry> entries;
    QString filePath;

    int findEntryIndex(int chapterId, int questionId) const;
    void sortByTimestampDesc();
};

#endif // WRONGBOOKMANAGER_H
