#include "RandomUtils.h"

#include <QRandomGenerator>
#include <QTime>

namespace RandomUtils {

void initSeed()
{
    Q_UNUSED(QTime::currentTime().msec())
}

int randomInt(int min, int max)
{
    Q_ASSERT(min <= max);
    if (min >= max) {
        return min;
    }
    return min + QRandomGenerator::global()->bounded(max - min + 1);
}

int weightedRandom(const QVector<int>& weights)
{
    if (weights.isEmpty()) {
        return -1;
    }

    // 计算权重总和
    int total = 0;
    for (int w : weights) {
        if (w > 0) {
            total += w;
        }
    }

    if (total <= 0) {
        return -1;
    }

    // 生成 [1, total] 的随机数
    int roll = 1 + QRandomGenerator::global()->bounded(total);

    // 按权重区间判定
    int accumulated = 0;
    for (int i = 0; i < weights.size(); ++i) {
        if (weights[i] > 0) {
            accumulated += weights[i];
            if (roll <= accumulated) {
                return i;
            }
        }
    }

    // 理论上不会到达此处，作为安全回退返回最后一个有效索引
    return weights.size() - 1;
}

} // namespace RandomUtils
