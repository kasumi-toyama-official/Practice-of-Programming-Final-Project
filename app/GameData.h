#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QJsonObject>
#include "models/Question.h"

struct QuestionData
{
    QuestionType type;
    QString description;
    QStringList options;
    QString codeTemplate;
    int tolerance;
    int correctOptionIndex;   // 选择题正确答案索引（0-based），非选择题填 -1
    int id;                   // 题目唯一编号（来自题库）
    QString explanation;      // 题目解析
};

struct SkillData
{
    int id;
    QString name;
    QString iconPath;
    QString attribute;
    Difficulty difficulty;
    int easyBonus;
    int mediumBonus;
    int hardBonus;
};

struct Stats
{
    int hp;
    int maxHp;
    int attack;
    int defence;
    double lifesteal;
    int passCards;
    int totalDamage;
    int round;
    QStringList buffs;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["hp"] = hp;
        obj["maxHp"] = maxHp;
        obj["attack"] = attack;
        obj["defence"] = defence;
        obj["lifesteal"] = lifesteal;
        return obj;
    }
    static Stats fromJson(const QJsonObject& obj) {
        Stats s;
        s.hp = obj["hp"].toInt(100);
        s.maxHp = obj["maxHp"].toInt(100);
        s.attack = obj["attack"].toInt(20);
        s.defence = obj["defence"].toInt(10);
        s.lifesteal = obj["lifesteal"].toDouble(0.0);
        s.passCards = 0;
        s.totalDamage = 0;
        s.round = 0;
        return s;
    }
};

#endif // GAMEDATA_H
