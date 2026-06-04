#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "models/ChapterArchive.h"
#include <QMap>
#include <QString>

 //
 // 负责章存档的保存、读取、删除和状态查询。

 // 每章对应一个独立的 JSON 存档文件，存放在指定的存档目录中。

class SaveManager {
public:
    void setSaveDirectory(const QString& path);

    bool saveChapterArchive(const ChapterArchive& archive);

    ChapterArchive loadChapterArchive(int chapterId);

    bool deleteChapterArchive(int chapterId);

    bool hasArchive(int chapterId) const;

    QMap<int, bool> loadAllArchiveStatus();

private:
    QString saveDir;

    QString getArchiveFilePath(int chapterId) const;
};

#endif // SAVEMANAGER_H
