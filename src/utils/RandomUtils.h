#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include <QVector>

namespace RandomUtils {

/**
 * @brief 初始化随机数种子
 * @note 在 main.cpp 中调用一次即可。Qt 5.10+ 的 QRandomGenerator::global()
 *       会自动使用系统熵池，通常不需要手动播种，但保留此接口以兼容需要
 *       固定种子的测试场景。
 */
void initSeed();

/**
 * @brief 生成 [min, max] 范围内的随机整数（包含两端）
 * @param min 最小值
 * @param max 最大值
 * @return 范围内的随机整数
 * @pre min <= max
 */
int randomInt(int min, int max);

/**
 * @brief 根据权重数组随机选择索引
 * @param weights 权重数组，如 {1, 2, 4}
 * @return 选中的索引。weights 为空时返回 -1
 * @note 返回 i 的概率 = weights[i] / sum(weights)
 *       例如 {1, 2, 4}：返回0的概率 1/7，返回1的概率 2/7，返回2的概率 4/7
 */
int weightedRandom(const QVector<int>& weights);

} // namespace RandomUtils

#endif // RANDOMUTILS_H
