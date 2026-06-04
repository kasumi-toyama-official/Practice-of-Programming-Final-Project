#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include <QJsonObject>
#include <QString>

 //
 // 存储玩家战斗中的各项属性，支持 JSON 序列化用于存档。

 // 包含生命、攻击、防御、吸血等属性，以及技能加成、治疗等操作。

struct PlayerState {
    int maxHp = 100;        // 生命上限
    int currentHp = 100;    // 当前生命
    int attack = 20;        // 攻击力
    int defense = 10;       // 防御力
    double lifeSteal = 0.1; // 吸血比例（如 0.15 表示 15%）

    void resetToInitial();

    void applyBuff(const QString& attribute, int value);

    void heal(int amount);

    int calculateLifeSteal(int damageDealt) const;

    QJsonObject toJson() const;

    static PlayerState fromJson(const QJsonObject& obj);

    static PlayerState createInitial();
};

#endif // PLAYERSTATE_H
