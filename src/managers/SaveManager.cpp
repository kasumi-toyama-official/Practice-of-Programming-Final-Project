#include "SaveManager.h"
#include "../utils/JsonUtils.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

void SaveManager::setSaveDirectory(const QString& path)
{
    saveDir = path;
    JsonUtils::ensureDirectoryExists(saveDir);
}

bool SaveManager::saveChapterArchive(const ChapterArchive& archive)
{
    if (saveDir.isEmpty()) {
        return false;
    }

    QString filePath = getArchiveFilePath(archive.chapterId);
    return JsonUtils::writeJsonObject(filePath, archive.toJson());
}

ChapterArchive SaveManager::loadChapterArchive(int chapterId)
{
    QString filePath = getArchiveFilePath(chapterId);

    auto optObj = JsonUtils::readJsonObject(filePath);
    if (!optObj.has_value()) {
        return ChapterArchive::createEmpty(chapterId);
    }

    ChapterArchive archive = ChapterArchive::fromJson(optObj.value());

    // 校验：如果存档中的 chapterId 与请求的不一致，以请求为准
    if (archive.chapterId != chapterId) {
        archive.chapterId = chapterId;
    }

    return archive;
}

bool SaveManager::deleteChapterArchive(int chapterId)
{
    QString filePath = getArchiveFilePath(chapterId);
    QFile file(filePath);

    if (!file.exists()) {
        return true; // 文件不存在视为已删除
    }

    return file.remove();
}

bool SaveManager::hasArchive(int chapterId) const
{
    QString filePath = getArchiveFilePath(chapterId);

    if (!QFile::exists(filePath)) {
        return false;
    }

    // 文件存在时，进一步检查内容是否有效
    auto optObj = JsonUtils::readJsonObject(filePath);
    if (!optObj.has_value()) {
        return false;
    }

    return optObj.value()["hasActiveArchive"].toBool(false);
}

QMap<int, bool> SaveManager::loadAllArchiveStatus()
{
    QMap<int, bool> status;

    if (saveDir.isEmpty() || !QDir(saveDir).exists()) {
        return status;
    }

    QDir dir(saveDir);
    // 匹配 chapter_N.json 格式的文件
    QRegularExpression re("chapter_(\\d+)\\.json");

    for (const QString& fileName : dir.entryList(QDir::Files)) {
        QRegularExpressionMatch match = re.match(fileName);
        if (match.hasMatch()) {
            int chapterId = match.captured(1).toInt();
            status[chapterId] = hasArchive(chapterId);
        }
    }

    return status;
}

QString SaveManager::getArchiveFilePath(int chapterId) const
{
    return QDir(saveDir).filePath(QString("chapter_%1.json").arg(chapterId));
}
