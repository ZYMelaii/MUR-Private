/**
 * @brief 提供用于MUR的基础数学函数
 * @author zymelaii
 * @date 2022-06-26
 */

#ifndef MATHTOOL_H
#define MATHTOOL_H

#include "common.h"
#include <math.h>

/**
 * @brief 将角度值规范至(-π,π]
 * @param angle 角度值
 * @param is_radian 输入输出的角度单位
 */
double normalizeAngle(double angle, bool is_radian = true) {
	const double unit = is_radian ? 2 * M_PI : 360.0;
	if (angle < 0) {
		return angle + (int)(-angle / unit) * unit;
	} else {
		return angle - (int)(angle / unit) * unit;
	}
}

/**
 * @brief 获取两点距离
 * @param a 点A
 * @param b 点B
 */
double getDistance(const CPoint &a, const CPoint &b) {
	const double dx = a.x - b.x, dy = a.y - b.y;
	return sqrt(dx * dx + dy * dy);
}

/**
 * @brief 获取向量与水平面的夹角
 * @param origin 向量起点
 * @param dest 向量终点
 * @note 单位为弧度，角度区间为[-π,π]
 */
double getVecAngle(const CPoint &origin, const CPoint &dest) {
	CPoint vec(dest.x - origin.x, dest.y - origin.x);
	return atan2(vec.y, vec.x);
}

#endif /*MATHTOOL_H*/