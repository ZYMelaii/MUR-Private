/**
 * @brief 提供用于MUR的基础数学函数
 * @author zymelaii
 * @date 2022-06-26
 */

#ifndef MATHTOOL_H
#define MATHTOOL_H

#include "common.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 将角度值规范至(-π,π]
 * @param angle 角度值
 * @param is_radian 输入输出的角度单位
 */
double normalizeAngle(double angle, bool is_radian = true);

/**
 * @brief 获取两点距离
 * @param a 点A
 * @param b 点B
 */
double getDistance(const CPoint &a, const CPoint &b);

/**
 * @brief 获取向量与水平面的夹角
 * @param origin 向量起点
 * @param dest 向量终点
 * @note 单位为弧度，角度区间为[-π,π]
 */
double getVecAngle(const CPoint &origin, const CPoint &dest);

#endif /*MATHTOOL_H*/