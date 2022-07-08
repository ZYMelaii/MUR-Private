#include "../official/strategyhelper.h"

/**
 * @note 策略所涉及的内存皆存储于MURobotCtrl程序中，
 *  切记基于传入的参数进行修改。
 */

/**
 * @brief 朴素水中协作顶球策略
 * @note 该策略时间相干，请确保策略的生命周期不短于赛程
 * @author zymelaii
 * @date 2022-07-04
 */
class CooperativeBallPassing : public CStrategy {
public:
	/**
	 * @brief 默认构造，作必要的初始化
	 * @param width 识别图像的宽度
	 * @param height 识别图像的高度
	 */
	CooperativeBallPassing(int width, int height);

	/**
	 * @brief 朴素水中协作顶球策略
	 * @note 策略按预先设计的流程运行，详见对应源码
	 */
	virtual bool Strategy(
		RefArray<CFishAction> aAction,
		RefArray<CFishInfo>   aFish,
		RefArray<CBallInfo>   aBallinfo,
		RefArray<OBSTAINFO>   aObstacle,
		RefArray<CHANNEL>     aChannel) override;

public:
	/**
	 * @brief 稳定悬停指令
	 * @note 原地转圈
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void stableHoverInstruct(CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 斜角移动指令
	 * @note 确保目标点在前方区域
	 * @note 当目标点在视野左侧，顺时针运动向上侧穿越；
	 *  否则逆时针运动向下侧穿越。
	 * @param goal 目标点
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void bevelMove(const CPoint &goal, CFishInfo &fish, CFishAction &action);

	/**
	 * @brief 水池区域划分
	 */
	enum Region {
		Empty  = 0b00000000, /// 未知区域
		Upper  = 0b00000001, /// 上半平面
		Lower  = 0b00000010, /// 下半平面
		Left   = 0b00000100, /// 左三分平面
		Middle = 0b00001000, /// 中三分平面
		Right  = 0b00010000, /// 右三分平面
		UL = Upper | Left,   /// 左上区域
		UM = Upper | Middle, /// 中上区域
		UR = Upper | Right,  /// 右上区域
		LL = Lower | Left,   /// 左下区域
		LM = Lower | Middle, /// 中下区域
		LR = Lower | Right,  /// 右下区域
	};

	/**
	 * @brief 区域判定
	 * @param pos 待判定的点位
	 * @return 所属区域的Region标志
	 */
	Region regionPredict(const CPoint &pos) const;

protected:
	/**
	 * @brief 策略阶段划分
	 * @note 策略划分为三个小阶段策略执行，三个阶段由下列策略
	 *  函数具体描述。
	 */
	enum class Phase { Stage_1st, Stage_2nd, Stage_3rd };

	/**
	 * @brief 阶段预测
	 * @note 具体执行的策略将依据该方法进行分发调用。
	 * @return 当前应当执行的策略阶段
	 */
	Phase phasePredict(
		const CPoint &a_pos,
		const CPoint &b_pos,
		const CPoint &ball_pos) const;

	/**
	 * @brief 第一阶段策略
	 * @note A鱼顶开球门进入下平面区并保证B鱼有足够活动空间，
	 *  维护水球的稳定位置应在此阶段进行，同时B鱼应当保证A进、
	 *  入下平面的过程中有足够的活动空间。
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_1st(
		CFishInfo &fish_a, CFishAction &action_a,
		CFishInfo &fish_b, CFishAction &action_b,
		CBallInfo &ball);

	/**
	 * @brief 第二阶段策略
	 * @note 此时A鱼已经进入下平面，B鱼开始顶球从一号门将球
	 *  送入上平面并继续送往二号门左上方；同时A鱼继续向二号
	 *  门移动，顶开球门在水池右上角等待。重点注意A鱼与B鱼的
	 *  任务完成同步率。
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_2nd(
		CFishInfo &fish_a, CFishAction &action_a,
		CFishInfo &fish_b, CFishAction &action_b,
		CBallInfo &ball);

	/**
	 * @brief 第三阶段策略
	 * @note A鱼已经顶开球门进入上平面右上角等待，B鱼将球送往
	 *  下平面并准备射门，同时A鱼等待B鱼送球完成并堵住二号门。
	 * @author zymelaii
	 * @date 2022-07-05
	 */
	void StageInstruct_3rd(
		CFishInfo &fish_a, CFishAction &action_a,
		CFishInfo &fish_b, CFishAction &action_b,
		CBallInfo &ball);

protected:
	/**
	 * @brief 场地初始化
	 * @note 方法将等待通道（门卡）设定完毕并初始化场地信息，
	 *  等待的结束条件为门1、门2、球门位置皆设定完毕且完成鱼
	 *  和水球的初始标定。
	 * @return 是否完成初始化
	 */
	bool init(
		const RefArray<CFishInfo> &aFish,
		const RefArray<CBallInfo> &aBallinfo,
		const RefArray<CHANNEL>   &aChannel);

	/**
	 * @brief 追踪并更新备份实物鱼与水球信息
	 * @note 该函数将对传参的有效性进行检测，对于无效信息所
	 *  属的成员将利用模拟计算进行数据的更新。
	 * @return 是否应用模拟计算
	 */
	bool trackAndUpdate(
		CFishInfo &fish_a, CFishAction &action_a,
		CFishInfo &fish_b, CFishAction &action_b,
		CBallInfo &ball);

private:
	/**
	 * @brief 是否完成初始化
	 * @note 所有操作都应在初始化完成之后进行
	 */
	bool initialized_;

	/**
	 * @brief 水池宽高
	 */
	int width_, height_;

	/**
	 * @brief 三个球门的中心位置
	 */
	CPoint door_center_[3];

	const int index_[3] = {
		0, /// A鱼索引
		1, /// B鱼索引
		0, /// 水球索引
	};
};