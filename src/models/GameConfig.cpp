#include "GameConfig.h"
#include "../utils/RandomUtils.h"

int GameConfig::rollDifficulty() const
{
    QVector<int> weights = {easyWeight, mediumWeight, hardWeight};
    return RandomUtils::weightedRandom(weights);
}

int GameConfig::rollQuestionType() const
{
    QVector<int> weights = {choiceWeight, codeCompletionWeight, codeWeight};
    return RandomUtils::weightedRandom(weights);
}

bool GameConfig::meetsThreshold() const
{
    int totalDifficulty = easyWeight + mediumWeight + hardWeight;
    if (totalDifficulty <= 0) {
        return false;
    }

    // 简单题占比 <= 60%
    double easyRatio = static_cast<double>(easyWeight) / totalDifficulty;
    if (easyRatio > 0.6) {
        return false;
    }

    int totalType = choiceWeight + codeCompletionWeight + codeWeight;
    if (totalType <= 0) {
        return false;
    }

    // 选择题占比 <= 80%
    double choiceRatio = static_cast<double>(choiceWeight) / totalType;
    if (choiceRatio > 0.8) {
        return false;
    }

    return true;
}

QJsonObject GameConfig::toJson() const
{
    QJsonObject obj;
    obj["easyWeight"] = easyWeight;
    obj["mediumWeight"] = mediumWeight;
    obj["hardWeight"] = hardWeight;
    obj["choiceWeight"] = choiceWeight;
    obj["codeCompletionWeight"] = codeCompletionWeight;
    obj["codeWeight"] = codeWeight;
    obj["presetName"] = presetName;
    return obj;
}

GameConfig GameConfig::fromJson(const QJsonObject& obj)
{
    GameConfig c;
    c.easyWeight = obj["easyWeight"].toInt(1);
    c.mediumWeight = obj["mediumWeight"].toInt(1);
    c.hardWeight = obj["hardWeight"].toInt(1);
    c.choiceWeight = obj["choiceWeight"].toInt(1);
    c.codeCompletionWeight = obj["codeCompletionWeight"].toInt(obj["blankWeight"].toInt(0));
    c.codeWeight = obj["codeWeight"].toInt(0);
    c.presetName = obj["presetName"].toString();
    return c;
}

GameConfig GameConfig::getDefault()
{
    GameConfig c;
    c.easyWeight = 1;
    c.mediumWeight = 1;
    c.hardWeight = 1;
    c.choiceWeight = 1;
    c.codeCompletionWeight = 0;
    c.codeWeight = 0;
    c.presetName = "默认配置";
    return c;
}
