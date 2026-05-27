#include "EnemyState.h"

QJsonObject EnemyState::toJson() const
{
    QJsonObject obj;
    obj["chapterId"] = chapterId;
    obj["name"] = name;
    obj["maxHp"] = maxHp;
    obj["currentHp"] = currentHp;
    obj["attack"] = attack;
    obj["defense"] = defense;
    obj["fixedDamage"] = fixedDamage;
    return obj;
}

EnemyState EnemyState::fromJson(const QJsonObject& obj)
{
    EnemyState e;
    e.chapterId = obj["chapterId"].toInt(0);
    e.name = obj["name"].toString();
    e.maxHp = obj["maxHp"].toInt(80);
    e.currentHp = obj["currentHp"].toInt(80);
    e.attack = obj["attack"].toInt(15);
    e.defense = obj["defense"].toInt(5);
    e.fixedDamage = obj["fixedDamage"].toInt(10);
    return e;
}
