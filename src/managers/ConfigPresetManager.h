#ifndef CONFIGPRESETMANAGER_H
#define CONFIGPRESETMANAGER_H

#include "models/GameConfig.h"
#include <QVector>
#include <QString>
#include <optional>

/**
 * @brief 配置预设管理器
 *
 * 管理玩家保存的多组题目配置预设，支持增删改查和持久化到 JSON 文件。
 * 每个预设通过 presetName 唯一标识，同名预设添加时会覆盖旧值。
 */
class ConfigPresetManager {
public:
    /**
     * @brief 从 JSON 文件加载已保存的配置预设
     * @param jsonPath 预设文件路径
     * @return 加载成功返回 true（文件不存在视为成功，内部为空列表）
     */
    bool loadPresets(const QString& jsonPath);

    /**
     * @brief 保存所有预设到 JSON 文件
     * @return 写入成功返回 true
     */
    bool savePresets();

    /**
     * @brief 添加一个新预设
     * @param config 要添加的配置
     * @note 如果 presetName 与已有预设重复，则覆盖旧预设
     */
    void addPreset(const GameConfig& config);

    /**
     * @brief 删除指定名称的预设
     * @param presetName 预设名称
     */
    void removePreset(const QString& presetName);

    /**
     * @brief 获取所有预设
     * @return 配置预设列表
     */
    QVector<GameConfig> getAllPresets() const;

    /**
     * @brief 按名称获取预设
     * @param name 预设名称
     * @return 对应的配置。若找不到则返回 std::nullopt
     */
    std::optional<GameConfig> getPreset(const QString& name) const;

    /**
     * @brief 检查是否存在指定名称的预设
     * @param presetName 预设名称
     * @return true 如果存在
     */
    bool hasPreset(const QString& presetName) const;

private:
    QVector<GameConfig> presets;
    QString filePath;
};

#endif // CONFIGPRESETMANAGER_H
