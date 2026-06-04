#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include <QVector>

namespace RandomUtils {

 // 会自动使用系统熵池，通常不需要手动播种，但保留此接口以兼容需要

 // 固定种子的测试场景。

void initSeed();

 // @pre min <= max

int randomInt(int min, int max);

 // 例如 {1, 2, 4}：返回0的概率 1/7，返回1的概率 2/7，返回2的概率 4/7

int weightedRandom(const QVector<int>& weights);

} // namespace RandomUtils

#endif // RANDOMUTILS_H
