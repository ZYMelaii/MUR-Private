/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

/**
 * @name:          多水中机器人控制无色标平台
 * @description:   版权所有，授权使用
 * @version:       1.2.0.0
 * @copyright:     国际水中机器人联盟
 * @author:        ZhangX,XiongML
 * @date:          14-04-04 16:55
 */

#ifndef MUR_BALLINFO_H
#define MUR_BALLINFO_H

#include "../official/struct.h"

/**
 * @brief 水球信息操作类
 */
class CBallInfo : public BALLINFO
{
public:
    /**
     * @brief 默认构造
     * @note 此时水球的信息是无效量
     * @author zymelaii
     */
    CBallInfo();

    /**
     * @brief 获取水球当前位置坐标信息
     * @author zymelaii
     */
    CPoint getCenter() const;

    /**
     * @brief 获取水球最近一次的位置坐标信息
     * @author zymelaii
     */
    CPoint getPrevCenter() const;

    /**
     * @brief 获取水球速度
     * @author zymelaii
     */
    double getSpeed() const;

    /**
     * @brief 获取速度方向与水平面的夹角
     * @note 以弧度为单位
     * @author zymelaii
     */
    double getDirection() const;

    /**
     * @brief 获取水球半径
     * @author zymelaii
     */
    double getRadius() const;

    /**
     * @brief 计算撞击点
     * @param goal 水球被撞击后期望到达的目标位置
     * @author zymelaii
     */
    CPoint calcHitPoint(const CPoint &goal) const;

    /**
     * @brief 根据时间步长更新水球运动参量
     * @note 时间步长由../official/externrobot.h中的TIMESTEP宏定义
     * @author zymelaii
     */
    CBallInfo& updateVelocity();

    /**
     * @brief 设置并更新水球信息
     * @param pos 新的水球中心坐标
     * @param duration 待设定属性与当前状态的时间间隔（单位为时间步长TIMESTAMP）
     * @param r 水球半径
     * @author zymelaii
     */
    CBallInfo& setProperty(CPoint pos, uint8_t duration, double r);

    /**
     * @brief 获取水球状态
     * @author zymelaii
     */
    CBallInfo& getProperty(BALLINFO &info);

    /**
     * @brief 水球状态赋值
     * @author zymelaii
     */
    CBallInfo& operator=(const BALLINFO &other);
};

#endif /*MUR_BALLINFO_H*/