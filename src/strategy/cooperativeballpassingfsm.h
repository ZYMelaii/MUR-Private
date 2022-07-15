/*!
 * \file cooperativeballpassingfsm.h
 * \brief MUR 2022 水中协作顶球策略
 * \author zymelaii
 * \date 2022-07-15
 */

#include "../official/strategyhelper.h"
#include <functional>
#include <utility>

/**
 * @brief 基于状态机运行的水中协作顶球策略类
 * @note 该策略引入了视觉图像信息以便于鱼的调试，同时其将充分利用图像信息，进行场地的定位等操作。
 * 这意味着意义不明的常量将基本不会出现在策略代码中，以便于调试与维护。
 * @attention 务必确保策略对象的生命周期不短于赛程。
 */
class CooperativeBallPassingFSM : public CStrategy {
public:
	/**
	 * @brief 集成鱼对象类
	 */
	struct Fish {
		/**
		 * @brief 重置绑定关系
		 * @param targetFish 待绑定的鱼对象
		 * @param targetAction 待绑定的鱼对象对应的动作对象
		 * @return 当前鱼对象
		 */
		Fish& reset(CFishInfo* targetFish, CFishAction* targetAction);

		//! \defgroup CooperativeBallPassingFSM鱼类预设动作
		//! @{
		/**
		 * @brief 通用点对点移动
		 * @param goal 目标点
		 * @param speed_gear 速度挡位（-1表示使用原速度）
		 * @note 指令内核为CStrategy基类的spinP2PMove函数，该方法从综合调参的方法得到，
		 * 故而对一些特殊情形，该指令的执行结果可能远低于期望。对于特殊情形，请酌情特殊化处理。
		 */
		void trivalMove(const CPoint& goal, int speed_gear);

		/**
		 * @brief 原地稳定悬停
		 */
		void stableHover();

		/**
		 * @brief 斜角移动
		 * @param goal 目标点
		 * @details 当目标点在视野左侧，顺时针运动向上侧穿越，否则逆时针运动向下侧穿越。
		 * @attention 确保目标点在前方区域
		 * @note 尚未实现
		 */
		void bevelMove(const CPoint& goal);
		//! @}

		/*! @brief 绑定的鱼对象 */
		CFishInfo* fish;

		/*! @brief 绑定鱼对应的动作对象 */
		CFishAction* action;
	};

public:
	/**
	 * @brief 策略入口
	 * @param aAction 鱼动作列表
	 * @param aFish 鱼列表
	 * @param aBallinfo 水球列表
	 * @param aChannel 通道列表
	 * @return 策略是否被成功调用（该值被忽略）
	 * @attention 调用该策略之前，务必先调用updateVisualFrame。
	 */
	virtual bool Strategy(RefArray<CFishAction> aAction, RefArray<CFishInfo> aFish,
						  RefArray<CBallInfo> aBallinfo, RefArray<OBSTAINFO> aObstacle,
						  RefArray<CHANNEL> aChannel) override;

	/**
	 * @details 所有的状态配置都将在此构造函数中完成，除此之外构造函数不做任何读写数据的操作。
	 */
	explicit CooperativeBallPassingFSM() noexcept;

	/**
	 * @brief 区域枚举值
	 */
	enum Region {
		Empty  = 0b00000000,	 /*!< 未知区域 */
		Upper  = 0b00000001,	 /*!< 上半平面 */
		Lower  = 0b00000010,	 /*!< 下半平面 */
		Left   = 0b00000100,	 /*!< 左三分平面 */
		Middle = 0b00001000,	 /*!< 中三分平面 */
		Right  = 0b00010000,	 /*!< 右三分平面 */
		UL	   = Upper | Left,	 /*!< 左上区域 */
		UM	   = Upper | Middle, /*!< 中上区域 */
		UR	   = Upper | Right,	 /*!< 右上区域 */
		LL	   = Lower | Left,	 /*!< 左下区域 */
		LM	   = Lower | Middle, /*!< 中下区域 */
		LR	   = Lower | Right,	 /*!< 右下区域 */
	};

	/**
	 * @brief 策略阶段枚举值（自动机状态定义）
	 */
	enum class Phase {
		Origin /*!< 初始起点（空状态） */
	};

	/**
	 * @brief 区域判定
	 * @param pos 待判定的点位
	 * @return 所属区域枚举值
	 */
	Region regionPredict(const CPoint& pos) const;

	/**
	 * @brief 阶段预测
	 * @return 当前所处的策略阶段
	 * @attention 具体执行的策略将依据该方法进行分发调用
	 */
	Phase phasePredict(const CPoint& a_pos, const CPoint& b_pos, const CPoint& ball_pos) const;

	/**
	 * @brief 更新视觉帧
	 * @attention 平台传入帧图像的生命周期不定，只能保证在一次策略调用中其未被销毁，
	 * 但是无法保证下一次调用其依旧存活。当访问已被销毁的帧图像时，该行为将导致平台崩溃。
	 * 所以请确保在每一次策略调用前都调用对平台的传参调用该函数。
	 * @return 是否更新成功
	 * @retval true 视觉帧已更新
	 * @retval false 视觉帧所处的内存地址未更新
	 */
	bool updateVisualFrame(IplImage* image);

	/**
	 * @brief 获取门中心坐标
	 * @param index 门的标号（0/1/2分别顺序对应三个门）
	 * @return 指定标号门的中心坐标
	 */
	CPoint door(int index) const;

	/**
	 * @brief 获取鱼实例
	 * @param index 鱼编号（0/1分别对应A鱼和B鱼）
	 * @return 请求的鱼实例
	 */
	Fish& fish(int index);

	/**
	 * @brief 获取水球对象
	 * @return 水球对象
	 */
	const CBallInfo& ball() const;

private:
	/*! @brief 目标球门中心坐标 */
	CPoint door_center_[3];

	/*! @brief 当前帧鱼实例 */
	Fish fishes_[2];

	/*! @brief 当前帧水球对象 */
	CBallInfo* ball_;

	/*! @brief 当前帧视觉图像 */
	IplImage* visframe_;
};