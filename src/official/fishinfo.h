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
 */
class CFishInfo {
public:
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
           m_fvdir,                 /// 速度方向，单位弧度[-π,π]
           m_fav,                   /// 角速度，单位弧度/s 
           m_fdirection,            /// 方向角,单位弧度[-π,π]
           m_flastdirection;        /// 前一采样时刻方向角,单位弧度[-π,π]
    double m_fTargetPosDir;         /// 到目标点后期望的方向，弧度[-π,π]
    int    m_fID;                   /// 机器鱼id号
    int    m_fTeamID;               /// 机器鱼所属的Team编号
    char   m_fTeamName[256];        /// 机器鱼所属的Team名字
    char   m_ftime;                 /// 前后两次更新信息时间间隔，待定
////////////////////////////////////////////////////////////////
    CFishInfo();

    BOOL IsSameAction(CFishAction action);         /**判断action是否跟目前鱼执行的动作一样*/
    BOOL GetLastAction(CFishAction& action);       /**将上一次动作赋值给action*/

    int GetCurrentSpeed();                         /**返回当前的控制速度档*/
    int GetCurrentDirection();                     /**返回当前的控制方向档*/
    int GetCurrentMode();                          /**返回当前的控制模式*/
    int GetCurrentState();                         /**返回当前的控制状态*/
    int GetCurrentDisplay();                       /**返回当前的调试状态位Display*/

    int GetFishID();                               /**返回鱼的ID*/
    int SetFishID(int id);                         /**设置鱼的ID*/

    int GetFishTeamID();                           /**返回鱼所属的Team ID*/
    int SetFishTeamID(int id);                     /**设置鱼所属的Team ID*/

    char* GetFishTeamName();                       /**返回鱼所属的Team名*/
    int SetFishTeamName(const char* fteamName);    /**设置鱼所属的Team名*/

    void GetCurrentAction(CFishAction& action);    /**获得当前的动作状态*/
    void SetAction(CFishAction& action);           /**记录机器鱼的动作*/

    CPoint GetHeaderPoint();                       /**返回鱼头尖端坐标*/
    CPoint GetHeaderPoint(int r,double dir=0.0);   /**返回距离鱼r，和鱼方向夹角为dir度（顺时针为正，逆为负,注意单位不是弧度）的点*/
    void   CalHeaderPoint();                       /**计算鱼头尖端坐标*/

    CPoint GetRotatePoint();                       /**返回第一关节转动轴坐标*/
    void   CalRotatePoint();                       /**计算第一关节转动轴坐标*/

    double GetV(void);                             /**返回速度，单位cm/s */
    double GetAV(void);                            /**返回角速度，单位弧度/s */
    double GetVdirection(void);                    /**返回速度方向，单位弧度*/
    double GetLastDiredtion(void);                 /**返回上一次鱼运动方向，单位弧度*/
    void   CalSpeedPara(void);                     /**计算速度、角速度、速度方向*/

    void   SetInfo(CPoint pt,double dir,int time); /**根据最新的图像识别信息，更新设置鱼的信息，在图像识别算法中用*/
    CPoint GetCenterPoint();                       /**返回中心点坐标*/
    CPoint GetLastCenterPoint();                   /**返回上个周期鱼中心点的位置*/
    double GetDirection( );                        /**返回鱼头方向*/


    void   SetToPos(CPoint pt,double dir);         /**设置目标点坐标及到目标点后期望的方向，dir弧度*/
    CPoint GetToPoint();                           /**返回目标点坐标*/

    double GetHeadToPosDistance();                 /**返回鱼鱼头尖端到设定目标点m_toPos的距离，单位为坐标单位，如果目标点没设定则返回-1*/
    double GetToPosDistance();                     /**返回鱼中心点到设定目标点m_toPos的距离，单位为坐标单位，如果目标点没设定则返回-1*/
    double GetToPosDirection(BOOL over=FALSE);     /**获得鱼中心点到设定目标点m_toPos的方向（-PI—PI），未设定目的点时返回-10,over为真则考虑过冲，现在一般不用考虑了*/
    double GetToPosDistance(CPoint pos,BOOL dir=FALSE);/**返回鱼中心点到pos点的距离，单位为坐标单位，如果dir为真，则返回结果是距离+角度*/

    BOOL   GetPTPAction(CFishAction& action);      /**通过路径规划，确定PTP的临时插值目标点，并得到目前到达临时目标点对应的action，待定*/
    CPoint GetInterPoint();                        /**返回由GetPTPAction确定的插值目标点坐标，待定*/
////////////////////////////////////////////////////////////////
};

#endif /*MUR_FISHACINFO*/