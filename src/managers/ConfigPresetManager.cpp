#include "ConfigPresetManager.h"
#include "../utils/JsonUtils.h"

#include <QJsonArray>

bool ConfigPresetManager::loadPresets(const QString& jsonPath)
{
    filePath = jsonPath;

    auto optArr = JsonUtils::readJsonArray(jsonPath);
    if (!optArr.has_value()) {
        // 文件不存在或解析失败：清空列表，视为成功
        presets.clear();
        return true;
    }

    QJsonArray arr = optArr.value();
    presets.clear();
    presets.reserve(arr.size());

    for (const auto& val : arr) {
        if (!val.isObject()) {
            continue;
        }
        GameConfig config = GameConfig::fromJson(val.toObject());
        // 忽略无效名称的预设
        if (!config.presetName.isEmpty()) {
            presets.append(config);
        }
    }

    return true;
}

bool ConfigPresetManager::savePresets()
{
    if (filePath.isEmpty()) {
        return false;
    }

    QJsonArray arr;
    for (const GameConfig& config : presets) {
        arr.append(config.toJson());
    }

    return JsonUtils::writeJsonArray(filePath, arr);
}

void ConfigPresetManager::addPreset(const GameConfig& config)
{
    if (config.presetName.isEmpty()) {
        return;
    }

    // 查找同名预设，存在则覆盖
    for (int i = 0; i < presets.size(); ++i) {
        if (presets[i].presetName == config.presetName) {
            presets[i] = config;
            return;
        }
    }

    // 不存在则追加
    presets.append(config);
}

void ConfigPresetManager::removePreset(const QString& presetName)
{
    for (int i = 0; i < presets.size(); ++i) {
        if (presets[i].presetName == presetName) {
            presets.removeAt(i);
            return;
        }
    }
}

QVector<GameConfig> ConfigPresetManager::getAllPresets() const
{
    return presets;
}

std::optional<GameConfig> ConfigPresetManager::getPreset(const QString& name) const
{
    for (const GameConfig& config : presets) {
        if (config.presetName == name) {
            return config;
        }
    }
    return std::nullopt;
}

bool ConfigPresetManager::hasPreset(const QString& presetName) const
{
    for (const GameConfig& config : presets) {
        if (config.presetName == presetName) {
            return true;
        }
    }
    return false;
}
