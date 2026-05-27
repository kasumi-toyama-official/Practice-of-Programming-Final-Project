#include "Achievement.h"

namespace {

QString categoryToString(AchievementCategory cat)
{
    return (cat == AchievementCategory::Combat) ? "combat" : "learning";
}

AchievementCategory stringToCategory(const QString& str)
{
    return (str == "combat") ? AchievementCategory::Combat : AchievementCategory::Learning;
}

} // anonymous namespace

QJsonObject Achievement::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["description"] = description;
    obj["category"] = categoryToString(category);
    obj["iconPath"] = iconPath;
    obj["unlocked"] = unlocked;
    return obj;
}

Achievement Achievement::fromJson(const QJsonObject& obj)
{
    Achievement a;
    a.id = obj["id"].toString();
    a.name = obj["name"].toString();
    a.description = obj["description"].toString();
    a.category = stringToCategory(obj["category"].toString());
    a.iconPath = obj["iconPath"].toString();
    a.unlocked = obj["unlocked"].toBool(false);
    return a;
}
