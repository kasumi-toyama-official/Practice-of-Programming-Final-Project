#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "models/ChapterArchive.h"
#include <QMap>
#include <QString>

/**
 * @brief 存档管理器
 *
 * 负责章存档的保存、读取、删除和状态查询。
 * 每章对应一个独立的 JSON 存档文件，存放在指定的存档目录中。
 */
class SaveManager {
public:
    /**
     * @brief 设置存档文件夹路径
     * @param path 存档目录的绝对路径或相对路径
     * @note 通常在应用启动时调用一次，如使用 QStandardPaths::AppDataLocation
     */
    void setSaveDirectory(const QString& path);

    /**
     * @brief 保存某章存档到 JSON 文件
     * @param archive 要保存的章节存档
     * @return 保存成功返回 true
     */
    bool saveChapterArchive(const ChapterArchive& archive);

    /**
     * @brief 读取某章存档
     * @param chapterId 章节编号
     * @return 读取到的存档。若文件不存在或解析失败，返回 createEmpty(chapterId)
     */
    ChapterArchive loadChapterArchive(int chapterId);

    /**
     * @brief 删除某章存档文件
     * @param chapterId 章节编号
     * @return 删除成功返回 true（文件不存在也视为成功）
     */
    bool deleteChapterArchive(int chapterId);

    /**
     * @brief 检查某章是否存在有效存档
     * @param chapterId 章节编号
     * @return true 如果存档文件存在且 hasActiveArchive 为 true
     */
    bool hasArchive(int chapterId) const;

    /**
     * @brief 加载所有章节的存档状态
     * @return 章节编号 -> 是否有有效存档 的映射
     * @note 用于章节选择界面显示"继续"标记
     */
    QMap<int, bool> loadAllArchiveStatus();

private:
    QString saveDir;

    /**
     * @brief 获取指定章节的存档文件完整路径
     * @param chapterId 章节编号
     * @return 文件路径，如 "<saveDir>/chapter_1.json"
     */
    QString getArchiveFilePath(int chapterId) const;
};

#endif // SAVEMANAGER_H
