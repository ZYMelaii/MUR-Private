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

#ifndef MUR_EXTERNROBOT_H
#define MUR_EXTERNROBOT_H

/**
 * @param 机器鱼的数目
 * @param 最大目标数目
 */
#define MAX_FISH_NUM        16
#define MAX_TARGET_NUMBER   100

/**
 * @param POOL_WIDTH 水池宽度
 * @param POOL_HEIGHT 水池高度
 */
#define POOL_WIDTH          300
#define POOL_HEIGHT         200

/**
 * @param 鱼头到中心的距离
 * @param 旋转到中心的距离
 * @param 水球半径
 */
#define HEADTOCENTER_DIS    13.0
#define ROTATETOCENTER_DIS  5.0
#define POLO_RADIUS         7.5

/**
 * @param GRID_STEP 网格步长
 * @param TIMESTEP 时间步长（单位为秒）
 */
#define TIMESTEP            0.047
#define GRID_STEP           10

/**
 * @param REGBMP_WIDTH 图像像素宽度
 * @param REGBMP_HEIGHT 图像像素高度
 * @param REGBMP_WIDTH1 校正图像像素宽度
 * @param REGBMP_HEIGHT1 校正图像像素高度
 */
#define REGBMP_WIDTH		752
#define REGBMP_HEIGHT    	480
#define REGBMP_WIDTH1		752
#define REGBMP_HEIGHT1		480

#endif /*MUR_EXTERNROBOT_H*/