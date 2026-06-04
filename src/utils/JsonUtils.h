#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <optional>

namespace JsonUtils {

std::optional<QJsonObject> readJsonObject(const QString& filePath);

std::optional<QJsonArray> readJsonArray(const QString& filePath);

bool writeJsonObject(const QString& filePath, const QJsonObject& obj);

bool writeJsonArray(const QString& filePath, const QJsonArray& arr);

void ensureDirectoryExists(const QString& path);

} // namespace JsonUtils

#endif // JSONUTILS_H
