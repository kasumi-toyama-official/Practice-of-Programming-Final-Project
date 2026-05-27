#include "JsonUtils.h"

#include <QFile>
#include <QJsonDocument>
#include <QDir>

namespace JsonUtils {

std::optional<QJsonObject> readJsonObject(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return std::nullopt;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return std::nullopt;
    }

    return doc.object();
}

std::optional<QJsonArray> readJsonArray(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return std::nullopt;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        return std::nullopt;
    }

    return doc.array();
}

bool writeJsonObject(const QString& filePath, const QJsonObject& obj)
{
    ensureDirectoryExists(QFileInfo(filePath).path());

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    qint64 written = file.write(data);
    file.close();

    return written == data.size();
}

bool writeJsonArray(const QString& filePath, const QJsonArray& arr)
{
    ensureDirectoryExists(QFileInfo(filePath).path());

    QJsonDocument doc(arr);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    qint64 written = file.write(data);
    file.close();

    return written == data.size();
}

void ensureDirectoryExists(const QString& path)
{
    QDir dir;
    if (!dir.exists(path)) {
        dir.mkpath(path);
    }
}

} // namespace JsonUtils
