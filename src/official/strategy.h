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

#ifndef MUR_STRATEGY_H
#define MUR_STRATEGY_H

#include "../official/struct.h"
#include "../official/ballinfo.h"
#include "../official/fishaction.h"
#include "../official/fishinfo.h"
#include "../refarray.h"

/**
 * @brief 机器鱼策略控制算法类
 */
class CStrategy {
private:
	/**
	 * @brief 旋转模态
	 * @value 0 通用转弯
	 * @value 1 原始转弯
	 */
	int turn_mode_;

	/**
	 * @brief 运动周期
	 * @note 记录一个鱼动作的时间状态，超出该值时应当被正确归零以进入状态循环
	 */
	const int state_period_;

public:
	CStrategy();
	virtual ~CStrategy();

	/**
	 * @brief 原地转圈指令
	 * @param aFish 鱼列表
	 * @param aAction 鱼动作信息列表
	 * @param id 要控制的鱼的编号
	 */
	void stop(RefArray<CFishInfo> &aFish, RefArray<CFishAction> &aAction, int id);

	/**
	 * @brief 基础行动指令
	 * @param aFish 鱼列表
	 * @param aAction 鱼动作信息列表
	 * @param id 要控制的鱼的编号
	 * @param speed 速度档位
	 * @param direction 方向档位
	 * @param mode 转弯模态：0为通用转弯；1为原始转弯
	 */
	void go(RefArray<CFishInfo> &aFish, RefArray<CFishAction> &aAction,
		int id, int speed, int direction, int mode);

	/**
	 * @brief 平缓转向的点对点移动指令
	 * @note 本函数的平缓乃通过对当前鱼的头身偏角调参实现，
	 *  其中细分为近距模式与远距离模式。
	 * @note 函数为对祖传的重构以提升可读性。
	 * @author zymelaii
	 * @date 2022-06-26
	 */
	static void spinP2PMove(const CPoint &goal, CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 自左侧平缓转向点对点移动指令
	 * @author zymelaii
	 * @date 2022-06-26
	 */
	static void spinP2PMoveFromLeft(const CPoint &goal, CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 自右侧平缓转向点对点移动指令
	 * @note 同spinP2PMove(const CPoint&,CFishInfo&,CFishAction&)
	 * @author zymelaii
	 * @date 2022-06-26
	 */
	static void spinP2PMoveFromRight(const CPoint &goal, CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 点对点移动指令
	 * @note 这是一个神奇的函数，我不知道它存在的意义是什么，但是它内部
	 *  调用了CStrategy::spinP2PMove并且他妈的还引用了一个全局变量。
	 * @note 因为其中的全局变量还未被重构后的代码正确处理，所以先不要使用它。
	 * @note 以上两条note作废
	 * @return 返回当前鱼与目标点的之间的距离是否在阈值（默认为30）之内，
	 *  小于阈值返回真。
	 * @author zymelaii
	 * @date 2022-06-26
	 */
	static bool P2PMove(const CPoint &goal, CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 策略函数
	 * @note 具体策略应在子类中重写
	 * @return 策略是否成功执行
	 * @author zymelaii
	 * @date 2022-06-27
	 */
	virtual bool Strategy(
		RefArray<CFishAction> aAction,
		RefArray<CFishInfo>   aFish,
		RefArray<CBallInfo>   aBallinfo,
		RefArray<OBSTAINFO>   aObstacle,
		RefArray<CHANNEL>     aChannel) = 0;
};

#endif /*MUR_STRATEGY_H*/