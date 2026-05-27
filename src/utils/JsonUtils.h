#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <optional>

namespace JsonUtils {

/**
 * @brief 从指定路径的文件读取 JSON 对象
 * @param filePath 文件路径
 * @return 成功返回 QJsonObject，失败返回 std::nullopt
 */
std::optional<QJsonObject> readJsonObject(const QString& filePath);

/**
 * @brief 从指定路径的文件读取 JSON 数组
 * @param filePath 文件路径
 * @return 成功返回 QJsonArray，失败返回 std::nullopt
 */
std::optional<QJsonArray> readJsonArray(const QString& filePath);

/**
 * @brief 将 JSON 对象写入指定文件（格式化输出）
 * @param filePath 文件路径
 * @param obj 要写入的 JSON 对象
 * @return 写入成功返回 true
 */
bool writeJsonObject(const QString& filePath, const QJsonObject& obj);

/**
 * @brief 将 JSON 数组写入指定文件（格式化输出）
 * @param filePath 文件路径
 * @param arr 要写入的 JSON 数组
 * @return 写入成功返回 true
 */
bool writeJsonArray(const QString& filePath, const QJsonArray& arr);

/**
 * @brief 确保指定目录存在，不存在则自动创建
 * @param path 目录路径
 */
void ensureDirectoryExists(const QString& path);

} // namespace JsonUtils

#endif // JSONUTILS_H
