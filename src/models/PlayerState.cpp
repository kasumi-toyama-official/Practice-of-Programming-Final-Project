#include "PlayerState.h"

void PlayerState::resetToInitial()
{
    *this = createInitial();
}

void PlayerState::applyBuff(const QString& attribute, int value)
{
    if (attribute == "attack") {
        attack += value;
    } else if (attribute == "maxHp") {
        maxHp += value;
        currentHp += value; // 增加上限时，当前生命同步增加
    } else if (attribute == "defense") {
        defense += value;
    } else if (attribute == "heal") {
        heal(value);
    } else if (attribute == "lifeSteal") {
        lifeSteal += value * 0.01; // value 为百分比数值，如 5 表示 +5%
    }
}

void PlayerState::heal(int amount)
{
    currentHp += amount;
    if (currentHp > maxHp) {
        currentHp = maxHp;
    }
}

int PlayerState::calculateLifeSteal(int damageDealt) const
{
    return static_cast<int>(damageDealt * lifeSteal);
}

QJsonObject PlayerState::toJson() const
{
    QJsonObject obj;
    obj["maxHp"] = maxHp;
    obj["currentHp"] = currentHp;
    obj["attack"] = attack;
    obj["defense"] = defense;
    obj["lifeSteal"] = lifeSteal;
    return obj;
}

PlayerState PlayerState::fromJson(const QJsonObject& obj)
{
    PlayerState p;
    p.maxHp = obj["maxHp"].toInt(100);
    p.currentHp = obj["currentHp"].toInt(100);
    p.attack = obj["attack"].toInt(20);
    p.defense = obj["defense"].toInt(10);
    p.lifeSteal = obj["lifeSteal"].toDouble(0.1);
    return p;
}

PlayerState PlayerState::createInitial()
{
    PlayerState p;
    p.maxHp = 100;
    p.currentHp = 100;
    p.attack = 20;
    p.defense = 10;
    p.lifeSteal = 0.1;
    return p;
}
