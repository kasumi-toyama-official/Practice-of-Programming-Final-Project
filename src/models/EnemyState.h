#ifndef ENEMYSTATE_H
#define ENEMYSTATE_H

#include <QJsonObject>
#include <QString>

 //
 // 存储敌人（章节守护者）的各项属性，支持 JSON 序列化用于存档。

 // 敌人使用固定伤害值攻击玩家（学习模式设计）。

struct EnemyState {
    int chapterId = 0;          // 所属章节
    QString name;               // 敌人名称（如"变量守护者"）
    int maxHp = 80;             // 生命上限
    int currentHp = 80;         // 当前生命
    int attack = 15;            // 攻击力（备用字段，实际使用 fixedDamage）
    int defense = 5;            // 防御力
    int fixedDamage = 10;       // 每回合固定攻击伤害

    bool isDefeated() const { return currentHp <= 0; }

    QJsonObject toJson() const;

    static EnemyState fromJson(const QJsonObject& obj);
};

#endif // ENEMYSTATE_H
