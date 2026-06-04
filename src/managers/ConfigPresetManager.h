#ifndef CONFIGPRESETMANAGER_H
#define CONFIGPRESETMANAGER_H

#include "models/GameConfig.h"
#include <QVector>
#include <QString>
#include <optional>

 //
 // 管理玩家保存的多组题目配置预设，支持增删改查和持久化到 JSON 文件。

 // 每个预设通过 presetName 唯一标识，同名预设添加时会覆盖旧值。

class ConfigPresetManager {
public:
    bool loadPresets(const QString& jsonPath);

    bool savePresets();

    void addPreset(const GameConfig& config);

    void removePreset(const QString& presetName);

    QVector<GameConfig> getAllPresets() const;

    std::optional<GameConfig> getPreset(const QString& name) const;

    bool hasPreset(const QString& presetName) const;

private:
    QVector<GameConfig> presets;
    QString filePath;
};

#endif // CONFIGPRESETMANAGER_H
