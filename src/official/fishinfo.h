/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

#ifndef MUR_FISHINFO_H
#define MUR_FISHINFO_H

#include "../official/struct.h"
#include "../official/fishaction.h"

/**
 * @brief 机器鱼信息操作类
 * @note 原来的代码真是一坨狗屎，即便是重构了之后也
 *  同样存在一些算法上的问题，所以请不要过度依赖跟计
 *  算相关的方法。
 * @author zymelaii
 * @date 2022-06-27
 */
class CFishInfo {
public:
    /**
     * @brief 通用无效参量
     * @note 用于无效坐标与无效角度的赋值
     * @note 之所以不设置为-1是因为-1是角度的有效值
     */
    static constexpr int nil = -16;

    /**
     * @brief 默认构造
     * @note 此时鱼的信息为无效量
     */
    CFishInfo();

    /**
     * @brief 鱼的编号
     */
    int& fishId();
    const int& fishId() const;

    /**
     * @brief 鱼所属的队伍编号
     */
    int& teamId();
    const int& teamId() const;

    /**
     * @brief 鱼所属的队伍名称
     */
    const char* teamName();
    const char* const teamName() const;

    /**
     * @brief 当前鱼中心坐标
     */
    CPoint centerPos() const;

    /**
     * @brief 上一次采样时鱼的中心坐标
     */
    CPoint lastCenterPos() const;

    /**
     * @brief 鱼头（尖端）坐标
     */
    CPoint headerPos() const;

    /**
     * @brief 带参获取鱼头相关位置坐标
     * @param radius 偏移距离
     * @param direction 偏移位置与水平面的偏转角（单位为弧度）
     * @note 使用该函数前务必确保鱼头坐标已被更新！
     * @return 返回距离鱼头radius距离，偏角为direction位置的坐标
     */
    CPoint getHeaderPosAdvance(double radius, double direction);

    /**
     * @brief 第一关节转动轴坐标
     */
    CPoint jointPos() const;

    /**
     * @brief 当前鱼的偏转角（弧度制）
     */
    double currentDirection() const;

    /**
     * @brief 上一次采样时鱼的偏转角（弧度制）
     */
    double lastDirection() const;

    /**
     * @brief 速度
     */
    double velocity() const;

    /**
     * @brief 速度方向
     */
    double velocityDirection() const;
    
    /**
     * @brief 角速度
     */
    double angularVelocity() const;

    /**
     * @brief 当前鱼执行的动作
     */
    const CFishAction& currentAction() const;

    /**
     * @brief 判断当前是否设定目标点
     */
    bool hasTarget() const;

    /**
     * @brief 当前目标点坐标
     */
    CPoint targetPos() const;

    /**
     * @brief 设置新的目标点
     * @param pos 目标点坐标
     * @param direction 期望达到目标点后鱼的方位角（弧度制）
     * @return 返回当前对象
     */
    CFishInfo& setTarget(const CPoint &pos, double direction);

    /**
     * @brief 鱼中心到目标点的方向角
     * @param correct 是否启用过冲修正
     * @return 返回弧度制方向角，若目标点不存在或距离过近则返回nil
     * @note 代码逻辑较为奇怪，慎用！
     */
    double directionToTarget(bool correct = false) const;

    /**
     * @brief 计算目标点的插值坐标并重规划动作
     * @param action 待重规划的动作
     * @param apply 是否将插值规划应用至action
     * @return 插值计算是否成功，目标点不存在或距离过近时返回假
     * @note 以中心、鱼头、鱼头与目标点的中点、目标点为基计算到达
     *  目标点的插值坐标。
     * @note 仅完成对原作者代码的重写，工作稳定与否不知道，慎用！
     * @note 更多吐槽见源代码文件
     * @author zymelaii
     * @date 2022-06-28
     */
    bool calcInterPos(CFishAction& action, bool apply = true);

    /**
     * @brief 插值坐标
     * @note 请结合calcInterPos(CFishAction&, bool)的返回值判断
     *  是否应该使用插值点
     */
    CPoint interPos() const;

    /**
     * @brief 获取上一次鱼执行的动作
     * @param action 若获取成功，则上一次动作写入action
     * @note 若上一次动作不存在，则获取失败
     * @return 为真则获取成功写入action，否则不改变action的值
     */
    bool getLastAction(CFishAction& action) const;

    /**
     * @brief 设置鱼的新动作
     * @return 返回当前对象
     */
    CFishInfo& updateAction(const CFishAction &action);

    /**
     * @brief 更新信息
     * @note 该函数疑似底层接口，此处保留原函数形式以便回滚：
     *  void SetInfo(CPoint recogPos, double direction, int time)
     * @param pos 全局视觉识别到的鱼中心的坐标/期望更新的鱼的坐标
     * @param direction 鱼的方位角（弧度制）
     * @param duration 设定采样与当前状态的时间间隔（单位为时间步长TIMESTAMP）
     * @return 返回当前对象
     */
    CFishInfo& update(CPoint pos, double direction, uint8_t duration);

protected:
    /**
     * @brief 更新鱼头坐标
     * @note 由update(CPoint,double,uint8_t)方法统一更新
     * @author zymelaii
     */
    void updateHeaderPos();

    /**
     * @brief 更新第一关节转动轴坐标
     * @note 由update(CPoint,double,uint8_t)方法统一更新
     * @author zymelaii
     */
    void updateJointPos();

    /**
     * @brief 更新所有速度量（速度、角速度、速度方向）
     * @note 由update(CPoint, double, uint8_t)方法统一更新
     * @author zymelaii
     */
    void updateVelocities();

protected:
    CFishAction m_fCurrentAction;   /// 当前执行动作
    CFishAction m_fLastAction;      /// 上一次动作
    CPoint m_fTargetPos,            /// 目标点坐标
           m_fCenterP,              /// 中心坐标，即后方色标中心点
           m_fLastCenterP,          /// 前一采样时刻中心坐标
           m_fHeaderP,              /// 鱼头尖端坐标
           m_fRotateP,              /// 第一关节转动轴坐标，通常这里晃动最小，方便计算速度
           m_fLastRotateP;          /// 前一采样时刻第一关节转动轴坐标
    CPoint m_fInterP;               /// 机器鱼运动中间插值临时点
    double m_fv,                    /// 鱼的速度，单位cm/s
           m_fvdir,                 /// 速度方向，单位rad [-π,π]
           m_fav,                   /// 角速度，单位rad/s 
           m_fdirection,            /// 方向角,单位弧度[-π,π]
           m_flastdirection;        /// 前一采样时刻方向角,单位rad[-π,π]
    double m_fTargetPosDir;         /// 到目标点后期望的方向，单位rad[-π,π]
    int    m_fID;                   /// 机器鱼id号
    int    m_fTeamID;               /// 机器鱼所属的Team编号
    char   m_fTeamName[256];        /// 机器鱼所属的Team名字
    char   m_ftime;                 /// 前后两次更新信息时间间隔（待定）
};

#endif /*MUR_FISHACINFO*/