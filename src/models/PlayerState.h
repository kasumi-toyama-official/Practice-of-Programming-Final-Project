#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include <QJsonObject>
#include <QString>

/**
 * @brief 玩家状态数据结构
 *
 * 存储玩家战斗中的各项属性，支持 JSON 序列化用于存档。
 * 包含生命、攻击、防御、吸血等属性，以及技能加成、治疗等操作。
 */
struct PlayerState {
    int maxHp = 100;        // 生命上限
    int currentHp = 100;    // 当前生命
    int attack = 20;        // 攻击力
    int defense = 10;       // 防御力
    double lifeSteal = 0.1; // 吸血比例（如 0.15 表示 15%）

    /**
     * @brief 重置为初始值（开始新章节时调用）
     */
    void resetToInitial();

    /**
     * @brief 应用额外回合的技能加成
     * @param attribute 属性名："attack"/"maxHp"/"defense"/"heal"/"lifeSteal"
     * @param value 加成数值
     */
    void applyBuff(const QString& attribute, int value);

    /**
     * @brief 受到治疗后确保不超过上限
     * @param amount 治疗量
     */
    void heal(int amount);

    /**
     * @brief 计算实际吸血量
     * @param damageDealt 造成的伤害值
     * @return 吸血回复的生命值
     */
    int calculateLifeSteal(int damageDealt) const;

    /**
     * @brief 序列化为 JSON 对象
     */
    QJsonObject toJson() const;

    /**
     * @brief 从 JSON 对象解析
     */
    static PlayerState fromJson(const QJsonObject& obj);

    /**
     * @brief 创建初始状态（工厂方法）
     * @return 固定初始值的 PlayerState
     */
    static PlayerState createInitial();
};

#endif // PLAYERSTATE_H
